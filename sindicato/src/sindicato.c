#include "sindicato.h"

t_log* iniciar_logger(void);
t_config* leer_config(void);
void terminar_programa();
void registrar_signals();
void manejar_signal(int signal);

t_log* iniciar_logger(void) {
	return log_create(LOG_FILENAME, LOG_NAME, true, LOG_LEVEL_DEBUG);
}

t_config* leer_config(void) {
	return config_create(CONFIG_FILENAME);
}

void terminar_programa() {
	log_info(logger, "Finalizando sindicato");
	config_destroy(config);
	log_destroy(logger);
}

void manejar_signal(int signal) {
	log_info(logger, "Signal capturada %d ", signal);
	switch (signal) {
	case SIGKILL: {
		terminar_programa();
		abort();
		break;
	}
	case SIGTERM: {
		terminar_programa();
		exit(0);
		break;
	}
	default:
		break;
	}
}

void registrar_signals() {
	log_debug(logger, "Registrando señales ");
	signal(SIGTERM, manejar_signal);
	signal(SIGKILL, manejar_signal);
}

int main(void) {

	pthread_t thread_consola, thread_servidor;

	logger = iniciar_logger();

	log_info(logger, "Log iniciado");

	config = leer_config();

	log_info(logger, "Configurado");

	registrar_signals();

	char * basedir = config_get_string_value(config, CONFIG_PUNTO_MONTAJE);

	// chequeo los permisos de los directorios
	afip_check_directories_permissions(basedir);

	// Inicializo filesystem
	afip_initialize(basedir);

	// creo mutex de filesystem
	afip_create_caches();

	// creo thread servidor
	pthread_create(&thread_servidor, NULL, exec_servidor, NULL);

	// creo thread consola
	pthread_create(&thread_consola, NULL, exec_consola, NULL);

	// espero thread consola
	pthread_join(thread_consola, NULL);


	printf("Saliendo");
	return EXIT_SUCCESS;
}


