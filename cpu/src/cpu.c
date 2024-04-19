#include "cpu.h"

void iterator(char* value) {
	log_info(logger_cpu,"%s", value);
}

int main(void) {
	sem_init(&sema_kernel, 0, 0);

	levantar_logger();
	levantar_config();

	puerto_escucha = config_get_string_value(config_cpu, "PUERTO_ESCUCHA");

	socket_servidor = iniciar_servidor(puerto_escucha, logger_cpu);
	if (socket_servidor == -1) {
		perror("Fallo la creacion del servidor para memoria.\n");
		exit(EXIT_FAILURE);
	}

	conectar_memoria();
	conectar_kernel();

	sem_wait(&sema_kernel);

    terminar_conexiones(2, socket_memoria, socket_kernel);
    terminar_programa();

    return 0;
}

void levantar_logger(){
	logger_cpu = log_create("cpu_log.log", "CPU",true, LOG_LEVEL_INFO);
	if (!logger_cpu) {
		perror("Error al iniciar logger de cpu\n");
		exit(EXIT_FAILURE);
	}
}

void levantar_config(){
    config_cpu = config_create("/home/marcos/cursadaSO/miRepoTp/conexiones/cpu/cpu.config");
	if (!config_cpu) {
		perror("Error al iniciar config de cpu\n");
		exit(EXIT_FAILURE);
	}
}

void conectar_kernel(){
	log_info(logger_cpu, "Esperando Kernel....");
    socket_kernel = esperar_cliente(socket_servidor, logger_cpu);
    log_info(logger_cpu, "Se conecto Kernel");

	int err = pthread_create(&hilo_kernel, NULL, atender_kernel, NULL);
	if (err != 0) {
		perror("Fallo la creacion de hilo para Kernel\n");
		return;
	}
	pthread_detach(hilo_kernel);
}

void atender_kernel(){
	while (1) {
		mensajeKernelCpu cod_kernel = recibir_codigo(socket_kernel);
		switch (cod_kernel) {
			case SALUDO_KERNEL_CPU:
				log_info(logger_cpu, "Kernel saluda a Cpu");
				break;
			case SALUDO_KERNEL_CPU_POR_IO:
				log_info(logger_cpu, "IO hace que Kernel salude a Cpu");
				break;
			case SALUDO_KERNEL_MEMORIA_MEDIANTE_CPU:
				enviar_codigo(socket_memoria, SALUDO_KERNEL_MEMORIA_POR_CPU);
				break;
			case SALUDO_CPU_MEMORIA_POR_KERNEL:
				log_info(logger_cpu, "Kernel me dice que salude a Memoria");
				enviar_codigo(socket_memoria, SALUDO_CPU_MEMORIA);
				break;
			case APAGAR_CPU:
				log_info(logger_cpu, "Kernel me pide que me apague");
				enviar_codigo(socket_memoria, -1);
				sem_post(&sema_kernel);
				return;
			case -1:
				log_error(logger_cpu, "Se desconecto Kernel");
				sem_post(&sema_kernel);
				return;
			default:
				break;
		}
	}
}

void conectar_memoria(){
	ip = config_get_string_value(config_cpu, "IP");
	puerto_mem = config_get_string_value(config_cpu, "PUERTO_MEM");

    socket_memoria = crear_conexion(ip, puerto_mem);
    if (socket_memoria == -1) {
		terminar_programa();
        exit(EXIT_FAILURE);
    }
/*
	int err = pthread_create(&hilo_memoria, NULL, atender_kernel, NULL);
	if (err != 0) {
		perror("Fallo la creacion de hilo para Memoria\n");
		return;
	}

    enviar_mensaje("CPU CONECTADO!", socket_memoria);
*/
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
	
	enviar_mensaje("CPU CONECTADO!", conexion);

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
    if(logger_cpu) log_destroy(logger_cpu);
    if(config_cpu) config_destroy(config_cpu);
}