#include "monitor-cocineros.h"

/*
 * t_dictionary:
 * <char* id_cocinero, t_cocinero cocinero>
 */
t_dictionary* cocineros;
pthread_mutex_t mutex;


void monitor_cocineros_init(){
	cocineros = dictionary_create();
	pthread_mutex_init(&mutex, NULL);
}

void monitor_cocineros_destroy(){
	pthread_mutex_destroy(&mutex);
	dictionary_destroy_and_destroy_elements(cocineros, (void*)cocinero_destroy);
}

bool monitor_cocineros_has_key(char* id_cocinero){
	bool result = false;
	pthread_mutex_lock(&mutex);
	result = dictionary_has_key(cocineros, id_cocinero);
	pthread_mutex_unlock(&mutex);

	return result;
}

void monitor_cocineros_put(char* id_cocinero, t_cocinero* cocinero){
	pthread_mutex_lock(&mutex);
	dictionary_put(cocineros, id_cocinero, cocinero);
	pthread_mutex_unlock(&mutex);
}

t_cocinero* monitor_cocineros_get(char* id_cocinero){
	t_cocinero* cocinero = NULL;
	pthread_mutex_lock(&mutex);
	cocinero = dictionary_get(cocineros, id_cocinero);
	pthread_mutex_unlock(&mutex);

	return cocinero;
}

void monitor_cocineros_update_estado(char* id_cocinero, e_estado estado){
	pthread_mutex_lock(&mutex);
	t_cocinero* cocinero = dictionary_get(cocineros, id_cocinero);
	cocinero->estado = estado;
	pthread_mutex_unlock(&mutex);
}

t_list* monitor_cocineros_get_all(){
	t_list* cocineros_list = list_create();
	void add_repartidor_to_list(char* key, void* value){
		t_cocinero* cocinero = (t_cocinero*) value;
		list_add(cocineros_list, cocinero);
	}

	pthread_mutex_lock(&mutex);
	dictionary_iterator(cocineros, add_repartidor_to_list);
	pthread_mutex_unlock(&mutex);

	return cocineros_list;
}

void monitor_cocineros_remove(char* id_cocinero){
	pthread_mutex_lock(&mutex);
	dictionary_remove_and_destroy(cocineros, id_cocinero, (void*)cocinero_destroy);
	pthread_mutex_unlock(&mutex);
}


