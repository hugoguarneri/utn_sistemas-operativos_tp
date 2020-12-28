#include "planificador-cola-block.h"

void cola_block_listener(){
	int retardo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");

	while(1){
		sleep(retardo_cpu);

		pthread_mutex_lock(&mutex_cola_block);
		for(int i = 0; i < cola_planificacion_size(cola_block); i++){
			t_pcb* pcb = cola_planificacion_get(cola_block, i);

			if(pcb->estado_repartidor != REPARTIDOR_EN_DESCANSO)
				continue;

			t_repartidor* repartidor = monitor_repartidores_get(pcb->id_repartidor);
			repartidor->tiempo = repartidor->tiempo+1;
			log_warning(logger, "repartidor: %s tiempo: %d", repartidor->id_repartidor, repartidor->tiempo);

			if(repartidor->tiempo_de_descanso == repartidor->tiempo){
				repartidor->tiempo = 0;
				pcb->estado_repartidor =  ACTIVO;

				pthread_mutex_lock(&mutex_cola_ready);
				cola_planificacion_add(cola_ready, pcb_duplicate(pcb));
				pthread_mutex_unlock(&mutex_cola_ready);

				log_debug(logger, "[PLANIFICADOR] - Se movio [pcb %s] a COLA READY. Repartidor termino descanso - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
						pcb->id, pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);
				cola_planificacion_remove(cola_block, pcb->id);

				sem_post(&sem_cola_ready);
				i--;
			}
		}
		pthread_mutex_unlock(&mutex_cola_block);
	}
}


void desbloquear_pedido_listo(char* id_pedido, char* restaurante){
	log_debug(logger, "[APP] desbloqueando pedido listo");
	pthread_mutex_lock(&mutex_cola_block);
	for(int i = 0; i < cola_planificacion_size(cola_block); i++){
		t_pcb* pcb = cola_planificacion_get(cola_block, i);
		if(strcmp(pcb->id_pedido,id_pedido) == 0 && strcmp(pcb->restaurante, restaurante) == 0){
			if(pcb->estado_repartidor == REPARTIDOR_ESPERANDO_PEDIDO){
				pcb->estado_repartidor =  ACTIVO;

				log_debug(logger, "[PLANIFICADOR] - Se movio [pcb %s] a COLA READY. El pedido ya esta listo - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
						pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);

				pthread_mutex_lock(&mutex_cola_ready);
				cola_planificacion_add(cola_ready, pcb_duplicate(pcb));
				pthread_mutex_unlock(&mutex_cola_ready);

				cola_planificacion_remove(cola_block, pcb->id);

				sem_post(&sem_cola_ready);
				i--;
			}
		}
	}
	free(id_pedido);
	free(restaurante);
	pthread_mutex_unlock(&mutex_cola_block);
}
