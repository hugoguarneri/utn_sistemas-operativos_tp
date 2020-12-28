#include <servidor/servidor-restaurante.h>

static void procesar_consultar_platos(int socket_app, t_paquete* paquete) {

	/*
	 * Se consulta al Módulo Sindicato, todos los platos del Restaurante;
	 * de esta forma, se podrá consultar información de los platos que el restaurante sepa cocinar.
	 */
	t_consultar_platos_req* req = consultar_platos_req_desempaquetar(paquete);
	char* request_str = consultar_platos_req_to_string(req);
	log_info(logger, "Se recibio el mensaje de la app: %s", request_str);

	char* ip_sindicato = strdup(config_get_string_value(config, "IP_SINDICATO"));
	char* puerto_sindicato = strdup(config_get_string_value(config, "PUERTO_SINDICATO"));
	char* restaurante_name = strdup(config_get_string_value(config, "NOMBRE_RESTAURANTE"));

	t_paquete* paquete_req = consultar_platos_req_empaquetar(req);

	int socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);

	log_info(logger, "Enviando al sindicato CONSULTAR_PLATOS del restaurante: %s", restaurante_name);

	if (enviar_mensaje(socket_sindicato, paquete_req) < 0) {

		log_error(logger, "Ocurrio un error al intentar enviar el paquete: %s", strerror(errno));
		printf("Ocurrio un error al intentar enviar el paquete.\n");

		free(ip_sindicato);
		free(puerto_sindicato);
		free(restaurante_name);
		free(request_str);
		paquete_destroy(paquete_req);
		liberar_conexion(socket_sindicato);
		consultar_platos_req_destroy(req);

		return;
	}

	log_info(logger, "Recibiendo respuesta del sindicato CONSULTAR_PLATOS del restaurante: %s", restaurante_name);
	t_paquete* paquete_res = recibir_mensaje(socket_sindicato);

	if (paquete_res->codigo_operacion != CONSULTAR_PLATOS) {

		log_error(logger, "Ocurrió un error al intentar recibir el paquete: %s", strerror(errno));
		printf("Ocurrio un error al intentar recibir el paquete. [%d]\n", paquete_res->codigo_operacion);

		free(ip_sindicato);
		free(puerto_sindicato);
		free(restaurante_name);
		free(request_str);
		paquete_destroy(paquete_req);
		paquete_destroy(paquete_res);
		liberar_conexion(socket_sindicato);
		consultar_platos_req_destroy(req);

		return;
	}

	log_info(logger, "Enviando respuesta a la app de CONSULTAR_PLATOS del restaurante: %s", restaurante_name);

	if (enviar_mensaje(socket_app, paquete_res) < 0) {
		log_error(logger, "Ocurrio un error al intentar enviar el paquete: %s", strerror(errno));
		printf("Ocurrio un error al intentar enviar el paquete.\n");
	}

	free(ip_sindicato);
	free(puerto_sindicato);
	free(restaurante_name);
	free(request_str);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	liberar_conexion(socket_sindicato);
	consultar_platos_req_destroy(req);
}

