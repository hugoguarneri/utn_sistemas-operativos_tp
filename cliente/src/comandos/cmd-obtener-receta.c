#include "cmd-obtener-receta.h"

void cmd_obtener_receta_exec(t_command* comando){

	if(!cmd_obtener_receta_validate(comando)){
		char* uso = cmd_obtener_receta_uso();
		printf("Los parametros ingresados no son validos.\n");
		printf("Uso: %s", uso);
		free(uso);
		return;
	}

	char* ip = strdup(config_get_string_value(config, "IP_SERVER"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_SERVER"));

	int socket_cliente = crear_conexion(ip, puerto);
	char* nombre_plato = list_get(comando->args, 0);

	t_obtener_receta_req* request = obtener_receta_req_create(nombre_plato);

	char* request_str = obtener_receta_req_to_string(request);
	log_info(logger, "Se va enviar el mensaje: %s" ,request_str);

	t_paquete* paquete_req = obtener_receta_req_empaquetar(request);

	if(enviar_mensaje(socket_cliente, paquete_req) < 0){
		free(ip);
		free(puerto);
		obtener_receta_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_cliente);
		return;
	}

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion != OBTENER_RECETA){
		log_error(logger, "Ocurrio un error al recibir el paquete: %s", strerror(errno));
		printf("El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip);
		free(puerto);
		obtener_receta_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_cliente);
		return;
	}

	t_obtener_receta_resp* response = obtener_receta_resp_desempaquetar(paquete_res);

	char* response_str = obtener_receta_resp_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s" ,response_str);

	char* mensaje_str = string_new();
	for(int i =0;i<response->pasos_tiempo->elements_count;i++){
		string_append_with_format(&mensaje_str,(char*)list_get(response->pasos_tiempo,i));
	}
	printf("Mensaje Recibido: %s \n", mensaje_str);

	free(ip);
	free(puerto);
	obtener_receta_req_destroy(request);
	free(response_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	obtener_receta_resp_destroy(response);
	free(request_str);
	free(mensaje_str);

	liberar_conexion(socket_cliente);

}

bool cmd_obtener_receta_validate(t_command* comando){
	//TODO: validdo que comando->cmd sea igual a cmd_obtener_receta
	//TODO: valido los argumentos comando->args
	return true;
}

char* cmd_obtener_receta_uso(){
	return string_duplicate("");
}
