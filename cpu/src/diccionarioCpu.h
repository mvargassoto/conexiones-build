#ifndef SRC_DICCIONARIOCPU_H_
#define SRC_DICCIONARIOCPU_H_


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>

char* puerto_escucha; 
int socket_servidor;
int socket_kernel;
int socket_memoria;
char* ip;
char* puerto_mem;
t_log* logger_cpu;
t_config* config_cpu;
pthread_t* hilo_kernel;
pthread_t* hilo_memoria;

sem_t sema_kernel;
sem_t sema_memoria;
sem_t sema_ejecucion;

#endif