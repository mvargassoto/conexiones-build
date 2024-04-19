#include "memoria.h"

void iterator(char* value) {
	log_info(logger_memoria,"%s", value);
}

int main(void) {

	// NOTA: si por ejemplo el unico semaforo iniciado es 0 2 NO va a funcionar, si hay un solo semaforo
	// para iniciar TIENE QUE ARRANCAR DE 0 0 
	// NOTA: EL QUE TENGA EL 0 0 SE TIENE QUE DESCONECTAR ULTIMO, no se por que pero pasa eso, averiguar por que
	sem_init(&sema_io, 0, 0);
	sem_init(&sema_cpu, 0, 1);
	sem_init(&sema_kernel, 0, 2);

	levantar_logger();
	levantar_config();

	puerto_escucha = config_get_string_value(config_memoria, "PUERTO_ESCUCHA");

	socket_servidor = iniciar_servidor(puerto_escucha, logger_memoria);
	if (socket_servidor == -1) {
		perror("Fallo la creacion del servidor para memoria.\n");
		exit(EXIT_FAILURE);
	}

	conectar_cpu();
	conectar_kernel();
	conectar_io();

	sem_wait(&sema_io);
	sem_wait(&sema_cpu);
	sem_wait(&sema_kernel);

	char c;
	printf("Ingrese cualquier letra para finalizar...\n");
	scanf("%c", &c);

	terminar_conexiones(3,socket_io,socket_cpu,socket_kernel);	
	terminar_programa();

	return 0;
}

void levantar_logger(){
    logger_memoria = log_create("memoria.log", "MEMORIA",true, LOG_LEVEL_INFO);
	if (!logger_memoria) {
		perror("Error al iniciar logger de memoria\n");
		exit(EXIT_FAILURE);
	}
}

void levantar_config(){
	config_memoria = config_create("/home/marcos/cursadaSO/miRepoTp/conexiones/memoria/memoria.config");
	if (!config_memoria) {
		perror("Error al iniciar config de memoria\n");
		exit(EXIT_FAILURE);
	}
}

void conectar_kernel(){
	log_info(logger_memoria, "Esperando Kernel....");
    socket_kernel = esperar_cliente(socket_servidor, logger_memoria);
    log_info(logger_memoria, "Se conecto Kernel");

	int err = pthread_create(&hilo_kernel, NULL, atender_kernel, NULL);
	if (err != 0) {
		perror("Fallo la creacion de hilo para Kernel\n");
		return;
	}
	pthread_detach(hilo_kernel);
}

void conectar_io(){
    log_info(logger_memoria, "Esperando IO....");
    socket_io = esperar_cliente(socket_servidor, logger_memoria);
    log_info(logger_memoria, "Se conecto IO");

	int err = pthread_create(&hilo_io, NULL, atender_io, NULL);
	if (err != 0) {
		perror("Fallo la creacion de hilo para IO\n");
		return;
	}
	pthread_detach(hilo_io);
}

void conectar_cpu(){
    log_info(logger_memoria, "Esperando Cpu....");
    socket_cpu = esperar_cliente(socket_servidor, logger_memoria);
    log_info(logger_memoria, "Se conecto Cpu");
    	
	int err = pthread_create(&hilo_cpu, NULL, atender_cpu, NULL);
	if (err != 0) {
		perror("Fallo la creacion de hilo para CPU\n");
		return;
	}
	pthread_detach(hilo_cpu);
}

void atender_kernel(){
	while (1) {
		mensajeKernelMemoria cod_kernel = recibir_codigo(socket_kernel);
		switch (cod_kernel) {
			case SALUDO_KERNEL_MEMORIA:
				log_info(logger_memoria, "Kernel saluda a Memoria");
				break;	
			case SALUDO_KERNEL_MEMORIA_POR_IO:
				log_info(logger_memoria, "IO saluda a Memoria a traves de Kernel");	
				break;
			case -1:
				log_error(logger_memoria, "Se desconecto Kernel");
				sem_post(&sema_kernel);
				return;
			default:
				break;
		}
	}
}

void atender_io(){
	while (1) {
		mensajeIOMemoria cod_io = recibir_codigo(socket_io);
		switch (cod_io) {
			case SALUDO_IO_MEMORIA:
				log_info(logger_memoria, "IO saluda a Memoria");
				break;
			case -1:
				log_error(logger_memoria, "Se desconecto IO");
				sem_post(&sema_io);
				return;
			default:
				break;
		}
	}
}

void atender_cpu(){
	while (1) {
		mensajeCpuMemoria cod_cpu = recibir_codigo(socket_cpu);
		switch (cod_cpu) {
			case SALUDO_CPU_MEMORIA:
				log_info(logger_memoria, "Cpu saluda a Memoria");
				break;
			case SALUDO_KERNEL_MEMORIA_POR_CPU:
				log_info(logger_memoria, "Kernel saluda a Memoria mediante Cpu");
				break;
			case -1:
				log_error(logger_memoria, "Se desconecto CPU");
				sem_post(&sema_cpu);
				return;
			default:
				break;
		}
	}
}

void terminar_conexiones(int num_sockets, ...) {
  va_list args;
  va_start(args, num_sockets);

  for (int i = 0; i < num_sockets; i++) {
    int socket_fd = va_arg(args, int);
    liberar_conexion(socket_fd);
  }

  va_end(args);
}

void terminar_programa(){
	if (logger_memoria) log_destroy(logger_memoria);
	if (config_memoria) config_destroy(config_memoria);
}
