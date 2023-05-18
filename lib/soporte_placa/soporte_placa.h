#ifndef SOPORTE_PLACA_H
#define SOPORTE_PLACA_H

#include <stdbool.h>   
// Biblioteca estandar de C.
// stdbool.h define un tipo de dato 'bool' y los valores booleanos 'True' y 'False'.

#include <stdint.h>    
// Biblioteca estandar de C.
// stdint.h define un conjunto de tipos de datos de longitud definidos (por ej. 'uint32_t' que es un entero
// sin signo de 32 bits).

// Declaraciones
// Espacio de nombres: SP_

/**
 * @brief Handles (manejadores) correspondientes a los pines de entrada/salida, para 
 * usar en el parámetro hPin (primer parámetro) de las funciones SP_Pin_xxx
 * 
 */
enum SP_Pines{
    SP_PB9,
    SP_PC13,
    SP_LED = SP_PC13, // LED verde integrado en la placa, pin PC13 
    SP_HPIN_LIMITE
};
// SP_PB9 , SP_PC13 y SP_LED son ctes enteras.

/**
 * @brief Handle que representa un objeto Pin.
 * Toma valores de las constantes SP_Pines
 */
typedef int SP_HPin; // Es una definicion de tipo. Aqui se crea un alias 'SP_HPin' para el tipo de datos 'int'.
                     // 'SP_HPin' se utiliza como tipo de datos para 'hPin'.

/**
 * @brief Definición de modo de operación de un Pin
 * 
 */
typedef enum SP_Pin_Modo{  // 'typedef enum SP_Pin_Modo' es una definicion de tipo que crea un nuevo tipo
                           // de datos 'SP_Pin_Modo' la cual es una enumeracion de los diferentes modos de 
                           // operacion  que se pueden configurar para un pin.


    SP_PIN_ENTRADA,          // Entrada flotante
    SP_PIN_ENTRADA_PULLUP,   // Entrada con resistencia pull-up interna
    SP_PIN_ENTRADA_PULLDN,   // Entrada con resistencia pull-down interna
    SP_PIN_SALIDA,           // Salida push-pull
    SP_PIN_SALIDA_OPEN_DRAIN // Salida con drenador abierto

    // SP_PIN_ENTRADA, SP_PIN_ENTRADA_PULLUP, SP_PIN_ENTRADA_PULLDN, SP_PIN_SALIDA y SP_PIN_SALIDA_OPEN_DRAIN
    // son los modos de operacion y son ctes.
}SP_Pin_Modo;

/**
 * @brief Configura el modo de un pin
 * 
 * @param hPin Handle al objeto Pin
 * @param modo Modo a configurar
 */
void SP_Pin_setModo(SP_HPin hPin,SP_Pin_Modo modo); // declaro la funcion 'SP_Pin_setModo' que configura el
                                                    // modo de un pin.Esta funcion toma un Handle al objeto
                                                    // Pin 'hPin' y un modo de operacion 'modo', el cual
                                                    // fue definido en 'SP_Pin_Modo'

/**
 * @brief Lee el buffer de entrada de un Pin
 * 
 * @param hPin Handle al objeto Pin
 * @return true Entrada ALTA
 * @return false Entrada BAJA
 */
bool SP_Pin_read(SP_HPin hPin);

/**
 * @brief Escribe el buffer de salida de un Pin
 * 
 * @param hPin Handle al objeto Pin
 * @param valor True: Salida ALTA. False: Salida BAJA.
 */
void SP_Pin_write(SP_HPin hPin, bool valor);    // declaro la funcion 'SP_Pin_write' que escribe un valor
                                                // en un pin de salida. esta funcion toma un handle al
                                                // objeto Pin 'hPin' y tambien toma un valor booleano 'valor'
                                                // que es 'True' si quiero el pin en estado alto y es 'false'
                                                // si quiero el pin en estado bajo.

/**
 * @brief Inicializa la librería. Es necesario llamar a
 * esta función antes de usar la misma.
 * 
 */
void SP_init(void); //declaro la funcion SP_init que inicializa la biblioteca.

/**
 * @brief Retardo con bloqueo durante un tiempo dado en milisegundos
 * @note Llamar a SP_init antes de usar.
 * @param tiempo Tiempo en milisegundos
 */
void SP_delay(uint32_t tiempo);

#endif