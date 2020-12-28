#include "servidor-comanda.h"
#include "core.h"
#include "restaurantes-comanda.h"

//void procesar_handshake_restaurante_app(int socket_cliente, t_paquete* paquete_req);
static void procesar_mensajes(int *socket);

void procesar_finalizar_pedido(int socket_cliente, t_paquete *paquete_req);
void procesar_plato_listo(int socket_cliente, t_paquete *paquete_req);
void procesar_confirmar_pedido(int socket_cliente, t_paquete *paquete_req);
void procesar_obtener_pedido(int socket_cliente, t_paquete *paquete_req);
void procesar_guardar_plato(int socket_cliente, t_paquete *paquete_req);
void procesar_guardar_pedido(int socket_cliente, t_paquete *paquete_req);
void procesar_handshake(int socket_cliente, t_paquete *paquete_req);

void exec_servidor_comanda() {

	int socket_servidor = iniciar_servidor(configuracion->ip_escucha, configuracion->puerto);
	log_info(logger, "Iniciando servidor comanda - %s:%s", configuracion->ip_escucha, configuracion->puerto);

	while (1) {
		log_info(logger, "Esperando nueva conexion..");
		int socket_cliente = esperar_cliente_(socket_servidor);

		pthread_create(&thread, NULL, (void*) procesar_mensajes, &socket_cliente);
		pthread_detach(thread);
	}
}

static void procesar_mensajes(int *socket) {
	int socket_cliente = *socket;

	t_paquete *paquete = recibir_mensaje(socket_cliente);

	switch (paquete->codigo_operacion) {
	case HANDSHAKE:
		procesar_handshake(socket_cliente, paquete);
		break;
	case GUARDAR_PEDIDO:
		procesar_guardar_pedido(socket_cliente, paquete);
		break;
	case OBTENER_PEDIDO:
		procesar_obtener_pedido(socket_cliente, paquete);
		break;
	case GUARDAR_PLATO:
		procesar_guardar_plato(socket_cliente, paquete);
		break;
	case CONFIRMAR_PEDIDO:
		procesar_confirmar_pedido(socket_cliente, paquete);
		break;
	case PLATO_LISTO:
		procesar_plato_listo(socket_cliente, paquete);
		break;
	case FINALIZAR_PEDIDO:
		procesar_finalizar_pedido(socket_cliente, paquete);
		break;
	case ERROR:
		log_error(logger, "Ocurrio un error al recibir el mensaje.");
		paquete_destroy(paquete);
		break;
	default:
		log_error(logger, "El tipo de mensaje recibido es incorrecto.");
		paquete_destroy(paquete);
		break;
	}
	liberar_conexion(socket_cliente);
}

