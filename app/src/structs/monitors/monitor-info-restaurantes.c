#include "monitor-info-restaurantes.h"


/*
 * t_dictionary:
 * <char* restaurante, t_info_restaurante info>
 */
t_dictionary* info_restaurantes;
pthread_mutex_t mutex;


void monitor_info_restaurantes_init(){
	info_restaurantes = dictionary_create();
	pthread_mutex_init(&mutex, NULL);
}

void monitor_info_restaurantes_destroy(){
	pthread_mutex_destroy(&mutex);
	dictionary_destroy_and_destroy_elements(info_restaurantes, (void*)info_restaurante_destroy);
}

bool monitor_info_restaurantes_has_key(char* restaurante){
	bool result = false;
	pthread_mutex_lock(&mutex);
	result = dictionary_has_key(info_restaurantes, restaurante);
	pthread_mutex_unlock(&mutex);

	return result;
}

void monitor_info_restaurantes_put(char* restaurante, t_info_restaurante* info_restaurante){
	pthread_mutex_lock(&mutex);
	dictionary_put(info_restaurantes, restaurante, info_restaurante);
	pthread_mutex_unlock(&mutex);
}

t_info_restaurante* monitor_info_restaurantes_get(char* restaurante){
	t_info_restaurante* info_restaurante = NULL;
	pthread_mutex_lock(&mutex);
	info_restaurante = dictionary_get(info_restaurantes, restaurante);
	pthread_mutex_unlock(&mutex);

	return info_restaurante;
}

t_list* monitor_info_restaurantes_get_all(){
	t_list* info_restaurantes_list = list_create();
	void add_restaurante_to_list(char* key, void* value){
		t_info_restaurante* info_restaurante = (t_info_restaurante*) value;
		list_add(info_restaurantes_list, info_restaurante);
	}

	pthread_mutex_lock(&mutex);
	dictionary_iterator(info_restaurantes, add_restaurante_to_list);
	pthread_mutex_unlock(&mutex);

	return info_restaurantes_list;
}

void monitor_info_restaurantes_remove(char* restaurante){
	pthread_mutex_lock(&mutex);
	dictionary_remove_and_destroy(info_restaurantes, restaurante, (void*)info_restaurante_destroy);
	pthread_mutex_unlock(&mutex);
}

