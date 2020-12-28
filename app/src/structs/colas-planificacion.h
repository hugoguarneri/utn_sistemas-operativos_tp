#ifndef COLAS_PLANIFICACION_H_
#define COLAS_PLANIFICACION_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include "pedido-control-block.h"


/*
 * t_list: <t_pcb>
 */
typedef struct {
	//list<t_pcb>
	t_list* list;
	pthread_mutex_t mutex;
} t_cola_planificacion;


void cola_planificacion_init(t_cola_planificacion** cola_plani);

void cola_planificacion_destroy(t_cola_planificacion* cola_plani);

void cola_planificacion_add(t_cola_planificacion* cola_plani, t_pcb* pbc);

t_pcb* cola_planificacion_get(t_cola_planificacion* cola_plani, int index);

void cola_planificacion_remove(t_cola_planificacion* cola_plani, char* id_pcb);

void cola_planificacion_iterate(t_cola_planificacion* cola_plani, void(*closure)(void*));

t_pcb* cola_planificacion_find(t_cola_planificacion* cola_plani, bool(*closure)(void*));

void cola_planificacion_sort(t_cola_planificacion* cola_plani, bool (*comparator)(void *, void *));

int cola_planificacion_size(t_cola_planificacion* cola_plani);


#endif /* COLAS_PLANIFICACION_H_ */