static void procesar_crear_pedido(int socket_app, t_paquete* paquete) {
	/*
	 * El Restaurante enviará Guardar Pedido al Módulo Sindicato para la creación de un pedido.
	 * Para esto, el restaurante generará un ID de pedido nuevo UNICO que permitirá identificarlo dentro de dicho restaurante.
	 * */
	char* ip_sindicato = strdup(config_get_string_value(config, "IP_SINDICATO"));
	char* puerto_sindicato = strdup(config_get_string_value(config, "PUERTO_SINDICATO"));
	char* restaurante_name = strdup(config_get_string_value(config, "NOMBRE_RESTAURANTE"));
	log_info(logger, "Se recibio el mensaje CREAR_PEDIDO de la app");

	char* id = string_itoa(++id_pedido);

	log_info(logger, "Enviando al sindicato GUARDAR_PEDIDO del restaurante: %s, id_pedido: %s", restaurante_name, id);

	t_guardar_pedido_req* req = guardar_pedido_req_create(restaurante_name, id);
	t_paquete* paquete_req = guardar_pedido_req_empaquetar(req);

	int socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);
	enviar_mensaje(socket_sindicato, paquete_req);

	t_paquete* paquete_res_sind = recibir_mensaje(socket_sindicato);
	t_guardar_pedido_res* res = guardar_pedido_res_desempaquetar(paquete_res_sind);

	log_info(logger, "Recibiendo respuesta del sindicato GUARDAR_PEDIDO del restaurante: %s, id_pedido: %s - ", restaurante_name, id, res->code);

	if(res->code == FAIL_){
		log_error(logger, "Se recibio de sidicanto el mensaje FAIL para la consulta GUARDAR_PEDIDO restaurante: %s, id_pedido %s", restaurante_name, id);
	}

	log_info(logger, "Enviando respuesta a la app de CREAR_PEDIDO del pedido: %s", id);

	t_crear_pedido_res* cp_res = crear_pedido_res_create(id);
	t_paquete* paquete_app = crear_pedido_res_empaquetar(cp_res);

	enviar_mensaje(socket_app, paquete_app);

	free(ip_sindicato);
	free(puerto_sindicato);
	free(restaurante_name);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res_sind);
	paquete_destroy(paquete_app);
	liberar_conexion(socket_sindicato);
	guardar_pedido_req_destroy(req);
	guardar_pedido_res_destroy(res);
	crear_pedido_res_destroy(cp_res);
}

static void procesar_aniadir_plato(int socket_app, t_paquete* paquete) {
	/*
	 * A través del envío del mensaje Guardar Plato al Módulo Sindicato,
	 * agrega un plato correspondiente a un pedido específico,
	 * que se encontrará relacionado con el Restaurante que envió dicho mensaje.
	 * Solo se podrá crear platos sobre pedidos existentes.
	 * */

	t_aniadir_plato_req* ap_request = aniadir_plato_req_desempaquetar(paquete);
	char* ap_request_str = aniadir_plato_req_to_string(ap_request);
	log_info(logger, "Se recibio el mensaje de la app: %s", ap_request_str);

	char* ip_sindicato = strdup(config_get_string_value(config, "IP_SINDICATO"));
	char* puerto_sindicato = strdup(config_get_string_value(config, "PUERTO_SINDICATO"));
	char* restaurante_name = strdup(config_get_string_value(config, "NOMBRE_RESTAURANTE"));

	t_guardar_plato_req* gp_request = guardar_plato_req_create(restaurante_name, ap_request->id_pedido, ap_request->plato, 1);
	t_paquete* gp_paquete_req = guardar_plato_req_empaquetar(gp_request);

	int socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);

	if (enviar_mensaje(socket_sindicato, gp_paquete_req) < 0) {

		log_error(logger,"Ocurrio un error al intentar enviar el paquete -> %s", strerror(errno));
		printf("Ocurrio un error al intentar enviar el paquete.\n");

		paquete_destroy(paquete);
		paquete_destroy(gp_paquete_req);
		aniadir_plato_req_destroy(ap_request);
		guardar_plato_req_destroy(gp_request);
		free(ap_request_str);
		free(ip_sindicato);
		free(puerto_sindicato);
		free(restaurante_name);
		liberar_conexion(socket_sindicato);

		return;
	}

	log_info(logger, "Se envía GUARDAR_PLATO al sindicato del restaurante: %s, id pedido: %s", restaurante_name, ap_request->id_pedido);

	t_paquete* sind_paquete_res = recibir_mensaje(socket_sindicato);

	if (sind_paquete_res->codigo_operacion != GUARDAR_PLATO) {

		log_error(logger, "Ocurrió un error al intentar recibir el paquete: %s", strerror(errno));
		printf("Ocurrio un error al intentar recibir el paquete. [%d]\n", sind_paquete_res->codigo_operacion);

		paquete_destroy(paquete);
		paquete_destroy(gp_paquete_req);
		paquete_destroy(sind_paquete_res);
		aniadir_plato_req_destroy(ap_request);
		guardar_plato_req_destroy(gp_request);
		free(ap_request_str);
		free(ip_sindicato);
		free(puerto_sindicato);
		free(restaurante_name);
		liberar_conexion(socket_sindicato);

		return;
	}

	t_guardar_plato_res* sind_response = guardar_plato_res_desempaquetar(sind_paquete_res);

	log_info(logger, "Se recibe GUARDAR_PLATO del sindicato. Restaurante: %s, id pedido: %s", restaurante_name, ap_request->id_pedido);

	t_aniadir_plato_res* ap_response = aniadir_plato_res_create(sind_response->code);
	t_paquete* ap_paquete_res = aniadir_plato_res_empaquetar(ap_response);

	char* response_str = aniadir_plato_res_to_string(ap_response);
	log_info(logger, "Se va a enviar el mensaje a la app: %s", response_str);

	if (enviar_mensaje(socket_app, ap_paquete_res) < 0) {
		log_error(logger, "Ocurrio un error al intentar enviar el paquete -> %s", strerror(errno));
		printf("Ocurrio un error al intentar enviar el paquete.\n");
	}

	paquete_destroy(paquete);
	paquete_destroy(sind_paquete_res);
	paquete_destroy(gp_paquete_req);
	paquete_destroy(ap_paquete_res);
	aniadir_plato_req_destroy(ap_request);
	aniadir_plato_res_destroy(ap_response);
	guardar_plato_res_destroy(sind_response);
	guardar_plato_req_destroy(gp_request);
	free(ap_request_str);
	free(response_str);
	free(ip_sindicato);
	free(puerto_sindicato);
	free(restaurante_name);
	liberar_conexion(socket_sindicato);
}

