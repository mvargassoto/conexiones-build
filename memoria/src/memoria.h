#ifndef SRC_MEMORIA_H_
#define SRC_MEMORIA_H_

#include <utils/conexiones.h>
#include <commons/string.h>
#include <diccionarioMem.h>

void levantar_logger();
void levantar_config();
void terminar_programa();
void terminar_conexiones(int, ...);
void conectar_cpu();
void conectar_io();
void conectar_kernel();
void atender_io();
void atender_cpu();
void atender_kernel();

#endif