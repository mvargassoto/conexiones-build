#include "kernel.h"

void iterator(char* value) {
	log_info(logger_kernel,"%s", value);
}

int main(void) {

	sem_init(&sema_consola, 0, 0);
	sem_init(&sema_io, 0, 0);

	levantar_logger();
	levantar_config();

	puerto_escucha = config_get_string_value(config_kernel, "PUERTO_ESCUCHA");

	socket_servidor = iniciar_servidor(puerto_escucha, logger_kernel);
	if (socket_servidor == -1) {
		perror("Fallo la creacion del servidor para memoria.\n");
		exit(EXIT_FAILURE);
	}

	conectar_cpu();
	conectar_memoria();
	conectar_io();

	consola();

	sem_wait(&sema_io);
	sem_wait(&sema_consola);

	// considerar que las interfaces van a ser varias, por lo que Kernel va a tener que estar
	// listo para eso

    terminar_conexiones(3, socket_io, socket_memoria, socket_cpu);
    terminar_programa();

    return 0;
}

void consola(){
	int c;
	while (1) {
		printf("Ingrese comando:\n");
		printf("\t1 -- Saludar a CPU \n");
		printf("\t2 -- Hacer que CPU salude a Memoria \n");
		printf("\t3 -- Saludar a Memoria a traves de CPU \n");
		printf("\t4 -- Apagar CPU\n");
		printf("\t9 -- Apagar Kernel\n");
		scanf("%d", &c);
		switch (c) {
			case 1:
				enviar_codigo(socket_cpu, SALUDO_KERNEL_CPU);
				break;
			case 2:
				enviar_codigo(socket_cpu, SALUDO_CPU_MEMORIA_POR_KERNEL);
				break;
			case 3:
				enviar_codigo(socket_cpu, SALUDO_KERNEL_MEMORIA_MEDIANTE_CPU);
				break;
			case 4: 
				enviar_codigo(socket_cpu, APAGAR_CPU);
				break;
			case 9:
				enviar_codigo(socket_cpu, APAGAR_CPU);
				enviar_codigo(socket_memoria, -1);
				sem_post(&sema_io);
				sem_post(&sema_consola);
				return;
			default:
				printf("\tcodigo no reconocido!\n");
				break;
		}
	}
}

void conectar_consola(){
	int err = pthread_create(&hilo_consola, NULL, consola, NULL);
	if (err != 0) {
		perror("Fallo la creacion de hilo para IO\n");
		return;
	}
	pthread_detach(hilo_consola);
}

void conectar_io(){
    log_info(logger_kernel, "Esperando IO....");
    socket_io = esperar_cliente(socket_servidor, logger_kernel);
    log_info(logger_kernel, "Se conecto IO");

	int err = pthread_create(&hilo_io, NULL, atender_io, NULL);
	if (err != 0) {
		perror("Fallo la creacion de hilo para IO\n");
		return;
	}
	pthread_detach(hilo_io);
}

void atender_io(){
	while (1) {
		mensajeIOKernel cod_io = recibir_codigo(socket_io);
		switch (cod_io) {
			case SALUDO_IO_KERNEL:
				log_info(logger_kernel, "IO saluda al Kernel");
				break;
			case SALUDO_KERNEL_CPU_DESDE_IO:
				enviar_codigo(socket_cpu, SALUDO_KERNEL_CPU_POR_IO);
				break;
			case SALUDO_IO_MEMORIA_MEDIANTE_KERNEL:
				enviar_codigo(socket_memoria, SALUDO_KERNEL_MEMORIA_POR_IO);
				break;
			case -1:
				log_error(logger_kernel, "Se desconecto IO");
				sem_post(&sema_io);
				return;
			default:
				break;
		}
	}
}

void conectar_memoria(){
	ip = config_get_string_value(config_kernel, "IP");
	puerto_mem = config_get_string_value(config_kernel, "PUERTO_MEM");

    socket_memoria = crear_conexion(ip, puerto_mem);
    if (socket_memoria == -1) {
		terminar_programa();
        exit(EXIT_FAILURE);
    }
}


void conectar_cpu(){
	ip = config_get_string_value(config_kernel, "IP");
	puerto_cpu = config_get_string_value(config_kernel, "PUERTO_CPU");

    socket_cpu = crear_conexion(ip, puerto_cpu);
    if (socket_cpu == -1) {
		terminar_programa();
        exit(EXIT_FAILURE);
    }
}

void levantar_logger(){
	logger_kernel = log_create("kernel_log.log", "KERNEL",true, LOG_LEVEL_INFO);
	if (!logger_kernel) {
		perror("Error al iniciar logger de kernel\n");
		exit(EXIT_FAILURE);
	}
}

void levantar_config(){
    config_kernel = config_create("/home/marcos/cursadaSO/miRepoTp/conexiones/kernel/kernel.config");
	if (!config_kernel) {
		perror("Error al iniciar config de kernel\n");
		exit(EXIT_FAILURE);
	}
}

void paquete(int conexion){
	char* leido;
	t_paquete* paquete = crear_paquete();

	while(1) {
		printf("agregar lineas al paquete...\n");
		leido = readline("> ");
		if (string_is_empty(leido)) {
			free(leido);
			break;
		}

		agregar_a_paquete(paquete, leido, strlen(leido)+1);
		free(leido);
	}

	enviar_paquete(paquete, conexion);

	eliminar_paquete(paquete);
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
    if(logger_kernel) log_destroy(logger_kernel);
    if(config_kernel) config_destroy(config_kernel);
}