static void procesar_consultar_pedido(int socket_app, t_paquete* paquete) {
	/*
	 * Se le enviará al Módulo Sindicato el mensaje Obtener Pedido especificando un pedido del Restaurante,
	 * con la finalidad de obtener información actualizada del mismo, como por ejemplo, la cantidad lista.
	 * */
	t_consultar_pedido_req* cp_request = consultar_pedido_req_desempaquetar(paquete);
	char* cp_request_str = consultar_pedido_req_to_string(cp_request);
	log_info(logger, "Se recibio el mensaje de la app: %s", cp_request_str);

	char* ip_sindicato = strdup(config_get_string_value(config, "IP_SINDICATO"));
	char* puerto_sindicato = strdup(config_get_string_value(config, "PUERTO_SINDICATO"));
	char* restaurante_name = strdup(config_get_string_value(config, "NOMBRE_RESTAURANTE"));

	t_obtener_pedido_req* op_request = obtener_pedido_req_create(restaurante_name, cp_request->id_pedido);
	t_paquete* op_paquete_req = obtener_pedido_req_empaquetar(op_request);

	int socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);

	if (enviar_mensaje(socket_sindicato, op_paquete_req) < 0) {

		log_error(logger,"Ocurrio un error al intentar enviar el paquete -> %s", strerror(errno));
		printf("Ocurrio un error al intentar enviar el paquete.\n");

		paquete_destroy(paquete);
		paquete_destroy(op_paquete_req);
		consultar_pedido_req_destroy(cp_request);
		obtener_pedido_req_destroy(op_request);
		free(cp_request_str);
		free(ip_sindicato);
		free(puerto_sindicato);
		free(restaurante_name);
		liberar_conexion(socket_sindicato);

		return;
	}

	log_info(logger, "Se envía OBTENER_PEDIDO al sindicato del restaurante: %s, id pedido: %s", restaurante_name, cp_request->id_pedido);

	t_paquete* sind_paquete_res = recibir_mensaje(socket_sindicato);

	if (sind_paquete_res->codigo_operacion != OBTENER_PEDIDO) {

		log_error(logger, "Ocurrió un error al intentar recibir el paquete: %s", strerror(errno));
		printf("Ocurrio un error al intentar recibir el paquete. [%d]\n", sind_paquete_res->codigo_operacion);

		paquete_destroy(paquete);
		paquete_destroy(op_paquete_req);
		paquete_destroy(sind_paquete_res);
		consultar_pedido_req_destroy(cp_request);
		obtener_pedido_req_destroy(op_request);
		free(cp_request_str);
		free(ip_sindicato);
		free(puerto_sindicato);
		free(restaurante_name);
		liberar_conexion(socket_sindicato);

		return;
	}

	t_obtener_pedido_res* sind_response = obtener_pedido_res_desempaquetar(sind_paquete_res);

	log_info(logger, "Se recibe OBTENER_PEDIDO del sindicato. Restaurante: %s, id pedido: %s", restaurante_name, cp_request->id_pedido);

	t_consultar_pedido_res* cp_response = consultar_pedido_res_create(restaurante_name,  sind_response->estado, sind_response->estado_platos);
	t_paquete* cp_paquete_res = consultar_pedido_res_empaquetar(cp_response);

	char* response_str = consultar_pedido_res_to_string(cp_response);
	log_info(logger, "Se va a enviar el mensaje a la app: %s", response_str);

	if (enviar_mensaje(socket_app, cp_paquete_res) < 0) {
		log_error(logger, "Ocurrio un error al intentar enviar el paquete -> %s", strerror(errno));
		printf("Ocurrio un error al intentar enviar el paquete.\n");
	}

	paquete_destroy(paquete);
	paquete_destroy(sind_paquete_res);
	paquete_destroy(cp_paquete_res);
	paquete_destroy(op_paquete_req);
	obtener_pedido_res_destroy(sind_response);
	obtener_pedido_req_destroy(op_request);
	consultar_pedido_req_destroy(cp_request);
	consultar_pedido_res_destroy(cp_response);
	free(cp_request_str);
	free(response_str);
	free(ip_sindicato);
	free(puerto_sindicato);
	free(restaurante_name);
	liberar_conexion(socket_sindicato);
}

