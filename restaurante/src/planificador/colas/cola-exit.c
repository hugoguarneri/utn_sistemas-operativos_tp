#include "cola-exit.h"

static void informar_finalizacion_de_plato(t_pcb* pcb);

void cola_exit_listener(){

	sem_t* sem_cola_exit = dictionary_get(cola_exit->dic_semaforos, COLA_EXIT);

	while(1){

		sem_wait(sem_cola_exit);

		pthread_mutex_lock(&mutex_cola_exit);

		t_pcb* pcb = pcb_duplicate(cola_planificacion_get_pcb(cola_exit, COLA_EXIT, 0));
		cola_planificacion_remove_pcb(cola_exit, COLA_EXIT, pcb->id);

		pthread_mutex_unlock(&mutex_cola_exit);

		informar_finalizacion_de_plato(pcb);
		pcb_destroy(pcb);
	}
}

static void informar_finalizacion_de_plato(t_pcb* pcb){
	char* ip_sindicato =  strdup(config_get_string_value(config, "IP_SINDICATO"));
	char* puerto_sindicato = strdup(config_get_string_value(config, "PUERTO_SINDICATO"));

	char* ip_app =  strdup(config_get_string_value(config, "IP_APP"));
	char* puerto_app = strdup(config_get_string_value(config, "PUERTO_APP"));

	char* restaurante_name = strdup(config_get_string_value(config, "NOMBRE_RESTAURANTE"));

	t_plato_listo_req* request = plato_listo_req_create(restaurante_name, pcb->id_pedido, pcb->plato);
	t_paquete* paquete_req = plato_listo_req_empaquetar(request);

	char* request_str = plato_listo_req_to_string(request);
	log_info(logger, "[PLANIFICADOR] - Se va a enviar a Sindicato el mensaje: %s", request_str);
	log_info(logger, "[PLANIFICADOR] - Se va a enviar a App el mensaje: %s", request_str);

	int socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	int socket_app = crear_conexion(ip_app, puerto_app);

	enviar_mensaje(socket_sindicato, paquete_req);
	enviar_mensaje(socket_app, paquete_req);

	t_paquete* paquete_res_sind = recibir_mensaje(socket_sindicato);
	t_paquete* paquete_res_app = recibir_mensaje(socket_app);

	t_plato_listo_res* response_sind = plato_listo_res_desempaquetar(paquete_res_sind);
	t_plato_listo_res* response_app = plato_listo_res_desempaquetar(paquete_res_app);

	char* response_str_sind = plato_listo_res_to_string(response_sind);
	char* response_str_app = plato_listo_res_to_string(response_app);

	log_info(logger, "[PLANIFICADOR] - Se recibió de Sindicato el mensaje: %s", response_str_sind);
	log_info(logger, "[PLANIFICADOR] - Se recibió de App el mensaje: %s", response_str_app);


	free(ip_sindicato);
	free(puerto_sindicato);
	free(ip_app);
	free(puerto_app);
	plato_listo_req_destroy(request);
	paquete_destroy(paquete_req);
	free(request_str);
	paquete_destroy(paquete_res_sind);
	paquete_destroy(paquete_res_app);
	plato_listo_res_destroy(response_sind);
	plato_listo_res_destroy(response_app);
	free(response_str_sind);
	free(response_str_app);
	liberar_conexion(socket_sindicato);
	liberar_conexion(socket_app);
}
