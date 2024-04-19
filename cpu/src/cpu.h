#ifndef SRC_CPU_H_
#define SRC_CPU_H_

#include <utils/conexiones.h>
#include <commons/string.h>
#include <diccionarioCpu.h>

void levantar_logger();
void levantar_config();
void terminar_programa();
void terminar_conexiones(int, ...);
void paquete(int);
void conectar_kernel();
void atender_kernel();
void conectar_memoria();
void comunicar_a_memoria();

#endif