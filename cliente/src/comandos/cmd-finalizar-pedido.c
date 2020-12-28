#include "cmd-finalizar-pedido.h"

void cmd_finalizar_pedido_exec(t_command* comando){

	if(!cmd_finalizar_pedido_validate(comando)){
		char* uso = cmd_finalizar_pedido_uso();
		printf("Los parametros ingresados no son validos.\n");
		printf("Uso: %s", uso);
		free(uso);
		return;
	}

	char* ip = strdup(config_get_string_value(config, "IP_SERVER"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_SERVER"));

	int socket_cliente = crear_conexion(ip, puerto);
	char* nombre_restaurante = list_get(comando->args, 0);
	char* id_pedido= list_get(comando->args, 1);

	t_finalizar_pedido_req* req = finalizar_pedido_req_create(id_pedido,nombre_restaurante);


	char* request_str = finalizar_pedido_req_to_string(req);
	log_info(logger, "Se va a enviar el mensaje: %s", request_str);

	t_paquete* paquete_req = finalizar_pedido_req_empaquetar(req);

	if(enviar_mensaje(socket_cliente,  paquete_req) < 0){
		log_error(logger, "Ocurrio un error al intentar enviar el paquete: %s", strerror(errno));
		printf("Ocurrio un error al intentar enviar el paquete.\n");

		free(ip);
		free(puerto);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_cliente);
		return;
	}

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion != FINALIZAR_PEDIDO){
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

	t_finalizar_pedido_res* response = finalizar_pedido_res_desempaquetar(paquete_res);

	char* response_str = finalizar_pedido_res_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s", response_str);

	char* mensaje_str = string_new();
	string_append_with_format(&mensaje_str, "Restaurante - %s \n", nombre_restaurante);
	string_append_with_format(&mensaje_str, "idPedido - %s \n", id_pedido);
	string_append_with_format(&mensaje_str, "Terminado: %s \n",response_str);

	printf("%s \n", mensaje_str);

	free(ip);
	free(puerto);
	free(request_str);
	free(response_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	finalizar_pedido_res_destroy(response);
	finalizar_pedido_req_destroy(req);
	free(mensaje_str);

	liberar_conexion(socket_cliente);



}

bool cmd_finalizar_pedido_validate(t_command* comando){
	//TODO: validdo que comando->cmd sea igual a cmd_finalizar_pedido
	//TODO: valido los argumentos comando->args
	return true;
}

char* cmd_finalizar_pedido_uso(){
	return string_duplicate("");
}