static void procesar_confirmar_pedido(int socket_app, t_paquete* paquete) {
	/*
	 * Este mensaje permitirá confirmar un pedido creado previamente. Para esto se recibirá el ID de pedido destino.
	 * La funcionalidad de este mensaje se descompone en los siguientes pasos:

	 1) Obtener el Pedido desde el Módulo Sindicato.

	 2) Generar el PCB (Plato Control Block) de cada plato del pedido en cuestión y
	 dejarlo en el ciclo de planificación (que se explicará a continuación en el módulo de Planificación de Platos).
	 Para esto, se deberá ejecutar el mensaje Obtener Receta al Módulo Sindicato
	 para saber la trazabilidad que deberá tener al momento de su ejecución.
	 Cabe aclarar que el número de pedido se deberá guardar dentro del PCB para su futuro uso.

	 3) Informar al Módulo que lo invocó que su pedido fue confirmado.
	 *
	 * */

	t_confirmar_pedido_req* cp_request = confirmar_pedido_req_desempaquetar(paquete);
	char* cp_request_str = confirmar_pedido_req_to_string(cp_request);
	log_info(logger, "Se recibio el mensaje de la app: %s", cp_request_str);

	char* ip_sindicato = strdup(config_get_string_value(config, "IP_SINDICATO"));
	char* puerto_sindicato = strdup(config_get_string_value(config, "PUERTO_SINDICATO"));
	char* restaurante_name = strdup(config_get_string_value(config, "NOMBRE_RESTAURANTE"));
	int quantum = config_get_int_value(config, "QUANTUM");

	t_obtener_pedido_req* op_req = obtener_pedido_req_create(restaurante_name, cp_request->id_pedido);
	t_paquete* op_paq = obtener_pedido_req_empaquetar(op_req);

	int socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);

	enviar_mensaje(socket_sindicato, op_paq);

	log_info(logger, "Se envió OBTENER_PEDIDO al sindicato del restaurante: %s, id pedido: %s", restaurante_name, cp_request->id_pedido);

	t_paquete* sind_paquete_res = recibir_mensaje(socket_sindicato);
	liberar_conexion(socket_sindicato);

	t_obtener_pedido_res* op_res = obtener_pedido_res_desempaquetar(sind_paquete_res);
	char* op_res_to_string = obtener_pedido_res_to_string(op_res);

	log_info(logger, "Se recibe OBTENER_PEDIDO del sindicato: %s", op_res_to_string);

	for(int i = 0; i < op_res->estado_platos->elements_count; i++){

		t_estado_plato* plato = list_get(op_res->estado_platos, i);

		//Aca obtengo las recetas
		t_obtener_receta_req* or_req = obtener_receta_req_create(plato->comida);
		t_paquete* or_paq = obtener_receta_req_empaquetar(or_req);
		socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);

		log_info(logger, "Se envía OBTENER_RECETA al sindicato de receta: %s", plato->comida);

		enviar_mensaje(socket_sindicato, or_paq);

		t_paquete* paquete_or = recibir_mensaje(socket_sindicato);
		liberar_conexion(socket_sindicato);

		t_obtener_receta_resp* or_res = obtener_receta_resp_desempaquetar(paquete_or);

		char* or_res_to_string = obtener_receta_resp_to_string(or_res);
		log_info(logger, "Se recibe OBTENER_RECETA del sindicato: %s", or_res_to_string);

		for(int l = 0; l < plato->cant_total; l++){

			id_pcb++;
			t_pcb* pcb = pcb_create(string_itoa(id_pcb), cp_request->id_pedido, NULL, plato->comida, or_res->pasos_tiempo, quantum);

			char* afinidad = AFINIDAD_DEFAULT;

			for(int k = 0; k < info_restaurante->afinidad->elements_count; k++){
				char* afinidad_aux = list_get(info_restaurante->afinidad, k);
				if(strcmp(afinidad_aux, plato->comida) == 0){
					afinidad = afinidad_aux;
					break;
				}
			}
			cola_planificacion_add_pcb(cola_new, afinidad, pcb);
			sem_t* sem_cola_new = dictionary_get(cola_new->dic_semaforos, afinidad);
			sem_post(sem_cola_new);
		}

	}

	//informar al sindicato la actualizacion del estado del pedido: enviar confirmar_plato al sindicato
	t_confirmar_pedido_req* cp_req = confirmar_pedido_req_create(cp_request->id_pedido, restaurante_name);
	t_paquete* cp_paq = confirmar_pedido_req_empaquetar(cp_req);
	socket_sindicato = crear_conexion(ip_sindicato, puerto_sindicato);

	enviar_mensaje(socket_sindicato, cp_paq);

	t_paquete* paquete_cp = recibir_mensaje(socket_sindicato);
	liberar_conexion(socket_sindicato);

	enviar_mensaje(socket_app, paquete_cp);

	paquete_destroy(paquete);
	paquete_destroy(op_paq);
	paquete_destroy(cp_paq);
	paquete_destroy(paquete_cp);
	confirmar_pedido_req_destroy(cp_request);
	confirmar_pedido_req_destroy(cp_req);
	obtener_pedido_req_destroy(op_req);
	obtener_pedido_res_destroy(op_res);
	free(ip_sindicato);
	free(puerto_sindicato);
	free(restaurante_name);
	free(op_res_to_string);
}

