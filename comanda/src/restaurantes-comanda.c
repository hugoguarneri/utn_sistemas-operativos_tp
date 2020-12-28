#include "restaurantes-comanda.h"

void restaurante_dictionary_init(){
	restaurantes_dictionary = malloc(sizeof(t_restaurante_dictionary));
	pthread_mutex_init(&restaurantes_dictionary->mutex, NULL);
	restaurantes_dictionary->restaurantes = dictionary_create();
}

void restaurante_dictionary_destroy(){
	pthread_mutex_destroy(&restaurantes_dictionary->mutex);
	// TODO Liberar memoria en forma anidada!!!
	dictionary_destroy_and_destroy_elements(restaurantes_dictionary->restaurantes, free);
	free(restaurantes_dictionary);
}

bool restaurante_dictionary_has_key(char *nombre) {
	bool value = false;
	pthread_mutex_lock(&restaurantes_dictionary->mutex);
	value = dictionary_has_key(restaurantes_dictionary->restaurantes, nombre);
	pthread_mutex_unlock(&restaurantes_dictionary->mutex);
	return value;
}

void restaurante_dictionary_put(char *nombre, void *data) {
	pthread_mutex_lock(&restaurantes_dictionary->mutex);
	dictionary_put(restaurantes_dictionary->restaurantes, nombre, data);
	pthread_mutex_unlock(&restaurantes_dictionary->mutex);
}

void* restaurante_dictionary_get(char *nombre) {
	void *data = NULL;
	pthread_mutex_lock(&restaurantes_dictionary->mutex);
	data = dictionary_get(restaurantes_dictionary->restaurantes, nombre);
	pthread_mutex_unlock(&restaurantes_dictionary->mutex);
	return data;
}
