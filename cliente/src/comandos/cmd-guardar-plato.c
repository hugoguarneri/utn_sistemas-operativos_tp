#include "cmd-guardar-plato.h"

void cmd_guardar_plato_exec(t_command* comando){

	if(!cmd_guardar_plato_validate(comando)){
		char* uso = cmd_guardar_plato_uso();
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
	char* comida= list_get(comando->args, 2);
	uint32_t cantidad = (uint32_t) atoi(list_get(comando->args, 3));
	t_guardar_plato_req* request = guardar_plato_req_create(nombre_restaurante,id_pedido,comida,cantidad);

	char* request_str = guardar_plato_req_to_string(request);
	log_info(logger, "Se va enviar el mensaje: %s" ,request_str);

	t_paquete* paquete_req = guardar_plato_req_empaquetar(request);

	if(enviar_mensaje(socket_cliente, paquete_req) < 0){
		free(ip);
		free(puerto);
		guardar_plato_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_cliente);
		return;
	}

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion != GUARDAR_PLATO){
		log_error(logger, "Ocurrio un error al recibir el paquete: %s", strerror(errno));
		printf("El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip);
		free(puerto);
		guardar_plato_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_cliente);
		return;
	}

	t_guardar_plato_res* response = guardar_plato_res_desempaquetar(paquete_res);

	char* response_str = guardar_plato_res_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s" ,response_str);

	char* mensaje_str = string_new();
	string_append_with_format(&mensaje_str,response_str);
	printf("Mensaje Recibido: %s \n", mensaje_str);

	free(ip);
	free(puerto);
	guardar_plato_req_destroy(request);
	free(response_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	guardar_plato_res_destroy(response);
	free(request_str);
	free(mensaje_str);

	liberar_conexion(socket_cliente);


}

bool cmd_guardar_plato_validate(t_command* comando){
	//TODO: validdo que comando->cmd sea igual a cmd_guardar_plato
	//TODO: valido los argumentos comando->args
	return true;
}

char* cmd_guardar_plato_uso(){
	return string_duplicate("");
}
