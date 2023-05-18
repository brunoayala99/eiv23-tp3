#include <soporte_placa.h>   // archivo de cabecera

#define LUZ_ON 0
#define LUZ_OFF 1
#define PULSADOR_ACTIVO 0
#define PULSADOR_NORMAL 1


int main(void){                                         // int: indica que la funcion devuelve un valor entero.
                                                        // void: indica que no recibe argumentos.

    SP_init();
// esta funcion inicializa los dispositivos de la placa para que esten listos para usar.

    SP_Pin_setModo(SP_PB9,SP_PIN_ENTRADA_PULLUP);
    SP_Pin_setModo(SP_LED,SP_PIN_SALIDA);
// Estas dos lineas configuran los pines de la placa.
// La 1era linea configura el pin PB9 como una entrada con resistencia pull-up( es decir, que cuando
// el boton este sin presionar el pin PB9 esta en un estado alto.
// La 2da linea configura el LED como una salida.

    SP_Pin_write(SP_LED,LUZ_OFF);
// Esta linea apaga el LED configurado anteriormente mostrando el pin en un estado en alto (el LED se apaga
// porque esta a una fuente de voltaje).

    for (;;){    // Bucle infinito.
        while(SP_Pin_read(SP_PB9) != PULSADOR_ACTIVO);
// Esta linea se ejecuta mientras PB9 no esta activo.
// SP_Pin_read(): Lee el estado del pin y devuelve un valor que se compara con PULSADOR_ACTIVO(que es = 0).

        SP_Pin_write(SP_LED,LUZ_ON);
        SP_delay(60000);
        SP_Pin_write(SP_LED,LUZ_OFF);
// Estas lineas encienden el LED.
// SP_Pin_write(SP_LED,LUZ_ON):El pin en un estado bajo me enciende el LED ya que esta conectado a una fuente
// de voltaje.
// SP_delay(60000): Me introduce un retardo de 60 segundos.
// SP_Pin_write(SP_LED,LUZ_OFF): Apaga el LED con el pin en un estado alto. 
    }
    return 0;
// Finaliza la funcion principal. La funcion me devuelve un entero = 0.
}