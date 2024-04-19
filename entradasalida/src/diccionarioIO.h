#ifndef SRC_DICCIONARIOIO_H_
#define SRC_DICCIONARIOIO_H_


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

char* ip;
char* puerto_mem;
char* puerto_kernel;
int socket_memoria;
int socket_kernel;
t_log* logger_io;
t_config* config_io;
pthread_t* hilo_kernel;
pthread_t* hilo_memoria;

sem_t sema_memoria;
sem_t sema_kernel;

#endif