#include "cliente.h"

void finalizar_proceso(){
	log_debug(logger, "Ocurrio un error inesperado, se va a finalizar el froceso cliente.");
	exit(-1);
}

/*
 * @PARAMS:
 * 		- config_file: ruta del arcivo de configuracion.
 * 		- log_file: ruta del archivo de log.
 */
int main(int argc, char *argv[]) {

	config = config_create(argv[1]);
	logger = log_create(argv[2], LOG_NAME, false, LOG_LEVEL_DEBUG);

	log_info(logger, "Inicializando cliente..");

	sem_init(&sem_habilitar_consola, 0, 1);
	sem_init(&sem_habilitar_notificaion, 0, 0);

	char* ip = strdup(config_get_string_value(config, "IP_ESCUCHA"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));
	char* id_cliente = strdup(config_get_string_value(config, "ID_CLIENTE"));
	int pos_x = config_get_int_value(config, "POSICION_X");
	int pos_y = config_get_int_value(config, "POSICION_Y");

	info_cliente = info_cliente_create(ip, puerto, id_cliente, pos_x, pos_y);

	log_info(logger,"Servidor puerto %s:%s",ip,puerto);

	if(pthread_create(&thread_consola, NULL, (void*) exec_consola_cliente, NULL) != 0){
		log_error(logger, "Ocurrio un error al intentar crear thread_consola -> %s", strerror(errno));
		return EXIT_FAILURE;
	}

	if(pthread_create(&thread_notificaciones, NULL, (void*) exec_notificaciones_cliente, NULL) != 0){
		log_error(logger, "Ocurrio un error al intentar crear thread_notificaciones -> %s", strerror(errno));
		return EXIT_FAILURE;
	}

	pthread_join(thread_consola, NULL);
	pthread_detach(thread_notificaciones);

	config_destroy(config);
	log_destroy(logger);

	sem_destroy(&sem_habilitar_consola);
	sem_destroy(&sem_habilitar_notificaion);

	return EXIT_SUCCESS;
}
