#include "handshake-sindicato.h"

t_obtener_restaurante_res* obtener_restaurante(char* restaurante_name) {

	char* ip_sindicato = strdup(config_get_string_value(config, "IP_SINDICATO"));
	char* puerto_sindicato = strdup(config_get_string_value(config, "PUERTO_SINDICATO"));

	int socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);

	t_obtener_restaurante_req* req = obtener_restaurante_req_create(restaurante_name);
	t_paquete* paquete_req = obtener_restaurante_req_empaquetar(req);

	log_info(logger, "Enviando el mensaje OBTENER_RESTAURANTE del restaurante: %s", restaurante_name);
	if (enviar_mensaje(socket_sindicato, paquete_req) < 0) {

		log_error(logger, "Ocurrio un error al intentar enviar el paquete: %s", strerror(errno));
		printf("Ocurrio un error al intentar enviar el paquete.\n");

		free(ip_sindicato);
		free(puerto_sindicato);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_sindicato);
		//todo: porque no anda?
		//obtener_restaurante_req_destroy(req);

		return NULL;
	}

	log_info(logger, "Recibiendo respuesta del mensaje OBTENER_RESTAURANTE del restaurante: %s", restaurante_name);
	t_paquete* paquete_res = recibir_mensaje(socket_sindicato);

	if (paquete_res->codigo_operacion != OBTENER_RESTAURANTE) {

		log_error(logger, "Ocurrió un error al intentar recibir el paquete: %s", strerror(errno));
		printf("Ocurrio un error al intentar recibir el paquete. [%d]\n", paquete_res->codigo_operacion);

		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		//todo: porque no anda?
		//obtener_restaurante_req_destroy(req);
		free(ip_sindicato);
		free(puerto_sindicato);
		liberar_conexion(socket_sindicato);

		return NULL;
	}

	t_obtener_restaurante_res* response = obtener_restaurante_res_desempaquetar(paquete_res);

	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	//todo: porque no anda?
	//obtener_restaurante_req_destroy(req);
	free(ip_sindicato);
	free(puerto_sindicato);
	liberar_conexion(socket_sindicato);

	return response;
}
