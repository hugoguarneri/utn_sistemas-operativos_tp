#include "colas-planificacion.h"

void cola_planificacion_init(t_cola_planificacion** cola_plani){
	*cola_plani = malloc(sizeof(t_cola_planificacion));
	(*cola_plani)->list = list_create();
	pthread_mutex_init(&(*cola_plani)->mutex, NULL);
}

void cola_planificacion_destroy(t_cola_planificacion* cola_plani){
	pthread_mutex_destroy(&cola_plani->mutex);
	list_destroy_and_destroy_elements(cola_plani->list, (void*)pcb_destroy);

}

void cola_planificacion_add(t_cola_planificacion* cola_plani, t_pcb* pbc){
	pthread_mutex_lock(&cola_plani->mutex);
	list_add(cola_plani->list, pbc);
	pthread_mutex_unlock(&cola_plani->mutex);
}

t_pcb* cola_planificacion_get(t_cola_planificacion* cola_plani, int index){
	t_pcb* pcb = NULL;
	pthread_mutex_lock(&cola_plani->mutex);
	pcb = list_get(cola_plani->list, index);
	pthread_mutex_unlock(&cola_plani->mutex);

	return pcb;
}

void cola_planificacion_remove(t_cola_planificacion* cola_plani, char* id_pcb){
	pthread_mutex_lock(&cola_plani->mutex);
	int index;
	for(index = 0; index < list_size(cola_plani->list); index++){
		t_pcb* pcb_aux = list_get(cola_plani->list, index);
		if(!strcmp(id_pcb, pcb_aux->id))
			break;
	}
	list_remove_and_destroy_element(cola_plani->list, index, (void*)pcb_destroy);
	pthread_mutex_unlock(&cola_plani->mutex);
}

void cola_planificacion_iterate(t_cola_planificacion* cola_plani, void(*closure)(void*)){
	pthread_mutex_lock(&cola_plani->mutex);
	list_iterate(cola_plani->list, closure);
	pthread_mutex_unlock(&cola_plani->mutex);
}

t_pcb* cola_planificacion_find(t_cola_planificacion* cola_plani, bool(*closure)(void*)){
	t_pcb* pcb = NULL;
	pthread_mutex_lock(&cola_plani->mutex);
	pcb = list_find(cola_plani->list, closure);
	pthread_mutex_unlock(&cola_plani->mutex);

	return pcb;
}

void cola_planificacion_sort(t_cola_planificacion* cola_plani, bool (*comparator)(void *, void *)){
	pthread_mutex_lock(&cola_plani->mutex);
	list_sort(cola_plani->list, comparator);
	pthread_mutex_unlock(&cola_plani->mutex);
}

int cola_planificacion_size(t_cola_planificacion* cola_plani){
	int size = 0;
	pthread_mutex_lock(&cola_plani->mutex);
	size = list_size(cola_plani->list);
	pthread_mutex_unlock(&cola_plani->mutex);

	return size;
}