void procesar_finalizar_pedido(int socket_cliente, t_paquete *paquete_req) {
	/*
	 * Este mensaje cumplirá la función de eliminar de la memoria los datos asociados al pedido. Para esto recibirá:
	 * 1. El nombre del Restaurante.
	 * 2. El ID de Pedido.
	 * Al recibir esta información se realizarán las siguientes operaciones:
	 * 1. Verificar si existe la tabla de segmentos de dicho Restaurante. En caso de no existir se deberá informar dicha situación.
	 * 2. Verificar que exista el segmento de dicho pedido dentro de la tabla de segmentos del Restaurante. En caso de no existir se deberá informar dicha situación.
	 * 3. Se deberá proceder a eliminar las páginas correspondientes a dicho segmento.
	 * 4. Por último, se procederá a eliminar el segmento correspondiente.
	 * 5. Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	 */
	t_finalizar_pedido_res *result = NULL;
	t_paquete *paquete_res = NULL;
	t_finalizar_pedido_req *request = finalizar_pedido_req_desempaquetar(paquete_req);
	finalizar_pedido_req_to_log("Recibi", request);

	if (restaurante_dictionary_has_key(request->restaurante)) {
		t_segmentos *segmentos = restaurante_dictionary_get(request->restaurante);
		// Obtengo el segmento del pedido
		t_nodo_segmento *segmento = segmentos_get_by_id(segmentos, atoi(request->id_pedido));
		if (segmento != NULL && segmento->estado == CONFIRMADO) {
			log_info(logger, "Existe segmento para pedido %s, lo finalizo", request->id_pedido);

			// Elimino el segmento de la lista
			segmentos_remove_reference(segmentos, atoi(request->id_pedido));

			// Elimino las paginas del segmento en memoria
			paginas_clear_memory(segmento->paginas);

			segmento_destroy(segmento);

			result = finalizar_pedido_res_create(OK_);
			finalizar_pedido_res_to_log("Envio", result);
			paquete_res = finalizar_pedido_res_empaquetar(result);
		} else {
			log_warning(logger, "No existe segmento para pedido %s o no esta pendiente", request->id_pedido);
			result = finalizar_pedido_res_create(FAIL_);
			finalizar_pedido_res_to_log("Envio", result);
			paquete_res = finalizar_pedido_res_empaquetar(result);
		}
	} else {
		// En caso de no existir se deberá informar dicha situación.
		log_warning(logger, "No existe la tabla de segmentos para %s", request->restaurante);
		result = finalizar_pedido_res_create(FAIL_);
		finalizar_pedido_res_to_log("Envio", result);
		paquete_res = finalizar_pedido_res_empaquetar(result);
	}
	log_info_message(MSG_INFO_ENVIO, finalizar_pedido_res_to_string(result));
	enviar_mensaje(socket_cliente, paquete_res);

	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	finalizar_pedido_req_destroy(request);
	finalizar_pedido_res_destroy(result);
}

void procesar_plato_listo(int socket_cliente, t_paquete *paquete_req) {
	/*
	 * Este mensaje cumplirá la función de aumentar en uno la cantidad de platos listos para un determinado pedido de un determinado Restaurante. Para esto recibirá:
	 * 1. El nombre del Restaurante.
	 * 2. El ID de Pedido.
	 * 3. El plato que está listo.
	 * Al recibir esta información se realizarán las siguientes operaciones:
	 * 1. Verificar si existe la tabla de segmentos de dicho Restaurante. En caso de no existir se deberá informar dicha situación.
	 * 2. Verificar que exista el segmento de dicho pedido dentro de la tabla de segmentos del Restaurante. En caso de no existir se deberá informar dicha situación.
	 * 3. Verificar si existe el plato dentro de la tabla de páginas del pedido. En caso contrario, se deberá informar dicha situación.
	 * 4. En caso de que las páginas que corresponden al pedido no se encuentren cargadas en memoria principal, se deberán cargar las mismas desde swap (iniciando la elección de víctimas de ser necesario). En caso contrario, proceder al paso  número 4.
	 * 5. Verificar que el pedido esté en estado “Confirmado”. En caso contrario se deberá informar dicha situación.
	 * 6. Se deberá aumentar en uno la cantidad lista de ese plato. En caso de que todos los platos estén listos, se deberá cambiar el estado del pedido a “Terminado”.
	 * 7. Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	 */
	t_plato_listo_res *result = NULL;
	t_paquete *paquete_res = NULL;
	t_plato_listo_req *request = plato_listo_req_desempaquetar(paquete_req);
	log_info_message(MSG_INFO_RECIBO, plato_listo_req_to_string(request));

	if (restaurante_dictionary_has_key(request->restaurante)) {
		t_segmentos *segmentos = restaurante_dictionary_get(request->restaurante);
		// Obtengo el segmento del pedido
		t_nodo_segmento *segmento = segmentos_get_by_id(segmentos, atoi(request->id_pedido));
		if (segmento != NULL && segmento->estado == CONFIRMADO) {
			t_nodo_pagina *pagina = platos_buscar_plato(segmento->paginas, request->comida);
			if (pagina != NULL) {
				platos_agregar_cantidad_lista(pagina);
				result = plato_listo_res_create(OK_);
				paquete_res = plato_listo_res_empaquetar(result);
			} else {
				log_warning(logger, "No existe la pagina del plato %s", request->comida);
				result = plato_listo_res_create(FAIL_);
				paquete_res = plato_listo_res_empaquetar(result);
			}
		} else {
			log_warning(logger, "No existe segmento para pedido %s o esta pendiente", request->id_pedido);
			result = plato_listo_res_create(FAIL_);
			paquete_res = plato_listo_res_empaquetar(result);
		}
	} else {
		// En caso de no existir se deberá informar dicha situación.
		log_warning(logger, "No existe la tabla de segmentos para %s", request->restaurante);
		result = plato_listo_res_create(FAIL_);
		paquete_res = plato_listo_res_empaquetar(result);
	}

	log_info_message(MSG_INFO_ENVIO, plato_listo_res_to_string(result));
	enviar_mensaje(socket_cliente, paquete_res);

	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	plato_listo_req_destroy(request);
	plato_listo_res_destroy(result);

}

