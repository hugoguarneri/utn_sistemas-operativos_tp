#include "restaurante.h"

int main(int argc, char *argv[]) {

	config = config_create(argv[1]);
	char* restaurante_name = strdup(config_get_string_value(config, "NOMBRE_RESTAURANTE"));

	logger = log_create(argv[2], restaurante_name, true, LOG_LEVEL_DEBUG);

	log_info(logger, "INICIADO RESTAURANTE: %s", restaurante_name);

	sem_init(&sem_habilitar_servidor, 0, 0);
	id_pcb = 0;
	id_pedido = 0;

	info_restaurante = obtener_restaurante(restaurante_name);
	handshake_app(info_restaurante, restaurante_name);	

	if(pthread_create(&thread_planificador, NULL, (void*) exec_planificador_restaurante, info_restaurante) != 0){
		log_error(logger, "Ocurrio un error al intentar crear thread_planificador -> %s", strerror(errno));
		return EXIT_FAILURE;
	}

	if(pthread_create(&thread_servidor, NULL, (void*) exec_servidor_restaurante, NULL) != 0){
		log_error(logger, "Ocurrio un error al intentar crear thread_servidor -> %s", strerror(errno));
		return EXIT_FAILURE;
	}

	pthread_join(thread_servidor, NULL);
	pthread_join(thread_planificador, NULL);

	/*config_destroy(config);
	log_destroy(logger);
	free(restaurante_name);
	obtener_restaurante_res_destroy(res);*/

	return EXIT_SUCCESS;
}
