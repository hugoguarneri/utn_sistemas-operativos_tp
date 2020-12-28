#include "app.h"

void finalizar_proceso(){
	log_error(logger, "[MAIN] - Ocurrio un error inesperado, se va a finalizar el froceso app.");
	exit(-1);
}

int main() {

	logger = log_create(LOG_FILENAME, LOG_NAME, true, LOG_LEVEL_DEBUG);
	config = config_create(CONFIG_FILENAME);

	sem_init(&sem_habilitar_resto_default, 0, 0);
	sem_init(&sem_habilitar_servidor, 0, 0);

	sem_init(&sem_repartidores_libres, 0, 0);
	sem_init(&sem_grado_multiprocesamiento, 0, config_get_int_value(config, "GRADO_DE_MULTIPROCESAMIENTO"));

	sem_init(&sem_cola_new, 0, 0);
	sem_init(&sem_cola_ready, 0, 0);
	sem_init(&sem_cola_exec, 0, 0);
	sem_init(&sem_cola_block, 0, 0);
	sem_init(&sem_cola_exit, 0, 0);

	pthread_mutex_init(&mutex_cola_block, NULL);
	pthread_mutex_init(&mutex_cola_ready, NULL);

	monitor_info_restaurantes_init();
	monitor_info_clientes_init();
	monitor_clientes_restaurantes_init();
	monitor_repartidores_init();

	id_pcb = 1;

	if(exec_hanshake_comanda() != 0){
		log_error(logger, "[MAIN] - Ocurrio un error al intentar realizar el handshake contra la comanda");
		return EXIT_FAILURE;
	}

	if(pthread_create(&thread_servidor, NULL, (void*) exec_servidor_app, NULL) != 0){
		log_error(logger, "[MAIN] - Ocurrio un error al intentar crear thread_servidor -> %s", strerror(errno));
		return EXIT_FAILURE;
	}

	if(pthread_create(&thread_planificador, NULL, (void*) exec_planificador_app, NULL) != 0){
		log_error(logger, "[MAIN] - Ocurrio un error al intentar crear thread_planificador -> %s", strerror(errno));
		return EXIT_FAILURE;
	}

	if(pthread_create(&thread_restaurante, NULL, (void*) exec_restaurante_default, NULL) != 0){
		log_error(logger, "[MAIN] - Ocurrio un error al intentar crear thread_restaurante -> %s", strerror(errno));
		return EXIT_FAILURE;
	}

	pthread_join(thread_servidor, NULL);
	pthread_join(thread_planificador, NULL);
	pthread_join(thread_restaurante, NULL);

	config_destroy(config);
	log_destroy(logger);

	return EXIT_SUCCESS;
}

