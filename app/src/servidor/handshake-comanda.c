#include "handshake-comanda.h"

int exec_hanshake_comanda(){
	log_info(logger, "[APP] - Conectando con COMANDA..");

	char* ip = strdup(config_get_string_value(config, "IP_COMANDA"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_COMANDA"));

	int socket_cliente = crear_conexion(ip, puerto);

	if(socket_cliente < 0){
		free(ip);
		free(puerto);
		return -1;
	}

	char* request_str = handshake_app_comanda_req_to_string();
	log_info(logger, "[APP] - Se va a enviar el mensaje a COMANDA: %s", request_str);

	t_paquete* paquete_req = handshake_app_comanda_req_empaquetar();

	if(enviar_mensaje(socket_cliente, paquete_req) < 0){
		log_error(logger, "[APP] - Ocurrio un error al intentar enviar el paquete: %s", strerror(errno));

		free(ip);
		free(puerto);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_cliente);
		return -1;
	}

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion != HANDSHAKE){
		log_error(logger, "[APP] - El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip);
		free(puerto);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_cliente);
		return -1;
	}

	t_handshake_app_comanda_res* response = handshake_app_comanda_res_desempaquetar(paquete_res);

	char* response_str = handshake_app_comanda_res_to_string(response);
	log_info(logger, "[APP] - Se recibio el mensaje de COMANDA: %s", response_str);
	status_code code = response->code;
	free(ip);
	free(puerto);
	free(request_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	handshake_app_comanda_res_destroy(response);
	free(response_str);
	liberar_conexion(socket_cliente);

	return (int)code;
}
