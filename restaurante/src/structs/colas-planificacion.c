#include <structs/colas-planificacion.h>

void cola_planificacion_init(t_cola_planif** cola_plani){
	*cola_plani = malloc(sizeof(t_cola_planif));
	(*cola_plani)->dic_colas = dictionary_create();
	(*cola_plani)->dic_semaforos = dictionary_create();
	pthread_mutex_init(&(*cola_plani)->mutex, NULL);
}

void cola_planificacion_destroy(t_cola_planif* cola_plani){
	pthread_mutex_destroy(&cola_plani->mutex);
	dictionary_destroy_and_destroy_elements(cola_plani->dic_colas, (void*)pcb_destroy);
	dictionary_destroy_and_destroy_elements(cola_plani->dic_semaforos, (void*)sem_destroy);
	free(cola_plani);
}

void cola_planificacion_create(t_cola_planif* cola_plani, char* key){
	t_list* cola = list_create();
	dictionary_put(cola_plani->dic_colas, strdup(key), cola);

	sem_t* sem = malloc(sizeof(sem_t));
	sem_init(sem, 0, 0);
	dictionary_put(cola_plani->dic_semaforos, strdup(key), sem);
}

int cola_planificacion_size(t_cola_planif* cola_plani, char* key){
	int size = 0;
	pthread_mutex_lock(&cola_plani->mutex);
	t_list* cola = dictionary_get(cola_plani->dic_colas, key);
	size = list_size(cola);
	pthread_mutex_unlock(&cola_plani->mutex);

	return size;
}

void cola_planificacion_add_pcb(t_cola_planif* cola_plani, char* key, t_pcb* pbc){
	pthread_mutex_lock(&cola_plani->mutex);
	t_list* cola = dictionary_get(cola_plani->dic_colas, key);
	list_add(cola, pbc);
	pthread_mutex_unlock(&cola_plani->mutex);
}

t_pcb* cola_planificacion_get_pcb(t_cola_planif* cola_plani, char* key, int index){
	t_pcb* pcb = NULL;
	pthread_mutex_lock(&cola_plani->mutex);
	t_list* cola = dictionary_get(cola_plani->dic_colas, key);
	pcb = list_get(cola, index);
	pthread_mutex_unlock(&cola_plani->mutex);

	return pcb;
}

t_pcb* cola_planificacion_get_pcb_from_id(t_cola_planif* cola_plani, char* key, char* id_pcb){
	int index;
	t_pcb* pcb = NULL;
	pthread_mutex_lock(&cola_plani->mutex);
	t_list* cola = dictionary_get(cola_plani->dic_colas, key);

	for(index = 0; index < list_size(cola); index++){
		t_pcb* pcb_aux = list_get(cola, index);
		if(strcmp(id_pcb, pcb_aux->id) == 0)
			break;
	}
	pcb = list_get(cola, index);
	pthread_mutex_unlock(&cola_plani->mutex);
	return pcb;
}

void cola_planificacion_remove_pcb(t_cola_planif* cola_plani, char* key, char* id_pcb){
	pthread_mutex_lock(&cola_plani->mutex);
	int index;
	t_list* cola = dictionary_get(cola_plani->dic_colas, key);

	for(index = 0; index < list_size(cola); index++){
		t_pcb* pcb_aux = list_get(cola, index);
		if(!strcmp(id_pcb, pcb_aux->id))
			break;
	}
	list_remove_and_destroy_element(cola, index, (void*)pcb_destroy);
	pthread_mutex_unlock(&cola_plani->mutex);
}

