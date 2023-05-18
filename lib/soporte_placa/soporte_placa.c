#include <soporte_placa.h>
#include <stm32f1xx.h>
#include <stdint.h>

// Implementación

/**
 * @brief Rutina de servicio de interrupción de timer SysTick
 * 
 */
void SysTick_Handler(void);

/* Inicialización general */

void SP_init(void){
    // Ver documentación CMSIS
    // https://arm-software.github.io/CMSIS_5/Core/html/group__system__init__gr.html#gae0c36a9591fe6e9c45ecb21a794f0f0f
    SystemCoreClockUpdate();
    
    uint32_t const frecuencia_hertz = SystemCoreClock;
    uint32_t const cuentas_por_milisgundo = frecuencia_hertz/1000;

    // https://arm-software.github.io/CMSIS_5/Core/html/group__SysTick__gr.html#gabe47de40e9b0ad465b752297a9d9f427
    SysTick_Config(cuentas_por_milisgundo); // Configura SysTick y la interrupción
}


/* Temporización */

/**
 * @brief Variable actualizada una vez por milisegundo en el handler
 * de interrupción del timer del sistema (SysTick)
 * 
 */
static uint32_t volatile ticks;

void SP_delay(uint32_t tiempo){
    uint32_t const ticks_inicial = ticks;
    uint32_t tiempo_transcurrido = ticks - ticks_inicial;
    while(tiempo_transcurrido < tiempo){
        // https://arm-software.github.io/CMSIS_5/Core/html/group__intrinsic__CPU__gr.html#gaed91dfbf3d7d7b7fba8d912fcbeaad88
        __WFI();
        tiempo_transcurrido = ticks - ticks_inicial;
    }

}


void SysTick_Handler(void){
    ++ticks;
}

/* GPIO */

/** @brief Contiene toda la información necesaria para 
 *  configurar y usar un pin en un puerto GPIO
 */
typedef struct Pin{
    /* @brief Dirección base del puerto */
    GPIO_TypeDef * puerto;
    /* @brief Número de pin*/
    int nrPin;
}Pin;

/** @brief Tabla que vincula los handle de pin con los objetos Pin que
 * contienen la información necesaria para utilizar cada pin.
 */
static Pin const pines[SP_HPIN_LIMITE] = {
    [SP_PB9] ={.puerto=GPIOB,.nrPin=9},
    [SP_PC13]={.puerto=GPIOC,.nrPin=13}
};

/**
 * @brief Obtiene un puntero a Pin a partir de su handle
 * 
 * @param hPin Handle
 * @return Pin const* Puntero al objeto Pin (solo lectura) 
 */
static Pin const * pinDeHandle(SP_HPin hPin){
    return &pines[hPin];
}
/**
 * @brief Calcula la posición en del bit de habilitación
 * del puerto en APB2_ENR a partir de su dirección en memoria.
 */

/**
 * @brief Habilita el reloj de un puerto GPIO
 * @note Debe ser llamada antes de intentar usar el puerto
 * por primera vez.
 * @param puerto Puntero al puerto GPIO 
 * @return uint32_t Máscara de habilitación de reloj
 */
static void habilitaRelojPuerto(GPIO_TypeDef const *puerto){
    enum{
        /** @brief Deducido del mapa de memoria, hoja de datos stm32f103xx */ 
        TAMANO_ENTRADA_APB2 = (uintptr_t)(GPIOB)-(uintptr_t)(GPIOA)
    };
    int const INDICE_EN_APB2 = ((((uintptr_t)(puerto) - APB2PERIPH_BASE) / TAMANO_ENTRADA_APB2));
    /* APB2ENR es un mapa de bits del bus APB2 considerado como arreglo. Esto no está documentado
    explicitamente pero debiera aplicar a toda la familia STM32F101xx, STM32F102xx, STM32F103xx, 
    STM32F105xx y STM32F107xx */
    RCC->APB2ENR |= 1 << INDICE_EN_APB2;
}
// ... continúa implementación

/**
 * @brief Escribe los bits de modo en la posición adecuada
 * de CRL o CRH según el pin
 * 
 * @param pin 
 * @param bits_modo 
 */
static void config_modo(Pin const *pin, int bits_modo){
    // Ver Manual de referencia de la familia sec. 9.2.1/.2
    int const offset = (pin->nrPin % 8)*4;
    uint32_t const mascara = 0xF; // 4 bits de configuración
    if (pin->nrPin < 8){
        pin->puerto->CRL =  (pin->puerto->CRL & ~(mascara << offset))
                          | ((bits_modo & mascara)<<offset); 
    }else{ // 8..15
        pin->puerto->CRH =  (pin->puerto->CRL & ~(mascara << offset))
                          | ((bits_modo & mascara)<<offset); 
    }
}

