#include "cmd-obtener-restaurante.h"

void cmd_obtener_restaurante_exec(t_command* comando){

	if(!cmd_obtener_restaurante_validate(comando)){
		char* uso = cmd_obtener_restaurante_uso();
		printf("Los parametros ingresados no son validos.\n");
		printf("Uso: %s", uso);
		free(uso);
		return;
	}

	char* ip = strdup(config_get_string_value(config, "IP_SERVER"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_SERVER"));

	int socket_cliente = crear_conexion(ip, puerto);
	char* nombre_restaurante = list_get(comando->args, 0);

	t_obtener_restaurante_req* request = obtener_restaurante_req_create(nombre_restaurante);

	char* request_str = obtener_restaurante_req_to_string(request);
	log_info(logger, "Se va enviar el mensaje: %s" ,request_str);

	t_paquete* paquete_req = obtener_restaurante_req_empaquetar(request);

	if(enviar_mensaje(socket_cliente, paquete_req) < 0){
		free(ip);
		free(puerto);
		obtener_restaurante_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_cliente);
		return;
	}

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion != OBTENER_RESTAURANTE){
		log_error(logger, "Ocurrio un error al recibir el paquete: %s", strerror(errno));
		printf("El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);

		free(ip);
		free(puerto);
		obtener_restaurante_req_destroy(request);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_cliente);
		return;
	}

	t_obtener_restaurante_res* response = obtener_restaurante_res_desempaquetar(paquete_res);

	char* response_str = obtener_restaurante_res_to_string(response);
	log_info(logger, "Se recibio el mensaje: %s" ,response_str);

	char* mensaje_str = string_new();
	string_append_with_format(&mensaje_str,"Afinidad: ");
	for(int i =0;i<response->afinidad->elements_count;i++){
		string_append_with_format(&mensaje_str,(char*)list_get(response->afinidad,i),"\n");
	}
	string_append(&mensaje_str,"Cantidad de hornos: ");
	string_append(&mensaje_str,(char*)response->cant_hornos);
	string_append(&mensaje_str,"Cantidad de pedidos: ");
	string_append(&mensaje_str,(char*)response->cant_pedidos);
	string_append(&mensaje_str,"Pos x: ");
	string_append(&mensaje_str,(char*)response->pos_x);
	string_append(&mensaje_str,"Pos y: ");
	string_append(&mensaje_str,(char*)response->pos_y);
	string_append(&mensaje_str,"Receta y precio: ");

	for(int i =0;i<response->receta_precio->elements_count;i++){
		string_append(&mensaje_str,(char*)list_get(response->receta_precio,i));
	}

	printf("Mensaje Recibido: %s \n", mensaje_str);

	free(ip);
	free(puerto);
	obtener_restaurante_req_destroy(request);
	free(response_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	obtener_restaurante_res_destroy(response);
	free(request_str);
	free(mensaje_str);

	liberar_conexion(socket_cliente);

}

bool cmd_obtener_restaurante_validate(t_command* comando){
	//TODO: validdo que comando->cmd sea igual a cmd_obtener_restaurante
	//TODO: valido los argumentos comando->args
	return true;
}

char* cmd_obtener_restaurante_uso(){
	return string_duplicate("");
}
