#include "monitor-clientes-restaurantes.h"


/*
 * t_dictionary:
 * <char* id_cliente, char* restaurante>
 */
t_dictionary* clientes_restaurante;
pthread_mutex_t mutex;


void monitor_clientes_restaurantes_init(){
	pthread_mutex_init(&mutex, NULL);
	clientes_restaurante = dictionary_create();
}

void monitor_clientes_restaurantes_destroy(){
	pthread_mutex_destroy(&mutex);
	dictionary_destroy_and_destroy_elements(clientes_restaurante, free);
}

bool monitor_clientes_restaurantes_has_key(char* id_cliente){
	bool result = false;
	pthread_mutex_lock(&mutex);
	result = dictionary_has_key(clientes_restaurante, id_cliente);
	pthread_mutex_unlock(&mutex);

	return result;
}

void monitor_clientes_restaurantes_put(char* id_cliente, char* restaurante){
	pthread_mutex_lock(&mutex);
	dictionary_put(clientes_restaurante, id_cliente, restaurante);
	pthread_mutex_unlock(&mutex);
}

char* monitor_clientes_restaurantes_get(char* id_cliente){
	char* restaurante = NULL;
	pthread_mutex_lock(&mutex);
	restaurante = dictionary_get(clientes_restaurante, id_cliente);
	pthread_mutex_unlock(&mutex);

	return restaurante;
}

t_list* monitor_clientes_restaurantes_get_all(){
	t_list* restaurantes = list_create();
	void add_restaurante_to_list(char* key, void* value){
		char* nombre_restaurante = string_duplicate((char*) value);
		list_add(restaurantes, nombre_restaurante);
	}

	pthread_mutex_lock(&mutex);
	dictionary_iterator(clientes_restaurante, add_restaurante_to_list);
	pthread_mutex_unlock(&mutex);

	return restaurantes;
}

void monitor_clientes_restaurantes_remove(char* id_cliente){
	pthread_mutex_lock(&mutex);
	dictionary_remove_and_destroy(clientes_restaurante, id_cliente, free);
	pthread_mutex_unlock(&mutex);
}
