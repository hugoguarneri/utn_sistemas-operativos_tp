#include "comanda.h"
#include "core.h"
#include "servidor-comanda.h"
#include "restaurantes-comanda.h"

void configuracion_init();
void configuracion_destroy();
void terminar_programa();
void registrar_signals_comanda();
void manejar_signal_comanda(int signal);

t_log* iniciar_logger(void) {
	return log_create(ARCHIVO_LOG, LOG_NAME, true, LOG_LEVEL_DEBUG);
}

t_config* leer_config(void) {
	return config_create(CONFIG_FILENAME);
}

void terminar_programa() {
	configuracion_destroy();
	log_info(logger, "Finalizando comanda");
	config_destroy(config);
	free(memoria_ppal);
	munmap(memoria_virt,configuracion->tamanio_swap_bytes);
	log_destroy(logger);
}

void registrar_signals_comanda() {
	log_debug(logger, "Registrando señales ");
	signal(SIGTERM, manejar_signal_comanda);
	signal(SIGKILL, manejar_signal_comanda);
}

void manejar_signal_comanda(int signal) {
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

int main(void) {

	pthread_t thread_servidor;

	logger = iniciar_logger();

	log_info(logger, "Log iniciado");

	config = leer_config();

	log_info(logger, "Configurado");

	configuracion_init();
	core_allocate_fullspace();

	//Inicializo el dictionary de restaurantes
	restaurante_dictionary_init();

	//Registro señales para liberar espacio cuando se cierre
	registrar_signals_comanda();

	// creo thread servidor
	pthread_create(&thread_servidor, NULL, (void*) exec_servidor_comanda, NULL);

	pthread_join(thread_servidor, NULL);

	terminar_programa();

	return EXIT_SUCCESS;
}

void configuracion_init() {
	configuracion = calloc(sizeof(t_configuracion), 1);
	configuracion->puerto = config_get_string_value(config, PUERTO_ESCUCHA);
	configuracion->ip_escucha = config_get_string_value(config, IP_ESCUCHA);
	configuracion->tamanio_memoria_bytes = config_get_int_value(config, TAMANIO_MEMORIA);
	configuracion->tamanio_swap_bytes = config_get_int_value(config, TAMANIO_SWAP);
	char* algoritmo = config_get_string_value(config, ALGORITMO_REEMPLAZO);

	if ((configuracion->tamanio_memoria_bytes % FRAME_SIZE) != 0) {
		log_error(logger, "El tamaño de memoria %d no es multiplo del tamaño de frame",
				configuracion->tamanio_memoria_bytes);
		exit_failure();
	}
	if ((configuracion->tamanio_swap_bytes % FRAME_SIZE) != 0) {
		log_error(logger, "El tamaño de swap %d no es multiplo del tamaño de frame",
				configuracion->tamanio_swap_bytes);
		exit_failure();
	}

	configuracion->memoria_max_frames = configuracion->tamanio_memoria_bytes / FRAME_SIZE;
	configuracion->swap_max_frames = configuracion->tamanio_swap_bytes / FRAME_SIZE;

	if(strcmp(algoritmo, "LRU") == 0){
		configuracion->algoritmo = LRU;
	} else if(strcmp(algoritmo, "CLOCK_MEJ") == 0){
		configuracion->algoritmo = CMJ;
	} else {
		log_error(logger, "Algoritmo de reemplazo no identificado");
		exit_failure();
	}
}

void configuracion_destroy() {
	free(configuracion);
}
