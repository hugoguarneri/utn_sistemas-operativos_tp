#include "monitor-pedido-default.h"


/*
 * t_monitor:
 * <char* id_pedido, t_pedido_default* pedido>
 */
t_dictionary* pedidos;
pthread_mutex_t mutex;


void monitor_pedido_default_init(){
	pedidos = dictionary_create();
	pthread_mutex_init(&mutex, NULL);
}

void monitor_pedido_default_destroy(){
	pthread_mutex_destroy(&mutex);
	dictionary_destroy_and_destroy_elements(pedidos, (void*)pedido_default_destroy);
}

bool monitor_pedido_default_has_key(char* id_pedido){
	bool result = false;
	pthread_mutex_lock(&mutex);
	result = dictionary_has_key(pedidos, id_pedido);
	pthread_mutex_unlock(&mutex);

	return result;
}

void monitor_pedido_default_put(char* id_pedido, t_pedido_default* pedido){
	pthread_mutex_lock(&mutex);
	dictionary_put(pedidos, id_pedido, pedido);
	pthread_mutex_unlock(&mutex);
}

t_pedido_default* monitor_pedido_default_get(char* id_pedido){
	t_pedido_default* pedido = NULL;
	pthread_mutex_lock(&mutex);
	pedido = dictionary_get(pedidos, id_pedido);
	pthread_mutex_unlock(&mutex);

	return pedido;
}

t_list* monitor_pedido_default_get_all(){
	t_list* pedidos_list = list_create();
	void add_pedido_to_list(char* key, void* value){
		t_pedido_default* pedido = (t_pedido_default*) value;
		list_add(pedidos_list, pedido);
	}

	pthread_mutex_lock(&mutex);
	dictionary_iterator(pedidos, add_pedido_to_list);
	pthread_mutex_unlock(&mutex);

	return pedidos_list;
}

void monitor_pedido_default_remove(char* id_pedido){
	pthread_mutex_lock(&mutex);
	dictionary_remove_and_destroy(pedidos, id_pedido, (void*)pedido_default_destroy);
	pthread_mutex_unlock(&mutex);
}
