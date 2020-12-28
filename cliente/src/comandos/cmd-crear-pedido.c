#include "cmd-crear-pedido.h"

void cmd_crear_pedido_exec(t_command* comando){

	if(!cmd_crear_pedido_validate(comando)){
		char* uso = cmd_crear_pedido_uso();
		printf("Los parametros ingresados no son validos.\n");
		printf("Uso: %s", uso);
		free(uso);
		return;
	}

	char* ip = strdup(config_get_string_value(config, "IP_SERVER"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_SERVER"));

	int socket_cliente = crear_conexion(ip, puerto);

	char* request_str = crear_pedido_req_to_string();
	log_info(logger, "Se va a enviar el mensake: %s", request_str);

	t_paquete* paquete_req = crear_pedido_req_empaquetar();
	enviar_mensaje(socket_cliente,  paquete_req);

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion == HANDSHAKE){
		t_paquete* ic_paquete_res = consultar_info_cliente_res_empaquetar(info_cliente);
		enviar_mensaje(socket_cliente, ic_paquete_res);

		paquete_res = recibir_mensaje(socket_cliente);
	}

	if(paquete_res->codigo_operacion != CREAR_PEDIDO){
		log_error(logger, "Ocurrio un error al recibir el paquete: %s", strerror(errno));
		printf("El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip);
		free(puerto);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_cliente);
		return;
	}

	t_crear_pedido_res* response = crear_pedido_res_desempaquetar(paquete_res);

	char* response_str = crear_pedido_res_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s", response_str);

	char* mensaje_str = string_from_format("> Id Pedido: %s", response->id_pedido);
	printf("%s \n", mensaje_str);

	free(ip);
	free(puerto);
	free(request_str);
	free(response_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	crear_pedido_res_destroy(response);
	free(mensaje_str);

	liberar_conexion(socket_cliente);
}

bool cmd_crear_pedido_validate(t_command* comando){
	return list_size(comando->args) == 0;
}

char* cmd_crear_pedido_uso(){
	return string_duplicate("CrearPedido");
}
