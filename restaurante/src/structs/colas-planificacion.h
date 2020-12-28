#ifndef SRC_STRUCTS_COLAS_PLANIFICACION_H_
#define SRC_STRUCTS_COLAS_PLANIFICACION_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

#include "plato-control-block.h"

typedef struct {
	pthread_mutex_t mutex;

	//t_dictionary<char*, t_list<t_pcb>>
	t_dictionary* dic_colas;

	//t_dictionary<char*, sem_t>
	t_dictionary* dic_semaforos;
} t_cola_planif;


void cola_planificacion_init(t_cola_planif** cola_plani);

void cola_planificacion_destroy(t_cola_planif* cola_plani);

void cola_planificacion_create(t_cola_planif* cola_plani, char* key);

int cola_planificacion_size(t_cola_planif* cola_plani, char* key);

void cola_planificacion_add_pcb(t_cola_planif* cola_plani, char* key, t_pcb* pbc);

t_pcb* cola_planificacion_get_pcb(t_cola_planif* cola_plani, char* key, int index);

t_pcb* cola_planificacion_get_pcb_from_id(t_cola_planif* cola_plani, char* key, char* id_pcb);

void cola_planificacion_remove_pcb(t_cola_planif* cola_plani, char* key, char* id_pcb);

#endif /* SRC_STRUCTS_COLAS_PLANIFICACION_H_ */