void procesar_confirmar_pedido(int socket_cliente, t_paquete *paquete_req) {
	/*
	 * Este mensaje cumplirá la función de cambiar el estado de un determinado pedido. Para esto recibirá:
	 * 1. El nombre del Restaurante
	 * 2. El ID del pedido
	 * Al recibir esta información se realizarán las siguientes operaciones:
	 * 1. Verificar si existe la tabla de segmentos de dicho Restaurante. En caso de no existir se deberá informar dicha situación.
	 * 2. Verificar que exista el segmento de dicho pedido dentro de la tabla de segmentos del Restaurante. En caso de no existir se deberá informar dicha situación.
	 * 3. Verificar que el pedido esté en estado “Pendiente”. En caso contrario se deberá informar dicha situación.
	 * 4. Cambiar el estado del pedido de “Pendiente” a “Confirmado”.
	 * 5. Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	 *
	 */
	t_confirmar_pedido_res *result = NULL;
	t_paquete *paquete_res = NULL;
	t_confirmar_pedido_req *request = confirmar_pedido_req_desempaquetar(paquete_req);
	confirmar_pedido_req_to_log("Recibi", request);

	if (restaurante_dictionary_has_key(request->restaurante)) {
		t_segmentos *segmentos = restaurante_dictionary_get(request->restaurante);
		// Obtengo el segmento del pedido
		t_nodo_segmento *segmento = segmentos_get_by_id(segmentos, atoi(request->id_pedido));
		if (segmento != NULL && segmento->estado == PENDIENTE) {
			log_info(logger, "Existe segmento para pedido %s, lo paso a confirmado", request->id_pedido);
			segmento->estado = Confirmado;
			result = confirmar_pedido_res_create(OK_);
			confirmar_pedido_res_to_log("Envio", result);
			paquete_res = confirmar_pedido_res_empaquetar(result);
		} else {
			log_warning(logger, "No existe segmento para pedido %s o no esta pendiente", request->id_pedido);
			result = confirmar_pedido_res_create(FAIL_);
			confirmar_pedido_res_to_log("Envio", result);
			paquete_res = confirmar_pedido_res_empaquetar(result);
		}
	} else {
		// En caso de no existir se deberá informar dicha situación.
		log_warning(logger, "No existe la tabla de segmentos para %s", request->restaurante);
		result = confirmar_pedido_res_create(FAIL_);
		confirmar_pedido_res_to_log("Envio", result);
		paquete_res = confirmar_pedido_res_empaquetar(result);
	}

	log_info_message(MSG_INFO_ENVIO, confirmar_pedido_res_to_string(result));
	enviar_mensaje(socket_cliente, paquete_res);

	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	confirmar_pedido_req_destroy(request);
	confirmar_pedido_res_destroy(result);
}

