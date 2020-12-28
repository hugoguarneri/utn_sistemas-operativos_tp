#include "cmd-consultar-pedido.h"

void cmd_consultar_pedido_exec(t_command* comando){

	if(!cmd_consultar_pedido_validate(comando)){
		char* uso = cmd_consultar_pedido_uso();
		printf("Los parametros ingresados no son validos.\n");
		printf("Uso: %s", uso);
		free(uso);
		return;
	}

	char* ip = strdup(config_get_string_value(config, "IP_SERVER"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_SERVER"));

	int socket_cliente = crear_conexion(ip, puerto);

	char* id_pedido = list_get(comando->args, 0);
	t_consultar_pedido_req* request = consultar_pedido_req_create(id_pedido);

	char* request_str = consultar_pedido_req_to_string(request);
	log_info(logger, "Se va a enviar el mensaje: %s", request_str);

	t_paquete* paquete_req = consultar_pedido_req_empaquetar(request);

	if(enviar_mensaje(socket_cliente, paquete_req) < 0){
		free(ip);
		free(puerto);
		consultar_pedido_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_cliente);
		return;
	}

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion != CONSULTAR_PEDIDO){
		log_error(logger, "Ocurrio un error al recibir el paquete: %s", strerror(errno));
		printf("El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip);
		free(puerto);
		consultar_pedido_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_cliente);
		return;
	}

	t_consultar_pedido_res* response = consultar_pedido_res_desempaquetar(paquete_res);

	char* response_str = consultar_pedido_res_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s", response_str);

	char* mensaje_str = string_new();
	string_append_with_format(&mensaje_str, "> Restaurate: %s, Estado: %s \n", response->restaurante, response->estado == 0 ? "PENDIENTE" : "CONFIRMADO");
	string_append_with_format(&mensaje_str, "> Estado de los platos: \n");

	for(int i = 0; i < response->estado_platos->elements_count; i++){
		t_estado_plato* estado = list_get(response->estado_platos, i);
		string_append_with_format(&mensaje_str, ">   - Comida: %s, Cant total: %d , Cant lista: %d \n", estado->comida, estado->cant_total, estado->cant_lista);
	}

	printf("%s \n", mensaje_str);

	free(ip);
	free(puerto);
	//free(id_pedido);
	consultar_pedido_req_destroy(request);
	free(request_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	consultar_pedido_res_destroy(response);
	free(response_str);
	free(mensaje_str);

	liberar_conexion(socket_cliente);

}

bool cmd_consultar_pedido_validate(t_command* comando){
	//TODO: validdo que comando->cmd sea igual a cmd_consultar_pedido
	//TODO: valido los argumentos comando->args
	return true;
}

char* cmd_consultar_pedido_uso(){
	return string_duplicate("");
}
