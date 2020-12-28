#include "cola-exec.h"

static void preparar_plato(t_pcb* pcb);

void cola_exec_listener(char* afinidad){

	while(1){

		sem_t* sem_cola_exec = dictionary_get(cola_exec->dic_semaforos, afinidad);
		sem_wait(sem_cola_exec);

		int size = cola_planificacion_size(cola_exec, afinidad);
		t_pcb* pcb = cola_planificacion_get_pcb(cola_exec, afinidad, size - 1);

		pthread_t thread;
		pthread_create(&thread, NULL, (void*) preparar_plato, pcb);
		pthread_detach(thread);
	}
}


static void preparar_plato(t_pcb* pcb){
	int retardo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
	char* algoritmo = strdup(config_get_string_value(config, "ALGORITMO_PLANIFICACION"));

	while(1){
		sleep(retardo_cpu);

		t_cocinero* cocinero = monitor_cocineros_get(pcb->id_cocinero);
		t_paso_info* paso_info = list_get(pcb->pasos_tiempo, pcb->index_paso);


		if(strcmp(paso_info->paso, HORNEAR) != 0 && strcmp(paso_info->paso, REPOSAR) != 0){
			pcb->tiempo_paso = pcb->tiempo_paso + 1;
			pcb->rafaga_cpu = pcb->rafaga_cpu + 1;

			if(pcb->quantum == pcb->rafaga_cpu && strcmp(algoritmo, "RR") == 0){

				if(pcb->index_paso != list_size(pcb->pasos_tiempo) && pcb->tiempo_paso != paso_info->tiempo){

					if(pcb->tiempo_paso == paso_info->tiempo){
						pcb->index_paso = pcb->index_paso + 1;
						pcb->tiempo_paso = 0;
					}

					log_warning(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se va a [%s].",
									pcb->id_pedido, pcb->id, paso_info->paso, (paso_info->tiempo - pcb->tiempo_paso));

					pcb->rafaga_cpu = 0;
					log_debug(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se movió pcb a cola de ready. Fin de quantum - { plato: %s, cocinero: %s}",
							pcb->id_pedido, pcb->id, pcb->plato, pcb->id_cocinero);

					cola_planificacion_add_pcb(cola_ready, cocinero->afinidad, pcb_duplicate(pcb));
					cola_planificacion_remove_pcb(cola_exec, cocinero->afinidad, pcb->id);

					sem_t* sem_cocinero_libre = dictionary_get(sems_cocineros_libres, cocinero->afinidad);
					sem_post(sem_cocinero_libre);

					sem_t* sem_cola_ready = dictionary_get(cola_ready->dic_semaforos, cocinero->afinidad);
					sem_post(sem_cola_ready);

					break;
				}
			}
		}

		if(pcb->tiempo_paso == paso_info->tiempo){
			pcb->index_paso = pcb->index_paso + 1;
			pcb->tiempo_paso = 0;
		}

		log_warning(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se va a [%s].",
				pcb->id_pedido, pcb->id, paso_info->paso, (paso_info->tiempo - pcb->tiempo_paso));

		if(pcb->index_paso == list_size(pcb->pasos_tiempo) && pcb->tiempo_paso == 0){

			pcb->rafaga_cpu = 0;

			log_debug(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se movió pcb a cola de exit. Plato listo - { plato: %s, cocinero: %s}",
					pcb->id_pedido, pcb->id, pcb->plato, pcb->id_cocinero);

			pthread_mutex_lock(&mutex_cola_exit);
			cola_planificacion_add_pcb(cola_exit, COLA_EXIT, pcb_duplicate(pcb));
			pthread_mutex_unlock(&mutex_cola_exit);

			cola_planificacion_remove_pcb(cola_exec, cocinero->afinidad, pcb->id);

			monitor_cocineros_update_estado(cocinero->id_cocinero, LIBRE);

			sem_t* sem_cocinero_libre = dictionary_get(sems_cocineros_libres, cocinero->afinidad);
			sem_post(sem_cocinero_libre);

			sem_t* sem_cola_exit = dictionary_get(cola_exit->dic_semaforos, COLA_EXIT);
			sem_post(sem_cola_exit);

			break;
		}

		if(strcmp(paso_info->paso, HORNEAR) == 0){

			pcb->rafaga_cpu = 0;

			log_debug(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se movió pcb a cola de block horno - { plato: %s, cocinero: %s}",
					pcb->id_pedido, pcb->id, pcb->plato, pcb->id_cocinero);

			cola_planificacion_add_pcb(cola_block_horno, COLA_BLOCK_HORNO, pcb_duplicate(pcb));
			pedir_horno(pcb->id);
			cola_planificacion_remove_pcb(cola_exec, cocinero->afinidad, pcb->id);

			monitor_cocineros_update_estado(cocinero->id_cocinero, LIBRE);
			sem_t* sem_cocinero_libre = dictionary_get(sems_cocineros_libres, cocinero->afinidad);
			sem_post(sem_cocinero_libre);

			sem_t* sem_cola_block_horno = dictionary_get(cola_block_horno->dic_semaforos, COLA_BLOCK_HORNO);
			sem_post(sem_cola_block_horno);

			break;
		}

		if(strcmp(paso_info->paso, REPOSAR) == 0){

			pcb->rafaga_cpu = 0;

			log_debug(logger, "[PLANIFICADOR] - PEDIDO [%s] - PCB [%s] - Se movió pcb a cola de block reposo - { plato: %s, cocinero: %s}",
					pcb->id_pedido, pcb->id, pcb->plato, pcb->id_cocinero);

			cola_planificacion_add_pcb(cola_block_reposo, COLA_BLOCK_REPOSO, pcb_duplicate(pcb));
			cola_planificacion_remove_pcb(cola_exec, cocinero->afinidad, pcb->id);

			monitor_cocineros_update_estado(cocinero->id_cocinero, LIBRE);

			sem_t* sem_cocinero_libre = dictionary_get(sems_cocineros_libres, cocinero->afinidad);
			sem_post(sem_cocinero_libre);

			sem_t* sem_cola_block_reposo = dictionary_get(cola_block_reposo->dic_semaforos, COLA_BLOCK_REPOSO);
			sem_post(sem_cola_block_reposo);
			break;
		}

	}
}
