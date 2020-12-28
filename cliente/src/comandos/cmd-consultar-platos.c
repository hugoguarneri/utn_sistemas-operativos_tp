#include "cmd-consultar-platos.h"

void cmd_consultar_platos_exec(t_command* comando){

	if(!cmd_consultar_platos_validate(comando)){
		char* uso = cmd_consultar_platos_uso();
		printf("Los parametros ingresados no son validos.\n");
		printf("Uso: %s", uso);
		free(uso);
		return;
	}

	char* ip = strdup(config_get_string_value(config, "IP_SERVER"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_SERVER"));

	int socket_cliente = crear_conexion(ip, puerto);

	char* restaurante = list_get(comando->args, 0);

	t_consultar_platos_req* request = consultar_platos_req_create(restaurante);

	char* request_str = consultar_platos_req_to_string(request);
	log_info(logger, "Se va consultar sobre el restaurante: %s" ,request_str);

	t_paquete* paquete_req = consultar_platos_req_empaquetar(request);

	if(enviar_mensaje(socket_cliente, paquete_req) < 0){
		free(ip);
		free(puerto);
		consultar_platos_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_cliente);
		return;
	}

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion != CONSULTAR_PLATOS){
		log_error(logger, "Ocurrio un error al recibir el paquete: %s", strerror(errno));
		printf("El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip);
		free(puerto);
		consultar_platos_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_cliente);
		return;
	}

	t_consultar_platos_res* response = consultar_platos_res_desempaquetar(paquete_res);

	char* response_str = consultar_platos_res_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s" ,response_str);

	char* mensaje_str = string_new();
	for(int i = 0; i < response->platos->elements_count; i++){
		char* plato = list_get(response->platos, i);
		string_append_with_format(&mensaje_str, "   - %s \n" ,plato);
	}

	printf("%s \n", mensaje_str);

	free(ip);
	free(puerto);
	consultar_platos_req_destroy(request);
	free(response_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	consultar_platos_res_destroy(response);
	free(request_str);
	free(mensaje_str);

	liberar_conexion(socket_cliente);

}

bool cmd_consultar_platos_validate(t_command* comando){
	//TODO: validdo que comando->cmd sea igual a cmd_consultar_platos
	//TODO: valido los argumentos comando->args
	return true;
}

char* cmd_consultar_platos_uso(){
	return string_duplicate("ConsultarPlatos [RESTAURANTE]");
}
