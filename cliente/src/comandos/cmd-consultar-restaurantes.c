#include "cmd-consultar-restaurantes.h"

void cmd_consultar_restaurantes_exec(t_command* comando){

	if(!cmd_consultar_restaurantes_validate(comando)){
		char* uso = cmd_consultar_restaurantes_uso();
		printf("Los parametros ingresados no son validos.\n");
		printf("Uso: %s", uso);
		free(uso);
		return;
	}

	char* ip_server = strdup(config_get_string_value(config, "IP_SERVER"));
	char* puerto_server = strdup(config_get_string_value(config, "PUERTO_SERVER"));

	int socket_cliente = crear_conexion(ip_server, puerto_server);

	char* request_str = consultar_restaurantes_req_to_string();
	log_info(logger, "Se va a enviar el mensaje: %s", request_str);

	t_paquete* paquete_req = consultar_restaurantes_req_empaquetar();

	if(enviar_mensaje(socket_cliente,  paquete_req) < 0){
		log_error(logger, "Ocurrio un error al intentar enviar el paquete: %s", strerror(errno));
		printf("Ocurrio un error al intentar enviar el paquete.\n");

		free(ip_server);
		free(puerto_server);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_cliente);
		return;
	}

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion != CONSULTAR_RESTAURANTES){
		log_error(logger, "Ocurrio un error al recibir el paquete: %s", strerror(errno));
		printf("El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip_server);
		free(puerto_server);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_cliente);
		return;
	}

	t_consultar_restaurantes_res* response = consultar_restaurantes_res_desempaquetar(paquete_res);

	char* response_str = consultar_restaurantes_res_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s", response_str);

	char* mensaje_str = string_duplicate("> Restaurantes: \n");
	for(int i = 0; i < response->nombres->elements_count; i++){
		char* nombre = list_get(response->nombres, i);
		string_append_with_format(&mensaje_str, "   - %s \n", nombre);
	}

	printf("%s \n", mensaje_str);

	free(ip_server);
	free(puerto_server);
	free(request_str);
	free(response_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	consultar_restaurantes_res_destroy(response);
	free(mensaje_str);

	liberar_conexion(socket_cliente);
}

bool cmd_consultar_restaurantes_validate(t_command* comando){
	//TODO: validdo que comando->cmd sea igual a CMD_CONSULTAR_RESTAURANTES
	//TODO: valido los argumentos comando->args
	return true;
}


char* cmd_consultar_restaurantes_uso(){
	return string_duplicate("ConsultarRestaurantes");
}
