#include "cmd-confirmar-pedido.h"

void cmd_confirmar_pedido_exec(t_command* comando){

	if(!cmd_confirmar_pedido_validate(comando)){
		char* uso = cmd_confirmar_pedido_uso();
		printf("Los parametros ingresados no son validos.\n");
		printf("Uso: %s", uso);
		free(uso);
		return;
	}

	char* ip = strdup(config_get_string_value(config, "IP_SERVER"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_SERVER"));

	int socket_cliente = crear_conexion(ip, puerto);

	char* id_pedido = list_get(comando->args, 1);

	char* restaurante = list_get(comando->args, 0);
	t_confirmar_pedido_req* request = confirmar_pedido_req_create(id_pedido, restaurante);

	char* request_str = confirmar_pedido_req_to_string(request);
	log_info(logger, "Se va a enviar el mensaje: %s", request_str);

	t_paquete* paquete_req = confirmar_pedido_req_empaquetar(request);
	enviar_mensaje(socket_cliente,  paquete_req);

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion == HANDSHAKE){
		t_paquete* ic_paquete_res = consultar_info_cliente_res_empaquetar(info_cliente);
		enviar_mensaje(socket_cliente, ic_paquete_res);

		paquete_res = recibir_mensaje(socket_cliente);
	}

	if(paquete_res->codigo_operacion != CONFIRMAR_PEDIDO){
		log_error(logger, "Ocurrio un error al recibir el paquete: %s", strerror(errno));
		printf("El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip);
		free(puerto);
		confirmar_pedido_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_cliente);
		return;
	}

	t_confirmar_pedido_res* response = confirmar_pedido_res_desempaquetar(paquete_res);

	char* response_str = confirmar_pedido_res_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s", response_str);

	printf("> %s\n", response->code == OK_ ? "OK" : "FAIL");

	free(ip);
	free(puerto);
	confirmar_pedido_req_destroy(request);
	free(request_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	confirmar_pedido_res_destroy(response);
	free(response_str);

	liberar_conexion(socket_cliente);
}

bool cmd_confirmar_pedido_validate(t_command* comando){
	return list_size(comando->args) == 2;
}

char* cmd_confirmar_pedido_uso(){
	return string_duplicate("ConfirmarPedido [ID_PEDIDO] [RESTAURANTE]");
}