void SP_Pin_setModo(SP_HPin hPin,SP_Pin_Modo modo){
    // Ver Manual de referencia de la familia sec. 9.2.1/.2
    enum ConfigsPin{
        /** 
         * Bits[1:0]: Modo E/S, 00 es modo entrada
         * Bits[3:2]: Configuración de entrada, 01 es entrada flotante
         */
        ENTRADA_FLOTANTE = 0b0100,
        /** 
         * Bits[1:0]: Modo E/S, 00 es modo entrada
         * Bits[3:2]: Configuración de entrada, 10 es entrada con pull-up/pull-dn
         */
        ENTRADA_PULLUP_PULLDN = 0b1000,
        /** 
         * Bits[1:0]: Modo E/S, 10 es modo salida con frec. máxima de 2MHz
         * Bits[3:2]: Configuración de salida, 00 es salida de propósito general normal (push/pull)
         */
        SALIDA_2MHz = 0b0010,
        /** 
         * Bits[1:0]: Modo E/S, 10 es modo salida con frec. máxima de 2MHz
         * Bits[3:2]: Configuración de salida, 01 es salida de propósito general open drain
         */
        SALIDA_2MHz_OPEN_DRAIN = 0b0110
    };
    if(hPin >= SP_HPIN_LIMITE) return; // debiera generar un error.
                                       //si el valor de hPin es mayor o igual que SP_HPIN_LIMITE, entonces se
                                       //retorna de la funcion sin hacer nada. En otras palabras si hPin no esta
                                       //dentro del rango valido de identificadores de hadware de los pines, la 
                                       //funcion no hace nada.



    Pin const * self = pinDeHandle(hPin); // se declara un puntero constante "self" a la estructura "Pin"
                                          // que corresponde al pin identificado por hPin. la funcion
                                          // pinDeHandle se encarga de devolver el puntero a dicha estructura.

    __disable_irq(); // deshabilita las interrupciones durante la ejecucion de la funcion
                    // esto evita que las interrupciones afecten la config. del pin.

    habilitaRelojPuerto(self->puerto); // la funcion habilitaRelojPuerto habilita el reloj del puerto al que esta
                                       // conectado el pin. esto se hace para que le pin funcione bien.

    switch (modo)  // switch se utiliza para seleccionar la configuracion del pin.
    {
    // dependiendo el valor de "modo" se llama a la funcion "config_modo" con el valor correspondiente de 
    // "configsPin" lo que configura el pin en el modo apropiado.

    case SP_PIN_ENTRADA:
        config_modo(self,ENTRADA_FLOTANTE);
    break;case SP_PIN_ENTRADA_PULLUP:  // se establece el pin de entrada como PULLUP.
        config_modo(self,ENTRADA_PULLUP_PULLDN);
        //self->puerto->ODR |= (1 << self->nrPin);
        self->puerto->BSRR = 1 << self->nrPin; // BSRR pone el pin en alto.?
    break;case SP_PIN_ENTRADA_PULLDN:
        config_modo(self,ENTRADA_PULLUP_PULLDN);
        //self->puerto->ODR &= ~(1 << self->nrPin);
        self->puerto->BRR = 1 << self->nrPin; // BRR pone el pin en bajo.? 
    break;case SP_PIN_SALIDA:
        config_modo(self,SALIDA_2MHz);
    break;case SP_PIN_SALIDA_OPEN_DRAIN:
        config_modo(self,SALIDA_2MHz_OPEN_DRAIN);
    break;default:
    // Debiera generar un error
    break;
    }
    __enable_irq();
}

bool SP_Pin_read(SP_HPin hPin){       // SP_Pin_read recibe un parametro hPin que debe ser un puntero
                                      // a una estructura de datos que representa un pin de hadware. esta
                                      // funcion debe leer el valor actual del pin y devolverlo como boleano
     if(hPin>=SP_HPIN_LIMITE)
		return false;
	Pin const * pin = pinDeHandle(hPin);
	bool estado = (pin->puerto->IDR & (1<<pin->nrPin))!=0; 
	return estado;      //si esta prendido devuelve 1 sino devuelve 0.

                                        
}

void SP_Pin_write(SP_HPin hPin, bool valor){  // SP_Pin_write recibe un parametro hPin que representa el pin de 
                                         // hadware que se desea escribir y un parametro "valor" que es un
                                         //  boleano que representa el valor que se debe escribir en el pin.
                                         // esta funcion debe escribir el valor "valor" en el pin especificado
                                         // por hPin.
     if(hPin>=SP_HPIN_LIMITE)
		return;
    Pin const * pin = pinDeHandle(hPin);
    
	if(valor){
		(pin->puerto->BSRR) = (1<< (pin->nrPin)); //pone pin en alto
	}else{
		(pin->puerto->BRR) = (1<< (pin->nrPin)); //pone pin en bajo
	}	
                                         
}