static void procesar_mensajes(int *socket) {
	int socket_app = *socket;

	t_paquete *paquete = recibir_mensaje(socket_app);

	switch (paquete->codigo_operacion) {
	case CONSULTAR_PLATOS:
		procesar_consultar_platos(socket_app, paquete);
		break;
	case CREAR_PEDIDO:
		procesar_crear_pedido(socket_app, paquete);
		break;
	case ANIADIR_PLATO:
		procesar_aniadir_plato(socket_app, paquete);
		break;
	case CONSULTAR_PEDIDO:
		procesar_consultar_pedido(socket_app, paquete);
		break;
	case CONFIRMAR_PEDIDO:
		procesar_confirmar_pedido(socket_app, paquete);
		break;
	case ERROR:
		log_error(logger, "Se produjo un error al recibir el mensaje.");
		liberar_conexion(socket_app);
		pthread_exit(NULL);
		break;
	default:
		log_error(logger, "El tipo de mensaje recibido no es correcto.");
		liberar_conexion(socket_app);
		pthread_exit(NULL);
		break;
	}
}

void exec_servidor_restaurante() {

	int socket_servidor = 0;
	sem_wait(&sem_habilitar_servidor);

	char *puerto = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));
	char *ip_escucha = strdup(config_get_string_value(config, "IP_ESCUCHA"));

	socket_servidor = iniciar_servidor(ip_escucha, puerto);
	log_debug(logger, "Iniciando servidor restaurante - %s:%s", ip_escucha, puerto);

	free(puerto);
	free(ip_escucha);
	while (1) {
		log_debug(logger, "Esperando nueva conexion..");
		int socket_app = esperar_cliente_(socket_servidor);

		//procesar_mensajes(&socket_app);
		pthread_create(&thread, NULL, (void*) procesar_mensajes, &socket_app);
		pthread_detach(thread);
	}

	//log_info(logger, "Finalizando servidor restaurante");
}
