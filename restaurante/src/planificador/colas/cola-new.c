#include "cola-new.h"

void cola_new_listener(char* afinidad){

	sem_t* sem_cola_new = dictionary_get(cola_new->dic_semaforos, afinidad);

	while(1){
		sem_wait(sem_cola_new);

		t_pcb* pcb = pcb_duplicate(cola_planificacion_get_pcb(cola_new, afinidad, 0));
		cola_planificacion_remove_pcb(cola_new, afinidad, pcb->id);
		t_list* cocineros = monitor_cocineros_get_all();

		bool cocinero_cumple_condicion(void* element){
			t_cocinero* cocinero = (t_cocinero*) element;
			return strcmp(cocinero->afinidad, pcb->plato) == 0 || strcmp(cocinero->afinidad, AFINIDAD_DEFAULT) == 0;
		}

		t_cocinero* cocinero = list_find(cocineros, cocinero_cumple_condicion);

		pcb->id_cocinero = strdup(cocinero->id_cocinero);

		log_debug(logger, "[PLANIFICADOR] - PEDIDO [%s] - PBC [%s] - Se asignó el cocinero [%s] al plato [%s].",
				pcb->id_pedido, pcb->id, pcb->id_cocinero, pcb->plato);

		cola_planificacion_add_pcb(cola_ready, afinidad, pcb);

		log_debug(logger, "[PLANIFICADOR] - PEDIDO [%s] - PBC [%s] - Se movió pcb a cola de ready - { plato: %s, cocinero: %s}",
				pcb->id_pedido, pcb->id, pcb->plato, pcb->id_cocinero);

		sem_t* sem_cola_ready = dictionary_get(cola_ready->dic_semaforos, afinidad);
		sem_post(sem_cola_ready);
	}
}
