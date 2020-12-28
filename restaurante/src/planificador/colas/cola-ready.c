#include "cola-ready.h"


void cola_ready_listener(char* afinidad){
	//char* algoritmo = strdup(config_get_string_value(config, "ALGORITMO_PLANIFICACION"));

	sem_t* sem_cocinero_libre = dictionary_get(sems_cocineros_libres, afinidad);
	sem_t* sem_cola_ready = dictionary_get(cola_ready->dic_semaforos, afinidad);

	while(1){
		sem_wait(sem_cola_ready);
		log_debug(logger, "[PLANIFICADOR] - Esperando cocinero");

		sem_wait(sem_cocinero_libre);
		log_debug(logger, "[PLANIFICADOR] - Cocinero libre");

		t_pcb* pcb = pcb_duplicate(cola_planificacion_get_pcb(cola_ready, afinidad, 0));
		cola_planificacion_remove_pcb(cola_ready, afinidad, pcb->id);
		//TODO ver si el cocinero esta libre realmente.

		monitor_cocineros_update_estado(pcb->id_cocinero, OCUPADO);

		cola_planificacion_add_pcb(cola_exec, afinidad, pcb);

		log_debug(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se movió pcb a cola de exec - { plato: %s, cocinero: %s}",
				pcb->id_pedido, pcb->id, pcb->plato, pcb->id_cocinero);

		sem_t* sem_cola_exec = dictionary_get(cola_exec->dic_semaforos, afinidad);
		sem_post(sem_cola_exec);
	}
}
