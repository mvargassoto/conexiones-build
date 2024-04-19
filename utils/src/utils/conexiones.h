#ifndef SRC_CONEXIONES_H_
#define SRC_CONEXIONES_H_

// SERVER
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<readline/readline.h>
#include<signal.h>


typedef enum {
	SALUDO_IO_KERNEL,
	SALUDO_KERNEL_CPU_DESDE_IO,
	SALUDO_IO_MEMORIA_MEDIANTE_KERNEL
}mensajeIOKernel;

typedef enum {
	SALUDO_IO_MEMORIA
}mensajeIOMemoria;

typedef enum {
	SALUDO_KERNEL_CPU_POR_IO,
	SALUDO_KERNEL_CPU,
	SALUDO_KERNEL_MEMORIA_MEDIANTE_CPU,
	SALUDO_CPU_MEMORIA_POR_KERNEL,
	APAGAR_CPU
}mensajeKernelCpu;

typedef enum {
	SALUDO_KERNEL_MEMORIA,
	SALUDO_KERNEL_MEMORIA_POR_IO
}mensajeKernelMemoria;

typedef enum {
	SALUDO_CPU_MEMORIA,
	SALUDO_KERNEL_MEMORIA_POR_CPU
}mensajeCpuMemoria;


// SERVER
typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

//CLIENT
typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

//SERVER
void* recibir_buffer(int*, int);

int iniciar_servidor(char*, t_log*);
int esperar_cliente(int, t_log*);
t_list* recibir_paquete(int);
void recibir_mensaje(int, t_log*);
int recibir_operacion(int);

//CLIENT
int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

void enviar_codigo(int socket_receptor, int codigo);
int recibir_codigo(int socket_emisor);

#endif