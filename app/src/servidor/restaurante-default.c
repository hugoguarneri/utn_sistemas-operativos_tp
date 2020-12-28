#include "restaurante-default.h"

static void exec_hanshake_to_app();
static void exec_servidor_resto_default();
static void procesar_mensajes(int* socket);
static void procesar_consultar_platos(int socket_cliente, t_paquete* paquete_req);
static void procesar_crear_pedido(int socket_cliente, t_paquete* paquete_req);
static void procesar_aniadir_plato(int socket_cliente, t_paquete* paquete_req);
static void procesar_confirmar_pedido(int socket_cliente, t_paquete* paquete_req);

void exec_restaurante_default(){

	sem_wait(&sem_habilitar_resto_default);
	pthread_mutex_init(&mutex_id_pedido_count, NULL);

	id_pedido_count = 0;
	monitor_pedido_default_init();

	char* ip = strdup(config_get_string_value(config, "IP_RESTO_DEFAULT"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_RESTO_DEFAULT"));
	char* nombre = strdup(config_get_string_value(config, "NOMBRE_RESTO_DEFAULT"));
	int pos_x = config_get_int_value(config, "POSICION_RESTO_DEFAULT_X");
	int pos_y = config_get_int_value(config, "POSICION_RESTO_DEFAULT_Y");
	char* platos = strdup(config_get_string_value(config, "PLATOS_RESTO_DEFAULT"));

	info_resto_default = info_restaurante_create(ip, puerto, nombre, pos_x, pos_y);
	platos_resto_default = list_from_string_array(platos);

	exec_hanshake_to_app();
	exec_servidor_resto_default();
}

static void exec_hanshake_to_app(){
	char* ip = strdup(config_get_string_value(config, "IP_ESCUCHA"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));

	log_info(logger, "[RESTO] - Conectando con APP..");

	int socket_cliente = crear_conexion(ip, puerto);

	for(int i = 0; socket_cliente < 0 && i < 10; i++){
		log_info(logger, "[RESTO] - Reintentando hanshake contra App");
		socket_cliente = crear_conexion(ip, puerto);
	}

	if(socket_cliente < 0){
		log_error(logger, "[RESTO] -Ocurrio un error al intentar realizar el hanshake contra Appp -> %s", strerror(errno));
		finalizar_proceso();
	}

	t_handshake_restaurante_app_req* request = handshake_restaurante_app_req_create(info_resto_default);
	t_paquete* paquete_req = handshake_restaurante_app_req_empaquetar(request);

	char* request_str = handshake_restaurante_app_req_to_string(request);
	log_info(logger, "[RESTO] - Socket: %d. Se va a enviar el mensaje a APP: %s", socket_cliente, request_str);

	if(enviar_mensaje(socket_cliente, paquete_req) < 0){
		log_error(logger, "[RESTO] -Ocurrio un error al intentar enviar el paquete -> %s", strerror(errno));
		finalizar_proceso();
	}

	t_paquete* paquete_res = recibir_mensaje(socket_cliente);

	if(paquete_res->codigo_operacion != HANDSHAKE){
		log_error(logger, "[RESTO] -El tipo de mensaje recibido es incorrecto. [%d]\n", paquete_res->codigo_operacion);
		finalizar_proceso();
	}

	t_handshake_restaurante_app_res* response = handshake_restaurante_app_res_desempaquetar(paquete_res);

	char* response_str = handshake_restaurante_app_res_to_string(response);
	log_info(logger, "[RESTO] - Socket: %d. Se recibio el mensaje de APP: %s", socket_cliente, response_str);

	handshake_restaurante_app_req_destroy(request);
	paquete_destroy(paquete_req);
	paquete_destroy(paquete_res);
	liberar_conexion(socket_cliente);
}

static void exec_servidor_resto_default(){
	log_info(logger, "[RESTO] -Inicializando servidor de escucha..");

	int socket_servidor = iniciar_servidor(info_resto_default->ip, info_resto_default->puerto);
	log_info(logger, "[RESTO] - Escuchando en %s:%s", info_resto_default->ip, info_resto_default->puerto);

	while (1) {
		int socket_cliente = esperar_cliente_ (socket_servidor);

		pthread_create(&thread, NULL, (void*) procesar_mensajes, &socket_cliente);
		pthread_detach(thread);
	}
}

static void procesar_mensajes(int* socket) {
	int socket_cliente = *socket;
	t_paquete* paquete = recibir_mensaje(socket_cliente);

	switch (paquete->codigo_operacion) {
	case CONSULTAR_PLATOS:;
		procesar_consultar_platos(socket_cliente, paquete);
		break;
	case CREAR_PEDIDO:;
		procesar_crear_pedido(socket_cliente, paquete);
		break;
	case ANIADIR_PLATO:;
		procesar_aniadir_plato(socket_cliente, paquete);
		break;
	case CONFIRMAR_PEDIDO:;
		procesar_confirmar_pedido(socket_cliente, paquete);
		break;
	case ERROR:;
		log_error(logger, "[RESTO] - Ocurrio un error al recibir el mensaje.");
		paquete_destroy(paquete);
		liberar_conexion(socket_cliente);
		break;
	default:;
		log_error(logger, "[RESTO] - El tipo de mensaje recibido es incorrecto.");
		paquete_destroy(paquete);
		liberar_conexion(socket_cliente);
		break;
	}
}

static void procesar_consultar_platos(int socket_cliente, t_paquete* paquete_req){
	t_consultar_platos_req* request = consultar_platos_req_desempaquetar(paquete_req);

	char* request_str = consultar_platos_req_to_string(request);
	log_info(logger, "[RESTO] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, request_str);

	t_consultar_platos_res* response = consultar_platos_res_create(platos_resto_default);
	t_paquete* paquete_res = consultar_platos_res_empaquetar(response);

	char* response_str = consultar_platos_res_to_string(response);
	log_info(logger, "[RESTO] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	if(enviar_mensaje(socket_cliente, paquete_res) < 0){
		log_error(logger, "[RESTO] - Ocurrio un error al intentar enviar el paquete- > %s", strerror(errno));
	}

	paquete_destroy(paquete_req);
	free(request_str);
	consultar_platos_res_destroy(response);
	paquete_destroy(paquete_res);
	free(response_str);
	liberar_conexion(socket_cliente);
}

static void procesar_crear_pedido(int socket_cliente, t_paquete* paquete_req){
	char* request_str = crear_pedido_req_to_string();
	//log_info(logger, "[RESTO] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, request_str);

	pthread_mutex_lock(&mutex_id_pedido_count);
	id_pedido_count++;
	char* id_pedido = string_itoa(id_pedido_count);
	pthread_mutex_unlock(&mutex_id_pedido_count);

	t_pedido_default* pedido = pedido_default_create(id_pedido);
	monitor_pedido_default_put(id_pedido, pedido);

	t_crear_pedido_res* response = crear_pedido_res_create(id_pedido);
	t_paquete* paquete_res = crear_pedido_res_empaquetar(response);

	char* response_str = crear_pedido_res_to_string(response);
	log_info(logger, "[RESTO] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	if(enviar_mensaje(socket_cliente, paquete_res) < 0){
		log_error(logger, "[RESTO] - Ocurrio un error al intentar enviar el paquete- > %s", strerror(errno));
	}

	paquete_destroy(paquete_req);
	free(request_str);
	free(id_pedido);
	crear_pedido_res_destroy(response);
	paquete_destroy(paquete_res);
	free(response_str);
	liberar_conexion(socket_cliente);
}

static void procesar_aniadir_plato(int socket_cliente, t_paquete* paquete_req){
	t_aniadir_plato_req* request = aniadir_plato_req_desempaquetar(paquete_req);

	char* request_str = aniadir_plato_req_to_string(request);
	log_info(logger, "[RESTO] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, request_str);

	status_code code = FAIL_;

	if(monitor_pedido_default_has_key(request->id_pedido)){
		code = OK_;
		t_pedido_default* pedido = monitor_pedido_default_get(request->id_pedido);
		pedido_default_aniadir_plato(pedido, request->plato);
		monitor_pedido_default_put(request->id_pedido, pedido);
	}

	t_aniadir_plato_res* response = aniadir_plato_res_create(code);
	t_paquete* paquete_res = aniadir_plato_res_empaquetar(response);

	char* response_str = aniadir_plato_res_to_string(response);
	log_info(logger, "[RESTO] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	if(enviar_mensaje(socket_cliente, paquete_res) < 0){
		log_error(logger, "[RESTO] - Ocurrio un error al intentar enviar el paquete- > %s", strerror(errno));
	}

	paquete_destroy(paquete_req);
	free(request_str);
	paquete_destroy(paquete_res);
	free(response_str);
	liberar_conexion(socket_cliente);
}

static void procesar_confirmar_pedido(int socket_cliente, t_paquete* paquete_req){
	t_confirmar_pedido_req* request = confirmar_pedido_req_desempaquetar(paquete_req);

	char* request_str = confirmar_pedido_req_to_string(request);
	log_info(logger, "[RESTO] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, request_str);

	status_code code = FAIL_;

	if(monitor_pedido_default_has_key(request->id_pedido)){
		code = OK_;
	}

	t_confirmar_pedido_res* response = confirmar_pedido_res_create(code);
	t_paquete* paquete_res = confirmar_pedido_res_empaquetar(response);


	char* response_str = confirmar_pedido_res_to_string(response);
	log_info(logger, "[RESTO] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	if(enviar_mensaje(socket_cliente, paquete_res) < 0){
		log_error(logger, "[RESTO] - Ocurrio un error al intentar enviar el paquete- > %s", strerror(errno));
	}

	//Envio mensaje de plato listo a la app por cada plato del pedido.
	t_pedido_default* pedido = monitor_pedido_default_get(request->id_pedido);
	for(int i = 0; i < pedido->platos->elements_count; i++){
		char* ip_app = strdup(config_get_string_value(config, "IP_ESCUCHA"));
		char* puerto_app = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));

		int socket_app = crear_conexion(ip_app, puerto_app);

		char* plato = list_get(pedido->platos, i);
		t_plato_listo_req* pl_request = plato_listo_req_create(request->restaurante, request->id_pedido, plato);

		char* pl_request_str = plato_listo_req_to_string(pl_request);
		log_info(logger, "[RESTO] - Socket: %d. Se va a enviar el mensaje: %s", socket_app, pl_request_str);

		t_paquete* pl_paquete_req = plato_listo_req_empaquetar(pl_request);

		enviar_mensaje(socket_app, pl_paquete_req);
		t_paquete* pl_paquete_res = recibir_mensaje(socket_app);

		t_plato_listo_res* pl_response = plato_listo_res_desempaquetar(pl_paquete_res);

		char* pl_response_str = plato_listo_res_to_string(pl_response);
		log_info(logger, "[RESTO] - Socket: %d. Se recibio el mensaje: %s", socket_app, pl_response_str);

		free(ip_app);
		free(puerto_app);
		plato_listo_req_destroy(pl_request);
		free(pl_request_str);
		paquete_destroy(pl_paquete_req);
		paquete_destroy(pl_paquete_res);
		plato_listo_res_destroy(pl_response);
		free(pl_response_str);
		liberar_conexion(socket_app);
	}

	paquete_destroy(paquete_req);
	free(request_str);
	paquete_destroy(paquete_res);
	free(response_str);
	liberar_conexion(socket_cliente);
}

