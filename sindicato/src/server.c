#include "server.h"

static void procesar_mensajes(int *socket);
void procesar_obtener_restaurante(int socket_cliente, t_paquete *paquete);
void procesar_obtener_receta(int socket_cliente, t_paquete *paquete);
void procesar_consultar_platos(int socket_cliente, t_paquete *paquete);
void procesar_guardar_pedido(int socket_cliente, t_paquete *paquete);
void procesar_guardar_plato(int socket_cliente, t_paquete *paquete);
void procesar_plato_listo(int socket_cliente, t_paquete *paquete);
void procesar_confirmar_pedido(int socket_cliente, t_paquete *paquete);
void procesar_obtener_pedido(int socket_cliente, t_paquete *paquete);
void procesar_terminar_pedido(int socket_cliente, t_paquete *paquete);

void* exec_servidor(void *ptr) {
	int socket_servidor = 0;
	log_debug(logger, "Iniciando servidor");

	char *puerto = config_get_string_value(config, CONFIG_KEY_PUERTO);
	char *ip_escucha = config_get_string_value(config, CONFIG_KEY_IP);

	log_debug(logger, "Servidor puerto: %s", puerto);

	socket_servidor = iniciar_servidor(ip_escucha, puerto);
	if(socket_servidor < 1){
		log_error(logger, "No se puede iniciar el servidor sindicato");
		exit_failure();
	}
	log_debug(logger, "Escuchando en el puerto: %s", puerto);
	free(puerto);
	free(ip_escucha);
	while (1) {
		log_debug(logger, "Esperando conexion cliente");
		int socket_cliente = esperar_cliente_(socket_servidor);

		pthread_create(&thread, NULL, (void*) procesar_mensajes, &socket_cliente);
		pthread_detach(thread);
	}

	log_info(logger, "Saliendo del servidor");

	return NULL;
}

static void procesar_mensajes(int *socket) {
	int socket_cliente = *socket;
	t_paquete *paquete = recibir_mensaje(socket_cliente);

	switch (paquete->codigo_operacion) {
	case OBTENER_RESTAURANTE:
		procesar_obtener_restaurante(socket_cliente, paquete);
		break;
	case OBTENER_RECETA:
		procesar_obtener_receta(socket_cliente, paquete);
		break;
	case GUARDAR_PLATO:
		procesar_guardar_plato(socket_cliente, paquete);
		break;
	case GUARDAR_PEDIDO:
		procesar_guardar_pedido(socket_cliente, paquete);
		break;
	case CONSULTAR_PLATOS:
		procesar_consultar_platos(socket_cliente, paquete);
		break;
	case PLATO_LISTO:
		procesar_plato_listo(socket_cliente, paquete);
		break;
	case CONFIRMAR_PEDIDO:
		procesar_confirmar_pedido(socket_cliente, paquete);
		break;
	case OBTENER_PEDIDO:
		procesar_obtener_pedido(socket_cliente, paquete);
		break;
	case TERMINAR_PEDIDO:
		procesar_terminar_pedido(socket_cliente, paquete);
		break;
	case ERROR:
		log_error(logger, "Ocurrio un error al recibir el mensaje.");
		paquete_destroy(paquete);
		liberar_conexion(socket_cliente);
		pthread_exit(NULL);
		break;
	default:
		;
		log_error(logger, "El tipo de mensaje recibido es incorrecto.");
		paquete_destroy(paquete);
		liberar_conexion(socket_cliente);
		pthread_exit(NULL);
		break;
	}
	liberar_conexion(socket_cliente);
}

void procesar_obtener_restaurante(int socket_cliente, t_paquete *paquete) {

	// deserealizo el mensaje
	t_obtener_restaurante_req *request = obtener_restaurante_req_desempaquetar(paquete);
	char *msg = obtener_restaurante_req_to_string(request);
	log_info(logger, "Se recibió el mensaje de %d: %s", socket_cliente, msg);

	char *data_restaurante = afip_obtener_restaurante(request->restaurante);
	uint32_t cantidad_pedidos = afip_obtener_cantidad_pedidos(request->restaurante);

	t_paquete *respuesta = obtener_restaurante_res_data_empaquetar(data_restaurante, cantidad_pedidos);
	enviar_mensaje(socket_cliente, respuesta);

	paquete_destroy(paquete);
	obtener_restaurante_req_destroy(request);
	free(msg);
	paquete_destroy(respuesta);
}

void procesar_obtener_receta(int socket_cliente, t_paquete *paquete) {
	t_obtener_receta_req *request = obtener_receta_req_desempaquetar(paquete);
	char *msg = obtener_receta_req_to_string(request);
	log_info(logger, "Se recibió el mensaje de %d: %s", socket_cliente, msg);

	char *data_receta = afip_obtener_receta(request->receta);
	t_paquete *respuesta = obtener_receta_resp_data_empaquetar(data_receta);
	enviar_mensaje(socket_cliente, respuesta);

	paquete_destroy(paquete);
	obtener_receta_req_destroy(request);
	free(msg);
	paquete_destroy(respuesta);
}

void procesar_consultar_platos(int socket_cliente, t_paquete *paquete) {
	t_consultar_platos_req *request = consultar_platos_req_desempaquetar(paquete);
	char *msg = consultar_platos_req_to_string(request);
	log_info(logger, "Se recibió el mensaje de %d: %s", socket_cliente, msg);
	char *data_platos = afip_obtener_restaurante(request->restaurante);
	t_paquete *respuesta = consutar_platos_res_data_empaquetar(data_platos);
	enviar_mensaje(socket_cliente, respuesta);

	paquete_destroy(paquete);
	consultar_platos_req_destroy(request);
	free(msg);
	paquete_destroy(respuesta);
}

