#include "monitor-info-clientes.h"

/*
 * t_dictionary:
 * <String id_cliente, t_info_cliente info>
 */
t_dictionary* info_clientes;
pthread_mutex_t mutex;


void monitor_info_clientes_init(){
	info_clientes = dictionary_create();
	pthread_mutex_init(&mutex, NULL);
}

void monitor_info_clientes_destroy(){
	pthread_mutex_destroy(&mutex);
		dictionary_destroy_and_destroy_elements(info_clientes, (void*)info_cliente_destroy);
}

bool monitor_info_clientes_has_key(char* id_cliente){
	bool result = false;
	pthread_mutex_lock(&mutex);
	result = dictionary_has_key(info_clientes, id_cliente);
	pthread_mutex_unlock(&mutex);

	return result;
}

void monitor_info_clientes_put(char* id_cliente, t_info_cliente* info_cliente){
	pthread_mutex_lock(&mutex);
	dictionary_put(info_clientes, id_cliente, info_cliente);
	pthread_mutex_unlock(&mutex);
}

t_info_cliente* monitor_info_clientes_get(char* id_cliente){
	t_info_cliente* info_cliente = NULL;
	pthread_mutex_lock(&mutex);
	info_cliente = dictionary_get(info_clientes, id_cliente);
	pthread_mutex_unlock(&mutex);

	return info_cliente;
}

t_list* monitor_info_clientes_get_all(){
	t_list* info_clientes_list = list_create();
	void add_cliente_to_list(char* key, void* value){
		t_info_cliente* info_cliente = (t_info_cliente*) value;
		list_add(info_clientes_list, info_cliente);
	}

	pthread_mutex_lock(&mutex);
	dictionary_iterator(info_clientes, add_cliente_to_list);
	pthread_mutex_unlock(&mutex);

	return info_clientes_list;
}

void monitor_info_clientes_remove(char* id_cliente){
	pthread_mutex_lock(&mutex);
	dictionary_remove_and_destroy(info_clientes, id_cliente, (void*)info_cliente_destroy);
	pthread_mutex_unlock(&mutex);
}
