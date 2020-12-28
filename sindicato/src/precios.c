#include "precios.h"

void precios_init() {
	precios = calloc(sizeof(t_precios), 1);
	precios->dictionary = dictionary_create();
	precios->rwlock = calloc(sizeof(pthread_rwlock_t), 1);
	pthread_rwlock_init(precios->rwlock, PTHREAD_RWLOCK_DEFAULT_NP);
	log_debug(logger, "Precios iniciados");
}

void precios_destroy() {
	dictionary_destroy_and_destroy_elements(precios->dictionary, free);
	pthread_rwlock_destroy(precios->rwlock);
	free(precios->rwlock);
	free(precios);
}

int precios_get_precio(char *restaurante, char *receta) {
	char *key = string_from_format("%s_%s", restaurante, receta);
	log_debug(logger, "Buscando el precio para %s", key);
	int valor = 0;
	pthread_rwlock_rdlock(precios->rwlock);
	valor = dictionary_get_int_value(precios->dictionary, key);
	pthread_rwlock_unlock(precios->rwlock);
	log_debug(logger, "El precio para %s es %d", key, valor);
	free(key);

	return valor;
}

void precios_set_precio(char *restaurante, char *receta, int precio) {
	char *key = string_from_format("%s_%s", restaurante, receta);
	log_debug(logger, "Seteo el precio para %s en %d", key, precio);
	pthread_rwlock_wrlock(precios->rwlock);
	dictionary_put(precios->dictionary, key, string_from_format("%d", precio));
	pthread_rwlock_unlock(precios->rwlock);
	free(key);
}

