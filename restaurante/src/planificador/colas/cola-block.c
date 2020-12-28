#include "cola-block.h"

void horno(){

	sem_t* sem_cola_block_horno = dictionary_get(cola_block_horno->dic_semaforos, COLA_BLOCK_HORNO);
	int retardo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");

	while(1){
		sem_wait(sem_cola_block_horno);

		pthread_mutex_lock(&mutex_cola_prioridades_horno);

		char* id_pcb = strdup(list_get(lista_prioridad_horno, 0));
		list_remove_and_destroy_element(lista_prioridad_horno, 0, free);

		pthread_mutex_unlock(&mutex_cola_prioridades_horno);

		t_pcb* pcb = cola_planificacion_get_pcb_from_id(cola_block_horno, COLA_BLOCK_HORNO, id_pcb);
		t_paso_info* paso_info = list_get(pcb->pasos_tiempo, pcb->index_paso);
		t_cocinero* cocinero = monitor_cocineros_get(pcb->id_cocinero);

		for(int i = 0; i < paso_info->tiempo; i++){
			sleep(retardo_cpu);

			pcb->tiempo_paso = pcb->tiempo_paso + 1;
			if(paso_info->tiempo == pcb->tiempo_paso){

				pcb->index_paso = pcb->index_paso + 1;
				pcb->tiempo_paso = 0;

				log_debug(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se movió pcb a cola de ready. Termino de hornearse - { plato: %s, cocinero: %s}",
						pcb->id_pedido, pcb->id, pcb->plato, pcb->id_cocinero);

				cola_planificacion_add_pcb(cola_ready, cocinero->afinidad, pcb_duplicate(pcb));
				cola_planificacion_remove_pcb(cola_block_horno, COLA_BLOCK_HORNO, pcb->id);

				sem_t* sem_cola_ready = dictionary_get(cola_ready->dic_semaforos, cocinero->afinidad);
				sem_post(sem_cola_ready);

				break;
			}

			log_warning(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se va a [%s].",
					pcb->id_pedido, pcb->id, paso_info->paso, (paso_info->tiempo - pcb->tiempo_paso));
		}
	}
}

void cola_block_reposo_listener(){
	//sem_t* sem_cola_block_reposo = dictionary_get(cola_block_reposo->dic_semaforos, COLA_BLOCK_REPOSO);
	int retardo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");

	while(1){
		sleep(retardo_cpu);
		//sem_wait(sem_cola_block_reposo);

		for(int i = 0; i < cola_planificacion_size(cola_block_reposo, COLA_BLOCK_REPOSO) ; i++){
			t_pcb* pcb = cola_planificacion_get_pcb(cola_block_reposo, COLA_BLOCK_REPOSO, i);
			t_paso_info* paso_info = list_get(pcb->pasos_tiempo, pcb->index_paso);
			t_cocinero* cocinero = monitor_cocineros_get(pcb->id_cocinero);

			pcb->tiempo_paso = pcb->tiempo_paso + 1;

			if(paso_info->tiempo == pcb->tiempo_paso){

				pcb->index_paso = pcb->index_paso + 1;
				pcb->tiempo_paso = 0;

				log_debug(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se movió pcb a cola de ready. Termino reposo - { plato: %s, cocinero: %s}",
						pcb->id_pedido, pcb->id, pcb->plato, pcb->id_cocinero);

				cola_planificacion_add_pcb(cola_ready, cocinero->afinidad, pcb_duplicate(pcb));
				cola_planificacion_remove_pcb(cola_block_reposo, COLA_BLOCK_REPOSO, pcb->id);

				sem_t* sem_cola_ready = dictionary_get(cola_ready->dic_semaforos, cocinero->afinidad);
				sem_post(sem_cola_ready);

				break;
			}

			log_warning(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se va a [%s].",
					pcb->id_pedido, pcb->id, paso_info->paso, (paso_info->tiempo - pcb->tiempo_paso));
		}
	}
}

void cola_block_horno_listener(t_obtener_restaurante_res* info_restaurante){

	for(int i = 0; i < info_restaurante->cant_hornos; i++) {
		pthread_t thread;
		pthread_create(&thread, NULL, (void*) horno, NULL);
		pthread_detach(thread);
	}
}

void pedir_horno(char* pcb_id){
	pthread_mutex_lock(&mutex_cola_prioridades_horno);
	list_add(lista_prioridad_horno, strdup(pcb_id));
	pthread_mutex_unlock(&mutex_cola_prioridades_horno);
}
