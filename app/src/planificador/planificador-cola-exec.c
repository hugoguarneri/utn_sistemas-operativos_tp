#include "planificador-cola-exec.h"

static void entregar_pedido(t_pcb* pcb);
static void mover_repartidor(t_repartidor* repartidor, int destino_x, int destino_y);
static bool is_pedido_listo(char* id_pedido,  char* restaurante);

int retardo_cpu;

void cola_exec_listener(){
	retardo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");

	while(1){
		sem_wait(&sem_cola_exec);

		int size = cola_planificacion_size(cola_exec);
		t_pcb* pcb = cola_planificacion_get(cola_exec, size-1);

		pthread_t thread;
		pthread_create(&thread, NULL, (void*) entregar_pedido, pcb);
		pthread_detach(thread);
	}
}

static void entregar_pedido(t_pcb* pcb){
	while(1){
		sleep(retardo_cpu);

		t_repartidor* repartidor = monitor_repartidores_get(pcb->id_repartidor);
		t_info_restaurante* restaurante = monitor_info_restaurantes_get(pcb->restaurante);
		t_info_cliente* cliente = monitor_info_clientes_get(pcb->id_cliente);

		repartidor->tiempo = repartidor->tiempo+1;
		pcb->raf_contador = pcb->raf_contador +1;

		int destino_x = (pcb->destino == RESTAURANTE) ? restaurante->pos_x : cliente->pos_x;
		int destino_y = (pcb->destino == RESTAURANTE) ? restaurante->pos_y : cliente->pos_y;

		if(pcb->destino == RESTAURANTE && (repartidor->pos_x == destino_x && repartidor->pos_y == destino_y)) {
			log_debug(logger, "[PLANIFICADOR] - El [Repartidor %s] llego al [Restaurante %s]. Se va a consultar el estado del [Pedido %s] - [id_pcb: %s]",
					pcb->id_repartidor ,pcb->restaurante, pcb->id_pedido, pcb->id);

			if(is_pedido_listo(pcb->id_pedido, pcb->restaurante)) {
				pcb->destino = CLIENTE;
				destino_x = cliente->pos_x;
				destino_y = cliente->pos_y;
				log_debug(logger, "[PLANIFICADOR] - El [Restaurante %s] ya tiene el [Pedido %s] listo. Nuevo destino del [Repartidor %s] [Cliente %s (%d,%d)] - [id_pcb: %s]",
						pcb->restaurante, pcb->id_pedido, pcb->id_repartidor, pcb->id_cliente, destino_x, destino_y, pcb->id);
			} else {
				pcb->estado_repartidor = REPARTIDOR_ESPERANDO_PEDIDO;
				pcb->raf_anterior = pcb->raf_contador;
				pcb->raf_contador = 0;

				cola_planificacion_add(cola_block, pcb_duplicate(pcb));

				log_debug(logger, "[PLANIFICADOR] - Se movio [pcb %s] a COLA BLOCK. Reparidor llego  RESTAURANTE, pedido no listo - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
						pcb->id, pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);
				cola_planificacion_remove(cola_exec, pcb->id);

				sem_post(&sem_cola_block);
				sem_post(&sem_grado_multiprocesamiento);
				break;
			}
		}

		if(pcb->destino == CLIENTE && (repartidor->pos_x == destino_x && repartidor->pos_y == destino_y)) {
			pcb->raf_anterior = pcb->raf_contador;
			pcb->raf_contador = 0;
			repartidor->estado = LIBRE;
			sem_post(&sem_repartidores_libres);

			cola_planificacion_add(cola_exit, pcb_duplicate(pcb));

			log_debug(logger, "[PLANIFICADOR] - Se movio [pcb %s] a COLA EXIT. Repartidor llego a CLIENTE, pedido entregado - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
					pcb->id, pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);
			cola_planificacion_remove(cola_exec, pcb->id);

			sem_post(&sem_cola_exit);
			sem_post(&sem_grado_multiprocesamiento);
			break;
		}

		mover_repartidor(repartidor, destino_x, destino_y);
		log_debug(logger, "[PLANIFICADOR] - Se movio al [Repartidor %s] a (%d,%d) - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
					repartidor->id_repartidor, repartidor->pos_x, repartidor->pos_y  ,pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);

		if(repartidor->tiempo_de_actividad == repartidor->tiempo){
			pcb->raf_anterior = pcb->raf_contador;
			repartidor->tiempo = 0;
			pcb->estado_repartidor = REPARTIDOR_EN_DESCANSO;
			pcb->raf_contador = 0;

			cola_planificacion_add(cola_block, pcb_duplicate(pcb));

			log_debug(logger, "[PLANIFICADOR] - Se movio [pcb %s] a COLA BLOCK. Repartido entro en descanso - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
					pcb->id, pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);
			cola_planificacion_remove(cola_exec, pcb->id);

			sem_post(&sem_cola_block);
			sem_post(&sem_grado_multiprocesamiento);
			break;
		}
	}
}


static void mover_repartidor(t_repartidor* repartidor, int destino_x, int destino_y) {
	if (repartidor->pos_x != destino_x) {
		repartidor->pos_x = (repartidor->pos_x > destino_x) ? repartidor->pos_x -1 : repartidor->pos_x +1;
	} else {
		if (repartidor->pos_y != destino_y) {
			repartidor->pos_y = (repartidor->pos_y > destino_y) ? repartidor->pos_y -1 : repartidor->pos_y +1;
		}
	}
}

static bool is_pedido_listo(char* id_pedido,  char* restaurante){
	char* ip_comanda =  strdup(config_get_string_value(config, "IP_COMANDA"));
	char* puerto_comanda = strdup(config_get_string_value(config, "PUERTO_COMANDA"));

	int socket_comanda = crear_conexion(ip_comanda, puerto_comanda);

	t_obtener_pedido_req* request = obtener_pedido_req_create(restaurante, id_pedido);

	char* request_str = obtener_pedido_req_to_string(request);
	log_info(logger, "[PLANIFICADOR] - Se va a enviar a Comanda el mensaje: %s", request_str);

	t_paquete* paquete_req = obtener_pedido_req_empaquetar(request);
	enviar_mensaje(socket_comanda, paquete_req);

	t_paquete* paquete_res = recibir_mensaje(socket_comanda);
	t_obtener_pedido_res* response = obtener_pedido_res_desempaquetar(paquete_res);

	char* response_str = obtener_pedido_res_to_string(response);
	log_info(logger, "[PLANIFICADOR] - Se recibio de Comanda el mensaje: %s", response_str);

	bool is_pedido_listo = true;
	for(int i = 0; i < response->estado_platos->elements_count; i++){
		t_estado_plato* estado_plato = list_get(response->estado_platos, i);
		if(estado_plato->cant_lista < estado_plato->cant_total){
			is_pedido_listo = false;
			break;
		}
	}

	return is_pedido_listo;
}


