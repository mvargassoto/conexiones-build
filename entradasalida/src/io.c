#include "io.h"

int main(void) {

	levantar_logger();
	levantar_config();

	conectar_kernel();
	conectar_memoria();

	consola();

    terminar_conexiones(2, socket_memoria, socket_kernel);
    terminar_programa();

    return 0;
}

void consola(){
	int c;

	while (1) {
		printf("Ingrese comando:\n");
		printf("\t1 -- Saludar a Kernel \n");
		printf("\t2 -- Saludar a Memoria \n");
		printf("\t3 -- Saludar a Memoria mediante Kernel \n");
		printf("\t9 -- Apagar IO\n");
		scanf("%d", &c);
		switch (c) {
			case 1:
				enviar_codigo(socket_kernel, SALUDO_IO_KERNEL);
				break;
			case 2:
				enviar_codigo(socket_memoria, SALUDO_IO_MEMORIA);
				break;
			case 3:
				enviar_codigo(socket_kernel, SALUDO_IO_MEMORIA_MEDIANTE_KERNEL);
				break;
			case 9:
				// esto es porque por algun motivo cuando se desconecta queda enviando codigo 0 en vez de -1
				enviar_codigo(socket_kernel, -1);
				enviar_codigo(socket_memoria, -1);
				return;
			default:
				printf("\tcodigo no reconocido!\n");
				break;
		}
	}
}

void conectar_memoria(){
	ip = config_get_string_value(config_io, "IP");
	puerto_mem = config_get_string_value(config_io, "PUERTO_MEM");

    socket_memoria = crear_conexion(ip, puerto_mem);
    if (socket_memoria == -1) {
		terminar_programa();
        exit(EXIT_FAILURE);
    }
}

void conectar_kernel(){
	ip = config_get_string_value(config_io, "IP");
	puerto_kernel = config_get_string_value(config_io, "PUERTO_KERNEL");

    socket_kernel = crear_conexion(ip, puerto_kernel);
    if (socket_kernel == -1) {
		terminar_programa();
        exit(EXIT_FAILURE);
    }
}

void levantar_logger(){
	logger_io = log_create("io_log.log", "IO",true, LOG_LEVEL_INFO);
	if (!logger_io) {
		perror("Error al iniciar logger de IO\n");
		exit(EXIT_FAILURE);
	}
}

void levantar_config(){
    config_io = config_create("/home/marcos/cursadaSO/miRepoTp/conexiones/entradasalida/io.config");
	if (!config_io) {
		perror("Error al iniciar config de IO\n");
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
    if(logger_io) log_destroy(logger_io);
    if(config_io) config_destroy(config_io);
}