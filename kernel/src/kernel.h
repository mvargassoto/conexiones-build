#ifndef SRC_KERNEL_H_
#define SRC_KERNEL_H_

#include <utils/conexiones.h>
#include <commons/string.h>
#include <diccionarioKernel.h>

void levantar_logger();
void levantar_config();
void terminar_programa();
void terminar_conexiones(int, ...);
void paquete(int);
void conectar_cpu();
void conectar_io();
void conectar_memoria();
void atender_io();
void comunicar_a_cpu();
void comunicar_a_memoria();

void consola();


#endif