void procesar_obtener_pedido(int socket_cliente, t_paquete *paquete_req) {
	/*
	 * Este mensaje cumplirá la función de obtener todos los datos de un determinado pedido. Para esto recibirá:
	 * 1. El nombre del Restaurante
	 * 2. El ID del pedido
	 * Al recibir esta información se realizarán las siguientes operaciones:
	 * 1. Verificar si existe la tabla de segmentos de dicho Restaurante. En caso de no existir se deberá informar dicha situación.
	 * 2. Verificar que exista el segmento de dicho pedido dentro de la tabla de segmentos del Restaurante. En caso de no existir se deberá informar dicha situación.
	 * 3. En caso de que las páginas que corresponden al pedido no se encuentren cargadas en memoria principal, se deberán cargar las mismas desde swap (iniciando la elección de víctimas de ser necesario). En caso contrario, proceder al paso  número 4.
	 * 4. Responder el mensaje indicando si se pudo realizar en conjunto con la información del pedido si correspondiera.
	 *
	 */
	t_obtener_pedido_req *request = obtener_pedido_req_desempaquetar(paquete_req);
	t_obtener_pedido_res *result = NULL;
	t_paquete *paquete_res = NULL;
	log_info_message(MSG_INFO_RECIBO, obtener_pedido_req_to_string(request));

	if (restaurante_dictionary_has_key(request->restaurante)) {
		t_segmentos *segmentos = restaurante_dictionary_get(request->restaurante);
		// Obtengo el segmento del pedido
		t_nodo_segmento *segmento = segmentos_get_by_id(segmentos, atoi(request->id_pedido));
		if (segmento != NULL) {
			log_info(logger, "Existe segmento para pedido %s, lo paso a confirmado", request->id_pedido);
			t_list *estado_platos = platos_obtener_platos(segmento->paginas);
			result = obtener_pedido_res_create(segmento->estado, estado_platos);
			paquete_res = obtener_pedido_res_empaquetar(result);
			list_destroy_and_destroy_elements(estado_platos, (void*) estado_plato_destroy);
		} else {
			log_warning(logger, "No existe segmento para pedido %s o no esta pendiente", request->id_pedido);
			//TODO enviar mensaje error
		}
	} else {
		// En caso de no existir se deberá informar dicha situación.
		log_warning(logger, "No existe la tabla de segmentos para %s", request->restaurante);
		//TODO enviar mensaje error
	}

	log_info_message(MSG_INFO_ENVIO, obtener_pedido_res_to_string(result));
	enviar_mensaje(socket_cliente, paquete_res);

	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	obtener_pedido_req_destroy(request);
	obtener_pedido_res_destroy(result);

}

