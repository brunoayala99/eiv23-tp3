#include "controlador_luz.h"
#include <maquina_estado_impl.h>

static Resultado estadoApagado(Maquina *contexto,Evento evento);
static Resultado estadoEncendido(Maquina *contexto,Evento evento);
static Resultado estadoMudanza (Maquina *contexto, Evento evento);

void ControladorLuz_init(ControladorLuz *self,uint32_t tiempoOn,SP_HPin pinLuz,bool nivelLuzOn,DespachoRetardado *despachoRetardado){

    Maquina_init(&self->maquina,estadoApagado);
    self->tiempoOn = tiempoOn;
    self->interfazLuz.pin=pinLuz;
    self->interfazLuz.nivelOn=nivelLuzOn;
    self->despachoRetardado = despachoRetardado;
}

Maquina * ControladorLuz_asMaquina(ControladorLuz *self){
    return &self->maquina;
}

static void ControladorLuz__apagaLuz(ControladorLuz *self){
    SP_Pin_write(self->interfazLuz.pin,!self->interfazLuz.nivelOn);
}