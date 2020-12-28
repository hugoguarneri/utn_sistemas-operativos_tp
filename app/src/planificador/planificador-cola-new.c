#include "planificador-cola-new.h"

void cola_new_listener(){
	while(1){
		sem_wait(&sem_cola_new);
		sem_wait(&sem_repartidores_libres);

		t_pcb* pcb = pcb_duplicate(cola_planificacion_get(cola_new, 0));

		t_info_restaurante* info_restaurante = monitor_info_restaurantes_get(pcb->restaurante);
		t_list* repartidores = monitor_repartidores_get_all();

		uint32_t restaurante_x = info_restaurante->pos_x;
		uint32_t restaurante_y = info_restaurante->pos_y;

		bool is_repartidor_libre(void* element){
			t_repartidor* repartidor = (t_repartidor*) element;
			return repartidor->estado == LIBRE ? true : false;
		}

		t_repartidor* repartidor = list_find(repartidores, is_repartidor_libre);
		for(int i = 0; i < list_size(repartidores); i++){
			t_repartidor* repartidor_aux = list_get(repartidores, i);
			if(!is_repartidor_libre(repartidor_aux))
				continue;
			float d1 = distancia(repartidor->pos_x, repartidor->pos_y, restaurante_x, restaurante_y);
			float d2 = distancia(repartidor_aux->pos_x, repartidor_aux->pos_y, restaurante_x, restaurante_y);
			repartidor = (d2 < d1) ? repartidor_aux : repartidor;

			log_warning(logger, "[PLANIFICADOR] - Repartidor %s ubicacion (%d,%d) - Restaurante destino ubicacion (%d,%d)",
				repartidor_aux->id_repartidor, repartidor_aux->pos_x, repartidor_aux->pos_y, restaurante_x, restaurante_y);
		}

		repartidor->estado = OCUPADO;
		pcb->id_repartidor = strdup(repartidor->id_repartidor);

		log_debug(logger, "[PLANIFICADOR] - Se asigno el [Repartidor %s] al [pcb %s]. Ubicacion del repartidor (%d,%d), destino [Restaurante %s, (%d,%d)] ",
				pcb->id_repartidor, pcb->id, repartidor->pos_x, repartidor->pos_y, pcb->restaurante, restaurante_x, restaurante_y);

		pthread_mutex_lock(&mutex_cola_ready);
		cola_planificacion_remove(cola_new, pcb->id);
		pthread_mutex_unlock(&mutex_cola_ready);
		cola_planificacion_add(cola_ready, pcb);

		log_debug(logger, "[PLANIFICADOR] - Se movio [pcb %s] a COLA READY - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
				pcb->id, pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);
		sem_post(&sem_cola_ready);
	}
}