void procesar_guardar_plato(int socket_cliente, t_paquete *paquete_req) {
	/**
	 * Este mensaje cumplirá la función de agregar un nuevo plato a un determinado pedido. Para esto recibirá:
	 * 1. El nombre del Restaurante
	 * 2. El ID del pedido
	 * 3. El plato que se va a agregar
	 * 4. La cantidad del plato a agregar
	 *
	 * Al recibir esta información se realizarán las siguientes operaciones:
	 * 1. Verificar si existe la tabla de segmentos de dicho Restaurante. En caso de no existir se deberá informar dicha situación.
	 * 2. Verificar que exista el segmento de dicho pedido dentro de la tabla de segmentos del Restaurante. En caso de no existir se deberá informar dicha situación.
	 * 3. Verificar si existe el plato dentro de la tabla de páginas del pedido. En caso contrario, se deberá asignar una nueva página al segmento.
	 * 4. En caso de que la página que corresponde al pedido no se encuentre cargada en memoria principal, se deberá cargar la misma desde swap
	 * 		(iniciando la elección de víctima de ser necesario). En caso contrario, proceder al paso  número 5.
	 * 5. Se deberá agregar el plato y anexar la cantidad que se tiene que cocinar de dicho plato.
	 * 6. Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	 */
	t_guardar_plato_req *request;
	t_guardar_plato_res *result;
	t_paquete *paquete_res;

	request = guardar_plato_req_desempaquetar(paquete_req);
	log_info_message(MSG_INFO_RECIBO, guardar_plato_req_to_string(request));

	if (restaurante_dictionary_has_key(request->restaurante)) {
		t_segmentos *segmentos = restaurante_dictionary_get(request->restaurante);
		// Obtengo el segmento del pedido
		t_nodo_segmento *segmento = segmentos_get_by_id(segmentos, atoi(request->id_pedido));
		if (segmento != NULL && segmento->estado == PENDIENTE) {
			t_nodo_pagina *pagina = platos_buscar_plato(segmento->paginas, request->comida);
			if (pagina != NULL) {
				platos_agregar_cantidad(pagina, request->cantidad);
			} else {
				platos_agregar(segmento->paginas, request->comida, request->cantidad);
			}
			log_info(logger, "Se agrego el plato %s o cantidad %d correctamente", request->comida,
					request->cantidad);
			result = guardar_plato_res_create(OK_);
			paquete_res = guardar_plato_res_empaquetar(result);
		} else {
			log_warning(logger, "No existe segmento para pedido %s o no esta pendiente", request->id_pedido);
			result = guardar_plato_res_create(FAIL_);
			paquete_res = guardar_plato_res_empaquetar(result);
		}
	} else {
		// En caso de no existir se deberá informar dicha situación.
		log_warning(logger, "No existe la tabla de segmentos para %s", request->restaurante);
		result = guardar_plato_res_create(FAIL_);
		paquete_res = guardar_plato_res_empaquetar(result);
	}
	log_info_message(MSG_INFO_ENVIO, guardar_plato_res_to_string(result));
	enviar_mensaje(socket_cliente, paquete_res);

	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	guardar_plato_req_destroy(request);
	guardar_plato_res_destroy(result);
}

void procesar_guardar_pedido(int socket_cliente, t_paquete *paquete_req) {
	/*
	 * Este mensaje cumplirá la función de inicializar todos los datos asociados a este pedido. Para esto recibirá:
	 *
	 * 1_ Verificar si existe la tabla de segmentos de dicho Restaurante.
	 * 2_ Si la tabla de segmentos no existe, deberá ser creada.
	 * 3_ En caso de no poder crearla, se deberá informar dicha situación.
	 * 4_ Luego, se procederá a crear la tabla de páginas dentro del nuevo segmento.
	 * 5_ Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	 */

	t_guardar_pedido_req *request = guardar_pedido_req_desempaquetar(paquete_req);
	log_info_message(MSG_INFO_RECIBO, guardar_pedido_req_to_string(request));
	if (!restaurante_dictionary_has_key(request->restaurante)) {
		restaurante_dictionary_put(strdup(request->restaurante), segmentos_new());
	}
	t_segmentos *segmentos = restaurante_dictionary_get(request->restaurante);
	segmentos_agregar_pedido(segmentos, atoi(request->id_pedido));

	t_guardar_pedido_res *result = guardar_pedido_res_create(OK_);

	t_paquete *paquete_res = guardar_pedido_res_empaquetar(result);

	log_info_message(MSG_INFO_ENVIO, guardar_pedido_res_to_string(result));
	enviar_mensaje(socket_cliente, paquete_res);

	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	guardar_pedido_req_destroy(request);
	guardar_pedido_res_destroy(result);
}

void procesar_handshake(int socket_cliente, t_paquete *paquete_req) {
	log_info(logger, "Recibo handshake desde la app socket %d", socket_cliente);
	t_handshake_app_comanda_res *result = handshake_app_comanda_res_create(OK_);
	t_paquete *paquete_res = handshake_app_comanda_res_empaquetar(result);
	log_info_message(MSG_INFO_ENVIO, handshake_app_comanda_res_to_string(result));
	enviar_mensaje(socket_cliente, paquete_res);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	handshake_app_comanda_res_destroy(result);
}

