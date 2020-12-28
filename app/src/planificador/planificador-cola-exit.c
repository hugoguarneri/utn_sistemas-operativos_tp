#include "planificador-cola-exit.h"

static void informar_a_comanda_finalizacion_de_pedido(t_pcb* pcb);
static void informar_a_cliente_entrega_de_pedido(t_pcb* pcb);

void cola_exit_listener(){
	while(1){
		sem_wait(&sem_cola_exit);
		t_pcb* pcb = cola_planificacion_get(cola_exit, 0);

		informar_a_comanda_finalizacion_de_pedido(pcb);
		informar_a_cliente_entrega_de_pedido(pcb);

		cola_planificacion_remove(cola_exit, pcb->id);
	}

}


static void informar_a_comanda_finalizacion_de_pedido(t_pcb* pcb){
	char* ip_comanda =  strdup(config_get_string_value(config, "IP_COMANDA"));
	char* puerto_comanda = strdup(config_get_string_value(config, "PUERTO_COMANDA"));

	t_finalizar_pedido_req* request = finalizar_pedido_req_create(pcb->id_pedido, pcb->restaurante);
	t_paquete* paquete_req = finalizar_pedido_req_empaquetar(request);

	char* request_str = finalizar_pedido_req_to_string(request);
	log_info(logger, "[PLANIFICADOR] - Se va a enviar a Comanda el mensaje: %s", request_str);

	int socket_comanda = crear_conexion(ip_comanda, puerto_comanda);

	enviar_mensaje(socket_comanda, paquete_req);

	t_paquete* paquete_res = recibir_mensaje(socket_comanda);

	t_finalizar_pedido_res* response = finalizar_pedido_res_desempaquetar(paquete_res);

	char* response_str = finalizar_pedido_res_to_string(response);
	log_info(logger, "[PLANIFICADOR] - Se recibio de Comanda el mensaje: %s", response_str);


	free(ip_comanda);
	free(puerto_comanda);
	finalizar_pedido_req_destroy(request);
	paquete_destroy(paquete_req);
	free(request_str);
	paquete_destroy(paquete_res);
	finalizar_pedido_res_destroy(response);
	free(response_str);
	liberar_conexion(socket_comanda);
}

static void informar_a_cliente_entrega_de_pedido(t_pcb* pcb){
	t_info_cliente* info_cliente = monitor_info_clientes_get(pcb->id_cliente);
	char* ip_cliente = strdup(info_cliente->ip);
	char* puerto_cliente = strdup(info_cliente->puerto);

	t_finalizar_pedido_req* request = finalizar_pedido_req_create(pcb->id_pedido, pcb->restaurante);
	t_paquete* paquete_req = finalizar_pedido_req_empaquetar(request);

	char* request_str = finalizar_pedido_req_to_string(request);
	log_info(logger, "[PLANIFICADOR] - Se va a enviar a Cliente %s el mensaje: %s", pcb->id_cliente, request_str);

	int socket_comanda = crear_conexion(ip_cliente, puerto_cliente);

	enviar_mensaje(socket_comanda, paquete_req);

	t_paquete* paquete_res = recibir_mensaje(socket_comanda);

	t_finalizar_pedido_res* response = finalizar_pedido_res_desempaquetar(paquete_res);

	char* response_str = finalizar_pedido_res_to_string(response);
	log_info(logger, "[PLANIFICADOR] - Se recibio de Cliente %s el mensaje: %s", pcb->id_cliente, response_str);

	free(ip_cliente);
	free(puerto_cliente);
	finalizar_pedido_req_destroy(request);
	paquete_destroy(paquete_req);
	free(request_str);
	paquete_destroy(paquete_res);
	finalizar_pedido_res_destroy(response);
	free(response_str);
	liberar_conexion(socket_comanda);
}
