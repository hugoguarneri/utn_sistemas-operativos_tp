#include "lru.h"

void lru_init() {
	lru_list = list_create();
	pthread_mutex_init(&lru_lock, NULL);
}

void lru_add_updated(int page) {
	// Busco si existe
	_Bool page_remove_condition(void *value) {
		int* valor = (int *)value;
		return *valor == page;
	}
	pthread_mutex_lock(&lru_lock);
	list_remove_and_destroy_by_condition(lru_list, page_remove_condition, free);
	list_add(lru_list, int_clone(page));
	pthread_mutex_unlock(&lru_lock);
}

void lru_remove(int page) {
	_Bool page_remove_condition(void *value) {
		int* valor = (int *)value;
		return *valor == page;
	}
	pthread_mutex_lock(&lru_lock);
	list_remove_and_destroy_by_condition(lru_list, page_remove_condition, free);
	pthread_mutex_unlock(&lru_lock);
}

int lru_get_candidate() {
	pthread_mutex_lock(&lru_lock);
	int *page = NULL;
	page = (int*)list_get(lru_list, 0);
	int valor = *page;
	list_remove_and_destroy_element(lru_list, 0, free);
	pthread_mutex_unlock(&lru_lock);
	return valor;
}

void lru_destroy() {
	pthread_mutex_destroy(&lru_lock);
	list_destroy_and_destroy_elements(lru_list, free);
}