void procesar_guardar_pedido(int socket_cliente, t_paquete *paquete) {
	t_guardar_pedido_req *request = guardar_pedido_req_desempaquetar(paquete);
	char *msg = guardar_pedido_req_to_string(request);
	log_info(logger, "Se recibió el mensaje de %d: %s", socket_cliente, msg);
	status_code resultado = afip_guardar_pedido(request->restaurante, request->id_pedido);
	t_guardar_pedido_res *guardar_pedido_res = guardar_pedido_res_create(resultado);
	char *msg_salida = guardar_pedido_res_to_string(guardar_pedido_res);
	log_info(logger, "Envio el mensaje %d: %s", socket_cliente, msg_salida);
	t_paquete *respuesta = guardar_pedido_res_empaquetar(guardar_pedido_res);
	enviar_mensaje(socket_cliente, respuesta);

	paquete_destroy(paquete);
	guardar_pedido_req_destroy(request);
	free(msg);
	free(msg_salida);
	guardar_pedido_res_destroy(guardar_pedido_res);
	paquete_destroy(respuesta);
}

void procesar_guardar_plato(int socket_cliente, t_paquete *paquete) {
	t_guardar_plato_req *request = guardar_plato_req_desempaquetar(paquete);
	char *msg = guardar_plato_req_to_string(request);
	log_info(logger, "Se recibió el mensaje de %d: %s", socket_cliente, msg);
	status_code resultado = afip_guardar_plato(request->restaurante, request->id_pedido, request->comida, request->cantidad);
	t_guardar_plato_res *guardar_res = guardar_plato_res_create(resultado);
	log_info_message("Enviando", guardar_plato_res_to_string(guardar_res));
	t_paquete *respuesta = guardar_plato_res_empaquetar(guardar_res);
	enviar_mensaje(socket_cliente, respuesta);

	paquete_destroy(paquete);
	guardar_plato_req_destroy(request);
	free(msg);
	guardar_plato_res_destroy(guardar_res);
	paquete_destroy(respuesta);
}

void procesar_plato_listo(int socket_cliente, t_paquete *paquete) {
	t_plato_listo_req *request = plato_listo_req_desempaquetar(paquete);
	char *msg = plato_listo_req_to_string(request);
	log_info(logger, "Se recibió el mensaje de %d: %s", socket_cliente, msg);
	status_code resultado = afip_plato_listo(request->restaurante, request->id_pedido, request->comida);
	t_plato_listo_res *res = plato_listo_res_create(resultado);

	log_info_message("Enviando", plato_listo_res_to_string(res));

	t_paquete *respuesta = plato_listo_res_empaquetar(res);
	enviar_mensaje(socket_cliente, respuesta);

	paquete_destroy(paquete);
	plato_listo_req_destroy(request);
	free(msg);
	plato_listo_res_destroy(res);
	paquete_destroy(respuesta);
}

void procesar_confirmar_pedido(int socket_cliente, t_paquete *paquete) {
	t_confirmar_pedido_req *request = confirmar_pedido_req_desempaquetar(paquete);
	char *msg = confirmar_pedido_req_to_string(request);
	log_info(logger, "Se recibió el mensaje de %d: %s", socket_cliente, msg);

	status_code resultado = afip_cambiar_estado_pedido(request->restaurante, request->id_pedido, Pendiente, Confirmado,
	STR_ESTADO_CONFIRMADO);
	t_confirmar_pedido_res *res = confirmar_pedido_res_create(resultado);
	log_info_message("Enviando", confirmar_pedido_res_to_string(res));
	t_paquete *respuesta = confirmar_pedido_res_empaquetar(res);
	enviar_mensaje(socket_cliente, respuesta);

	paquete_destroy(paquete);
	confirmar_pedido_req_destroy(request);
	free(msg);
	confirmar_pedido_res_destroy(res);
	paquete_destroy(respuesta);

}

void procesar_obtener_pedido(int socket_cliente, t_paquete *paquete) {
	t_obtener_pedido_req *request = obtener_pedido_req_desempaquetar(paquete);
	char *msg = obtener_pedido_req_to_string(request);
	log_info(logger, "Se recibió el mensaje de %d: %s", socket_cliente, msg);

	t_obtener_pedido_res *res = afip_obtener_pedido(request->restaurante, request->id_pedido);
	log_info_message("Enviando", obtener_pedido_res_to_string(res));
	t_paquete *respuesta = obtener_pedido_res_empaquetar(res);

	enviar_mensaje(socket_cliente, respuesta);

	paquete_destroy(paquete);
	obtener_pedido_req_destroy(request);
	free(msg);
	obtener_pedido_res_destroy(res);
	paquete_destroy(respuesta);

}

void procesar_terminar_pedido(int socket_cliente, t_paquete *paquete) {
	t_terminar_pedido_req *request = terminar_pedido_req_desempaquetar(paquete);
	char *msg = terminar_pedido_req_to_string(request);
	log_info(logger, "Se recibió el mensaje de %d: %s", socket_cliente, msg);

	status_code resultado = afip_cambiar_estado_pedido(request->restaurante, request->id_pedido, Confirmado, Terminado,
	STR_ESTADO_TERMINADO);
	t_terminar_pedido_res *res = terminar_pedido_res_create(resultado);
	log_info_message("Enviando", terminar_pedido_res_to_string(res));
	t_paquete *respuesta = terminar_pedido_res_empaquetar(res);
	enviar_mensaje(socket_cliente, respuesta);

	paquete_destroy(paquete);
	terminar_pedido_req_destroy(request);
	free(msg);
	terminar_pedido_res_destroy(res);
	paquete_destroy(respuesta);

}
