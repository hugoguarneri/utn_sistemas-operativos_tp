#include "cmd-seleccionar-restaurante.h"

void cmd_seleccionar_restaurante_exec(t_command* comando){

	if(!cmd_seleccionar_restaurante_validate(comando)){
		char* uso = cmd_seleccionar_restaurante_uso();
		printf("Los parametros ingresados no son validos.\n");
		printf("Uso: %s", uso);
		free(uso);
		return;
	}

	char* ip = strdup(config_get_string_value(config, "IP_SERVER"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_SERVER"));

	int socket_cliente = crear_conexion(ip, puerto);

	char* id_cliente = strdup(config_get_string_value(config, "ID_CLIENTE"));
	char* restaurante = list_get(comando->args, 0);

	t_seleccionar_restaurante_req* request = seleccionar_restaurante_req_create(id_cliente, restaurante);

	char* request_str = seleccionar_restaurante_req_to_string(request);
	log_info(logger, "Se va consultar sobre el restaurante: %s" ,request_str);

	t_paquete* paquete_req = seleccionar_restaurante_req_empaquetar(request);

	if(enviar_mensaje(socket_cliente, paquete_req) < 0){
		free(ip);
		free(puerto);
		seleccionar_restaurante_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_cliente);
		return;
	}

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);


	if(paquete_res->codigo_operacion != SELECCIONAR_RESTAURANTE){
		log_error(logger, "Ocurrio un error al recibir el paquete: %s", strerror(errno));
		printf("El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip);
		free(puerto);
		seleccionar_restaurante_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_cliente);
		paquete_destroy(paquete_res);
		return;
	}

	t_seleccionar_restaurante_res* response = seleccionar_restaurante_res_desempaquetar(paquete_res);

	char* response_str = seleccionar_restaurante_res_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s", response_str);

	printf("> %s \n", response->code == OK_ ? "OK" : "FAIL");

	free(ip);
	free(puerto);
	seleccionar_restaurante_req_destroy(request);
	free(request_str);
	paquete_destroy(paquete_req);
	liberar_conexion(socket_cliente);
	paquete_destroy(paquete_res);
	seleccionar_restaurante_res_destroy(response);
	free(response_str);
}

bool cmd_seleccionar_restaurante_validate(t_command* comando){
	return list_size(comando->args) == 1;
}

char* cmd_seleccionar_restaurante_uso(){
	return string_duplicate("SeleccionarRestaurante [RESTAURANTE]");
}
