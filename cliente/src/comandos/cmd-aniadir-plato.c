#include "cmd-aniadir-plato.h"

void cmd_aniadir_plato_exec(t_command* comando){

	if(!cmd_aniadir_plato_validate(comando)){
		char* uso = cmd_aniadir_plato_uso();
		printf("Los parametros ingresados no son validos.\n");
		printf("Uso: %s", uso);
		free(uso);
		return;
	}

	char* ip = strdup(config_get_string_value(config, "IP_SERVER"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_SERVER"));

	int socket_cliente = crear_conexion(ip, puerto);

	char* plato = list_get(comando->args, 0);
	char* id_pedido = list_get(comando->args, 1);
	t_aniadir_plato_req* request = aniadir_plato_req_create(plato, id_pedido);

	char* request_str = aniadir_plato_req_to_string(request);
	log_info(logger, "Se va a enviar el mensaje: %s", request_str);

	t_paquete* paquete_req = aniadir_plato_req_empaquetar(request);
	enviar_mensaje(socket_cliente, paquete_req);

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion == HANDSHAKE){
		t_paquete* ic_paquete_res = consultar_info_cliente_res_empaquetar(info_cliente);
		enviar_mensaje(socket_cliente, ic_paquete_res);

		paquete_res = recibir_mensaje(socket_cliente);
	}

	if(paquete_res->codigo_operacion != ANIADIR_PLATO){
		log_error(logger, "Ocurrio un error al recibir el paquete: %s", strerror(errno));
		printf("El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip);
		free(puerto);
		aniadir_plato_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_cliente);
		return;
	}

	t_aniadir_plato_res* response = aniadir_plato_res_desempaquetar(paquete_res);

	char* response_str = aniadir_plato_res_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s", response_str);

	printf("> %s\n", response->code == OK_ ? "OK" : "FAIL");

	free(ip);
	free(puerto);
	aniadir_plato_req_destroy(request);
	free(request_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	aniadir_plato_res_destroy(response);
	free(response_str);

	liberar_conexion(socket_cliente);
}

bool cmd_aniadir_plato_validate(t_command* comando){
	return list_size(comando->args) == 2;
}

char* cmd_aniadir_plato_uso(){
	return string_duplicate("AniadirPlato [PLATO] [ID_PEDIDO]");
}
