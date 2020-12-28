#include "handshake-app.h"

void handshake_app(t_obtener_restaurante_res* t_obtener_restaurante_res, char* restaurante_name){

	char* ip_app = strdup(config_get_string_value(config, "IP_APP"));
	char* puerto_app = strdup(config_get_string_value(config, "PUERTO_APP"));
	int socket_app = crear_conexion(ip_app, puerto_app);

	t_info_restaurante* info_restaurante = info_restaurante_create(strdup(config_get_string_value(config, "IP_ESCUCHA")),
																   strdup(config_get_string_value(config, "PUERTO_ESCUCHA")),
																   strdup(config_get_string_value(config, "NOMBRE_RESTAURANTE")),
																   t_obtener_restaurante_res->pos_x,
																   t_obtener_restaurante_res->pos_y);

	t_handshake_restaurante_app_req* req = handshake_restaurante_app_req_create(info_restaurante);
	t_paquete* paquete_req = handshake_restaurante_app_req_empaquetar(req);

	log_info(logger, "Enviando handshake a la app del restaurante: %s", restaurante_name);

	if (enviar_mensaje(socket_app, paquete_req) < 0) {
		log_error(logger, "Ocurrio un error al intentar enviar el paquete: %s", strerror(errno));
		printf("Ocurrio un error al intentar enviar el paquete.\n");
	}

	free(ip_app);
	free(puerto_app);
	paquete_destroy(paquete_req);
	liberar_conexion(socket_app);
	info_restaurante_destroy(info_restaurante);
	handshake_restaurante_app_req_destroy(req);

	return;
}
