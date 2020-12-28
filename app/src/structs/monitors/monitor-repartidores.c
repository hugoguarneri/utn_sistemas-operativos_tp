#include "monitor-repartidores.h"


/*
 * t_dictionary:
 * <char* id_repartidor, t_repartidor repartidor>
 */
t_dictionary* repartidores;
pthread_mutex_t mutex;


void monitor_repartidores_init(){
	repartidores = dictionary_create();
	pthread_mutex_init(&mutex, NULL);
}

void monitor_repartidores_destroy(){
	pthread_mutex_destroy(&mutex);
	dictionary_destroy_and_destroy_elements(repartidores, (void*)repartidor_destroy);
}

bool monitor_repartidores_has_key(char* id_repartidor){
	bool result = false;
	pthread_mutex_lock(&mutex);
	result = dictionary_has_key(repartidores, id_repartidor);
	pthread_mutex_unlock(&mutex);

	return result;
}

void monitor_repartidores_put(char* id_repartidor, t_repartidor* repartidor){
	pthread_mutex_lock(&mutex);
	dictionary_put(repartidores, id_repartidor, repartidor);
	pthread_mutex_unlock(&mutex);
}

t_repartidor* monitor_repartidores_get(char* id_repartidor){
	t_repartidor* repartidor = NULL;
	pthread_mutex_lock(&mutex);
	repartidor = dictionary_get(repartidores, id_repartidor);
	pthread_mutex_unlock(&mutex);

	return repartidor;
}

t_list* monitor_repartidores_get_all(){
	t_list* repartidores_list = list_create();
	void add_repartidor_to_list(char* key, void* value){
		t_repartidor* repartidor = (t_repartidor*) value;
		list_add(repartidores_list, repartidor);
	}

	pthread_mutex_lock(&mutex);
	dictionary_iterator(repartidores, add_repartidor_to_list);
	pthread_mutex_unlock(&mutex);

	return repartidores_list;
}

void monitor_repartidores_remove(char* id_cliente){
	pthread_mutex_lock(&mutex);
	dictionary_remove_and_destroy(repartidores, id_cliente, (void*)repartidor_destroy);
	pthread_mutex_unlock(&mutex);
}

