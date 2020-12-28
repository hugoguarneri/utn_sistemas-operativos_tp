#include "servidor-app.h"

static void procesar_mensajes(int* socket_cliente);
static void procesar_handshake_restaurante_app(int socket_cliente, t_paquete* paquete_req);
static void procesar_consultar_restaurantes(int socket_cliente, t_paquete* paquete_req);
static void procesar_seleccionar_restaurante(int socket_cliente, t_paquete* paquete_req);
static void procesar_consultar_platos(int socket_cliente, t_paquete* paquete_req);
static void procesar_crear_pedido(int socket_cliente, t_paquete* paquete_req);
static void procesar_aniadir_plato(int socket_cliente, t_paquete* paquete_req);
static void procesar_plato_listo(int socket_cliente, t_paquete* paquete_req);
static void procesar_consultar_pedido(int socket_cliente, t_paquete* paquete_req);
static void procesar_confirmar_pedido(int socket_cliente, t_paquete* paquete_req);

void exec_servidor_app() {
	sem_wait(&sem_habilitar_servidor);

	log_info(logger, "[APP] - Inicializando servidor de escucha..");

	char* ip = strdup(config_get_string_value(config, "IP_ESCUCHA"));
	char* puerto = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));

	int socket_servidor = iniciar_servidor(ip, puerto);
	log_info(logger, "[APP] - Escuchando en %s:%s", ip, puerto);

	sem_post(&sem_habilitar_resto_default);

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
	case HANDSHAKE:;
		procesar_handshake_restaurante_app(socket_cliente, paquete);
		break;
	case CONSULTAR_RESTAURANTES:;
		procesar_consultar_restaurantes(socket_cliente, paquete);
		break;
	case SELECCIONAR_RESTAURANTE:;
		procesar_seleccionar_restaurante(socket_cliente, paquete);
		break;
	case CONSULTAR_PLATOS:;
		procesar_consultar_platos(socket_cliente, paquete);
		break;
	case CREAR_PEDIDO:;
		procesar_crear_pedido(socket_cliente, paquete);
		break;
	case ANIADIR_PLATO:;
		procesar_aniadir_plato(socket_cliente, paquete);
		break;
	case PLATO_LISTO:;
		procesar_plato_listo(socket_cliente, paquete);
		break;
	case CONSULTAR_PEDIDO:;
		procesar_consultar_pedido(socket_cliente, paquete);
		break;
	case CONFIRMAR_PEDIDO:;
		procesar_confirmar_pedido(socket_cliente, paquete);
		break;
	case ERROR:;
		log_error(logger, "[APP] - Ocurrio un error al recibir el mensaje.");
		paquete_destroy(paquete);
		liberar_conexion(socket_cliente);
		break;
	default:;
		log_error(logger, "[APP] - El tipo de mensaje recibido es incorrecto.");
		paquete_destroy(paquete);
		liberar_conexion(socket_cliente);
		break;
	}
}

static void procesar_handshake_restaurante_app(int socket_cliente, t_paquete* paquete_req){
	t_handshake_restaurante_app_req* request = handshake_restaurante_app_req_desempaquetar(paquete_req);

	char* request_str = handshake_restaurante_app_req_to_string(request);
	log_info(logger, "[APP] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, request_str);

	if(monitor_info_restaurantes_has_key(request->info_restaurante->nombre)) {
		monitor_info_restaurantes_remove(request->info_restaurante->nombre);
	}

	monitor_info_restaurantes_put(request->info_restaurante->nombre, request->info_restaurante);

	status_code code = OK_;

	t_handshake_restaurante_app_res* response = handshake_restaurante_app_res_create(code);
	t_paquete* paquete_res = handshake_restaurante_app_res_empaquetar(response);

	char* response_str = handshake_restaurante_app_res_to_string(response);
	log_info(logger, "[APP] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	if(enviar_mensaje(socket_cliente, paquete_res) < 0){
		log_error(logger, "[APP] - Ocurrio un error al intentar enviar el paquete- > %s", strerror(errno));
	}

	//handshake_restaurante_app_req_destroy(request);
	handshake_restaurante_app_res_destroy(response);
	liberar_conexion(socket_cliente);
	paquete_destroy(paquete_res);
	paquete_destroy(paquete_req);
	free(request_str);
	free(response_str);
}


/* Se deberán obtener todos los nombres de los restaurantes conectados a dicho módulo y retornarlos.
 * En caso que no exista ninguno conectado se deberá retornar el Restaurante con nombre “Resto Default”.
 * Este último restaurante nos permitirá la prueba de dicho módulo sin la conexión de restaurantes al mismo.
 */
static void procesar_consultar_restaurantes(int socket_cliente, t_paquete* paquete_req){
	char* request_str = consultar_restaurantes_req_to_string();
	log_info(logger, "[APP] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, request_str);

	t_list* restaurantes = monitor_info_restaurantes_get_all();
	t_list* nombres = list_create();

	for(int i = 0; i < restaurantes->elements_count; i++){
		t_info_restaurante* info_restaurante = list_get(restaurantes, i);
		list_add(nombres, strdup(info_restaurante->nombre));
	}

	t_consultar_restaurantes_res* response = consultar_restaurantes_res_create_from_list(nombres);
	t_paquete* paquete_res = consultar_restaurantes_res_empaquetar(response);

	char* response_str = consultar_restaurantes_res_to_string(response);
	log_info(logger, "[APP] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	if(enviar_mensaje (socket_cliente, paquete_res) < 0){
		log_error(logger, "[APP] - Ocurrio un error al intentar enviar el paquete -> %s", strerror(errno));
	}

	list_destroy_and_destroy_elements(nombres, free);
	consultar_restaurantes_res_destroy(response);
	liberar_conexion(socket_cliente);
	paquete_destroy(paquete_res);
	paquete_destroy(paquete_req);
	free(request_str);
	free(response_str);
}


/* Este mensaje se encargará de asociar, de manera lógica, a un Cliente con un Restaurante.
 * Esto permitirá que en la posterior creación o consulta de un pedido se sepa sobre qué restaurante se está realizando.
 */
static void procesar_seleccionar_restaurante(int socket_cliente, t_paquete* paquete_req){
	t_seleccionar_restaurante_req* request = seleccionar_restaurante_req_desempaquetar(paquete_req);

	char* request_str = seleccionar_restaurante_req_to_string(request);
	log_info(logger, "[APP] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, request_str);

	status_code code = FAIL_;

	if(monitor_info_restaurantes_has_key(request->restaurante)) {
		if(monitor_clientes_restaurantes_has_key(request->cliente)) {
			monitor_clientes_restaurantes_remove(request->cliente);
		}
		monitor_clientes_restaurantes_put(strdup(request->cliente), strdup(request->restaurante));
		code = OK_;
	}

	t_seleccionar_restaurante_res* response = seleccionar_restaurante_res_create(code);
	t_paquete* paquete_res = seleccionar_restaurante_res_empaquetar(response);

	char* response_str = seleccionar_restaurante_res_to_string(response);
	log_info(logger, "[APP] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	if(enviar_mensaje(socket_cliente, paquete_res) < 0){
		log_error(logger, "[APP] - Ocurrio un error al intentar enviar el paquete -> %s", strerror(errno));
	}

	paquete_destroy(paquete_req);
	seleccionar_restaurante_req_destroy(request);
	free(request_str);
	seleccionar_restaurante_res_destroy(response);
	paquete_destroy(paquete_res);
	free(response_str);
	liberar_conexion(socket_cliente);
}


/* Este mensaje permitirá la consulta de las recetas del Restaurante seleccionado.
 * De esta manera, el módulo APP servirá de “pasamano” del mensaje al modulo Restaurante requerido.
 * Cabe aclarar que en caso que no exista ningún Restaurante conectado se retornarán la lista de platos por default informado en el archivo de configuración.
 */
static void procesar_consultar_platos(int socket_cliente, t_paquete* paquete_req){
	t_consultar_platos_req* request = consultar_platos_req_desempaquetar(paquete_req);

	char* request_str = consultar_platos_req_to_string(request);
	log_info(logger, "[APP] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, request_str);

	t_info_restaurante* info_restaurante = monitor_info_restaurantes_get(request->restaurante);

	int socket_restaurante = crear_conexion(info_restaurante->ip, info_restaurante->puerto);

	enviar_mensaje(socket_restaurante,  paquete_req);

	t_paquete* paquete_res = recibir_mensaje(socket_restaurante);
	t_consultar_platos_res* response = consultar_platos_res_desempaquetar(paquete_res);

	char* response_str = consultar_platos_res_to_string(response);
	log_info(logger, "[APP] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	enviar_mensaje(socket_cliente,  paquete_res);

	paquete_destroy(paquete_req);
	consultar_platos_req_destroy(request);
	free(request_str);
	//info_restaurante_destroy(info_restaurante);
	paquete_destroy(paquete_res);
	consultar_platos_res_destroy(response);
	free(response_str);
	liberar_conexion(socket_cliente);
}


/* Este mensaje permitirá la creación de un Pedido sobre el restaurante seleccionado. La funcionalidad de este mensaje se descompone en los siguientes pasos:
 * 1. Enviar el mensaje al Restaurante seleccionado y obtener el ID de mensaje que este ultimo genero.
 *    En caso que no exista ningún restaurante conectado se generará un ID arbitrario que defina al pedido (tener en cuenta que debe ser ÚNICO).
 * 2. Enviar el mensaje Guardar Pedido al Módulo Comanda informando la creación de un nuevo pedido.
 *    Si este módulo informa un error deberá informarse que el pedido está rechazado (la casuística para que esto pase se explica en dicho módulo).
 * 3. Retorna el ID del pedido al Cliente que solicitó el pedido.
 */
// TODO: Si la comanda retorna error a quien le tengo que inforar  que el pedido esta rechazado, al restaurante? El cliente solo recibe id_pedido
static void procesar_crear_pedido(int socket_cliente, t_paquete* cp_paquete_req){

	char* cp_request_str = crear_pedido_req_to_string();
	log_info(logger, "[APP] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, cp_request_str);

	t_paquete* ic_paquete_req = consultar_info_cliente_req_empaquetar();
	enviar_mensaje(socket_cliente, ic_paquete_req);
	t_paquete* ic_paquete_res = recibir_mensaje(socket_cliente);
	t_info_cliente* info_cliente = consultar_info_cliente_res_desempaquetar(ic_paquete_res);


	char* restaurante = monitor_clientes_restaurantes_get(info_cliente->id_cliente);
	t_info_restaurante* info_restaurante = monitor_info_restaurantes_get(restaurante);

	int socket_restaurante = crear_conexion(info_restaurante->ip, info_restaurante->puerto);
	enviar_mensaje(socket_restaurante,  cp_paquete_req);

	t_paquete* cp_paquete_res = recibir_mensaje(socket_restaurante);
	t_crear_pedido_res* cp_response = crear_pedido_res_desempaquetar(cp_paquete_res);

	//status_code code = FAIL_;

	if(cp_paquete_res->codigo_operacion != ERROR){
		t_guardar_pedido_req* gp_request = guardar_pedido_req_create(restaurante, cp_response->id_pedido);
		t_paquete* gp_paquete_req = guardar_pedido_req_empaquetar(gp_request);

		char* ip_comanda =  strdup(config_get_string_value(config, "IP_COMANDA"));
		char* puerto_comanda = strdup(config_get_string_value(config, "PUERTO_COMANDA"));
		int socket_comanda = crear_conexion(ip_comanda, puerto_comanda);

		enviar_mensaje(socket_comanda,  gp_paquete_req);

		t_paquete* gp_paquete_res = recibir_mensaje(socket_comanda);
		t_guardar_pedido_res* gp_response = guardar_pedido_res_desempaquetar(gp_paquete_res);

		//code = gp_response->code;

		guardar_pedido_req_destroy(gp_request);
		paquete_destroy(gp_paquete_req);
		free(ip_comanda);
		free(puerto_comanda);
		liberar_conexion(socket_comanda);
		paquete_destroy(gp_paquete_res);
		guardar_pedido_res_destroy(gp_response);
	}

	char* response_str = crear_pedido_res_to_string(cp_response);
	log_info(logger, "[APP] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	enviar_mensaje(socket_cliente,  cp_paquete_res);

	paquete_destroy(cp_paquete_req);
	free(cp_request_str);
	//free(restaurante);
	//info_restaurante_destroy(info_restaurante);
	liberar_conexion(socket_restaurante);
	liberar_conexion(socket_cliente);
	crear_pedido_res_destroy(cp_response);
	paquete_destroy(cp_paquete_res);
	free(response_str);
}


/* Este mensaje permitirá añadir un plato a un determinado pedido. Para esto se recibirá tanto el plato como el ID de Pedido destino.
 * La funcionalidad de este mensaje se descompone en los siguientes pasos:
 * 1. Enviar al Restaurante dueño de dicho Pedido el mensaje que nos llegó. En caso que se trate del “Restaurante Default” pasar al punto 3.
 * 2. Si el Restaurante dueño de dicho pedido retorna un error, se debe informar el rechazo al Cliente solicitado
 * 3. Informar al Módulo Comanda la adhesión de un nuevo Plato a dicho Pedido. Si el módulo informa un error deberá informarse el rechazo del mismo (la casuística para que esto pase se explica en dicho módulo).
 * 4. Retornar OK al cliente que lo solicitó.
 */
static void procesar_aniadir_plato(int socket_cliente, t_paquete* ap_paquete_req){
	t_aniadir_plato_req* ap_request = aniadir_plato_req_desempaquetar(ap_paquete_req);

	char* ap_request_str = aniadir_plato_req_to_string(ap_request);
	log_info(logger, "[APP] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, ap_request_str);

	t_paquete* ic_paquete_req = consultar_info_cliente_req_empaquetar();
	enviar_mensaje(socket_cliente, ic_paquete_req);
	t_paquete* ic_paquete_res = recibir_mensaje(socket_cliente);
	t_info_cliente* info_cliente = consultar_info_cliente_res_desempaquetar(ic_paquete_res);

	char* restaurante = monitor_clientes_restaurantes_get(info_cliente->id_cliente);
	t_info_restaurante* info_restaurante = monitor_info_restaurantes_get(restaurante);

	int socket_restaurante = crear_conexion(info_restaurante->ip, info_restaurante->puerto);

	enviar_mensaje(socket_restaurante,  ap_paquete_req);

	t_paquete* ap_paquete_res = recibir_mensaje(socket_restaurante);
	t_aniadir_plato_res* ap_response = aniadir_plato_res_desempaquetar(ap_paquete_res);

	status_code code = FAIL_;

	if(ap_response->code == OK_){
		t_guardar_plato_req* gp_request = guardar_plato_req_create(restaurante, ap_request->id_pedido, ap_request->plato, 1);
		t_paquete* gp_paquete_req = guardar_plato_req_empaquetar(gp_request);

		char* ip_comanda =  strdup(config_get_string_value(config, "IP_COMANDA"));
		char* puerto_comanda = strdup(config_get_string_value(config, "PUERTO_COMANDA"));
		int socket_comanda = crear_conexion(ip_comanda, puerto_comanda);

		enviar_mensaje(socket_comanda,  gp_paquete_req);

		t_paquete* gp_paquete_res = recibir_mensaje(socket_comanda);
		t_guardar_plato_res* gp_response = guardar_plato_res_desempaquetar(gp_paquete_res);

		code = gp_response->code;

		guardar_plato_req_destroy(gp_request);
		paquete_destroy(gp_paquete_req);
		free(ip_comanda);
		free(puerto_comanda);
		liberar_conexion(socket_comanda);
		paquete_destroy(gp_paquete_res);
		guardar_plato_res_destroy(gp_response);
	}

	t_aniadir_plato_res* ap_response_2 = aniadir_plato_res_create(code);
	t_paquete* ap_paquete_res_2 = aniadir_plato_res_empaquetar(ap_response_2);

	char* response_str = aniadir_plato_res_to_string(ap_response_2);
	log_info(logger, "[APP] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	enviar_mensaje(socket_cliente,  ap_paquete_res_2);

	paquete_destroy(ap_paquete_req);
	aniadir_plato_req_destroy(ap_request);
	free(ap_request_str);
	//free(restaurante);
	//info_restaurante_destroy(info_restaurante);
	liberar_conexion(socket_restaurante);
	liberar_conexion(socket_cliente);
	aniadir_plato_res_destroy(ap_response_2);
	paquete_destroy(ap_paquete_res_2);
	free(response_str);
}


/* Este mensaje permitirá confirmar un pedido creado previamente. Para esto se recibirá el ID de pedido destino.
 * La funcionalidad de este mensaje se descompone en los siguientes pasos:
 * 1. Obtener el Pedido desde el Módulo Comanda. Enviar al Restaurante dueño de dicho Pedido el mensaje que nos llegó.
 *    En caso que se trate del “Restaurante Default”, pasar al punto 3.
 * 2. Si el restaurante dueño de dicho pedido retorna un error, se debe informar el rechazo al cliente solicitado.
 * 3. Generar el PCB (Pedido Control Block) del Pedido en cuestión y dejarlo en el ciclo de planificación
 * 4. Informar al Módulo Comanda la actualización del estado del pedido.
 * 5. Informar al Módulo Cliente que su pedido fue confirmado
 */
static void procesar_confirmar_pedido(int socket_cliente, t_paquete* cp_paquete_req){
	t_confirmar_pedido_req* cp_request = confirmar_pedido_req_desempaquetar(cp_paquete_req);

	char* cp_request_str = confirmar_pedido_req_to_string(cp_request);
	log_info(logger, "[APP] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, cp_request_str);

	t_paquete* ic_paquete_req = consultar_info_cliente_req_empaquetar();
	enviar_mensaje(socket_cliente, ic_paquete_req);
	t_paquete* ic_paquete_res = recibir_mensaje(socket_cliente);
	t_info_cliente* info_cliente = consultar_info_cliente_res_desempaquetar(ic_paquete_res);
	monitor_info_clientes_put(info_cliente->id_cliente, info_cliente);

	char* ip_comanda =  strdup(config_get_string_value(config, "IP_COMANDA"));
	char* puerto_comanda = strdup(config_get_string_value(config, "PUERTO_COMANDA"));
	int socket_comanda = crear_conexion(ip_comanda, puerto_comanda);

	t_info_restaurante* info_restaurante = monitor_info_restaurantes_get(cp_request->restaurante);
	int socket_restaurante = crear_conexion(info_restaurante->ip, info_restaurante->puerto);
	enviar_mensaje(socket_restaurante, cp_paquete_req);

	t_paquete* cp_paquete_res = recibir_mensaje(socket_restaurante);
	t_confirmar_pedido_res* cp_response =  confirmar_pedido_res_desempaquetar(cp_paquete_res);

	char* response_str =  confirmar_pedido_res_to_string(cp_response);
	log_info(logger, "[APP] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	if(cp_response->code == FAIL_) {
		enviar_mensaje(socket_cliente, cp_paquete_res);
	} else {
		double est_inicial = config_get_double_value(config, "ESTIMACION_INICIAL");
		t_pcb* pcb = pcb_create(string_itoa(id_pcb++), cp_request->id_pedido, cp_request->restaurante, NULL, info_cliente->id_cliente, est_inicial);
		cola_planificacion_add(cola_new, pcb);

		log_debug(logger, "[PLANIFICADOR] - Se creo [pcb %s] en COLA NEW - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
				pcb->id, pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);

		sem_post(&sem_cola_new);

		enviar_mensaje(socket_cliente, cp_paquete_res);
		enviar_mensaje(socket_comanda, cp_paquete_req);
		t_paquete* cp_paquete_res_2 = recibir_mensaje(socket_comanda);

		paquete_destroy(cp_paquete_res_2);
	}

	paquete_destroy(cp_paquete_req);
	confirmar_pedido_req_destroy(cp_request);
	free(cp_request_str);
	paquete_destroy(cp_paquete_res);
	confirmar_pedido_res_destroy(cp_response);
	liberar_conexion(socket_cliente);
	liberar_conexion(socket_comanda);
	liberar_conexion(socket_restaurante);
}


/* Cuando llegue este mensaje desde el Restaurante, la App deberá reenviarselo a la comanda para que sume uno a la cantidad lista de dicho plato.
 * La funcionalidad de este mensaje se descompone en los siguientes pasos:
 * 1. Enviar el mensaje Plato Listo a la Comanda.
 * 2. Se deberá ejecutar el mensaje Obtener Pedido a la Comanda con el fin de comparar la cantidad con la cantidad lista.
 *    En el caso de que sean iguales, significará que el repartidor ya puede retirar el pedido del Restaurante, desencadenando los eventos necesarios en la planificación.
 */
static void procesar_plato_listo(int socket_cliente, t_paquete* pl_paquete_req){
	t_plato_listo_req* pl_request = plato_listo_req_desempaquetar(pl_paquete_req);

	char* pl_request_str = plato_listo_req_to_string(pl_request);
	log_info(logger, "[APP] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, pl_request_str);


	char* ip_comanda =  strdup(config_get_string_value(config, "IP_COMANDA"));
	char* puerto_comanda = strdup(config_get_string_value(config, "PUERTO_COMANDA"));

	int socket_comanda = crear_conexion(ip_comanda, puerto_comanda);
	enviar_mensaje(socket_comanda, pl_paquete_req);

	t_paquete* pl_paquete_res = recibir_mensaje(socket_comanda);
	t_plato_listo_res* pl_response = plato_listo_res_desempaquetar(pl_paquete_res);

	status_code code = FAIL_;

	if(pl_response->code == OK_){
		t_obtener_pedido_req* op_request = obtener_pedido_req_create(pl_request->restaurante, pl_request->id_pedido);
		t_paquete* op_paquete_req = obtener_pedido_req_empaquetar(op_request);

		int socket_comanda2 = crear_conexion(ip_comanda, puerto_comanda);
		enviar_mensaje(socket_comanda2,  op_paquete_req);

		t_paquete* op_paquete_res = recibir_mensaje(socket_comanda2);
		t_obtener_pedido_res* op_response = obtener_pedido_res_desempaquetar(op_paquete_res);

		bool is_pedido_listo(){
			bool pedido_listo = true;
			for(int i = 0; i < op_response->estado_platos->elements_count; i++){
				t_estado_plato* estado_plato = list_get(op_response->estado_platos, i);
				if(estado_plato->cant_lista < estado_plato->cant_total){
					pedido_listo = false;
					break;
				}
			}
			return pedido_listo;
		}

		if(is_pedido_listo()){
			desbloquear_pedido_listo(strdup(pl_request->id_pedido), strdup(pl_request->restaurante));
		}

		obtener_pedido_req_destroy(op_request);
		paquete_destroy(op_paquete_req);
		paquete_destroy(op_paquete_res);
		obtener_pedido_res_destroy(op_response);
		liberar_conexion(socket_comanda2);
		code = OK_;
	}

	pl_response = plato_listo_res_create(code);
	t_paquete* paquete_res = plato_listo_res_empaquetar(pl_response);

	char* response_str = plato_listo_res_to_string(pl_response);
	log_info(logger, "[APP] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	enviar_mensaje(socket_cliente, paquete_res);

	paquete_destroy(pl_paquete_req);
	plato_listo_req_destroy(pl_request);
	free(pl_request_str);
	free(ip_comanda);
	free(puerto_comanda);
	paquete_destroy(pl_paquete_res);
	plato_listo_res_destroy(pl_response);
	paquete_destroy(paquete_res);
	free(response_str);
	liberar_conexion(socket_cliente);
	liberar_conexion(socket_comanda);
}


/*Este mensaje permitirá obtener el estado de un pedido previamente confirmado.
 * Para esto se obtendrán los datos desde el Módulo Comanda y se retornarán:
 * 1. El Restaurante
 * 2. El estado del pedido
 * 3. La lista de platos con sus estados
 */
static void procesar_consultar_pedido(int socket_cliente, t_paquete* paquete_req){
	t_consultar_pedido_req* request = consultar_pedido_req_desempaquetar(paquete_req);

	char* request_str = consultar_pedido_req_to_string(request);
	log_info(logger, "[APP] - Socket: %d. Se recibio el mensaje: %s", socket_cliente, request);

	char* ip_comanda =  strdup(config_get_string_value(config, "IP_COMANDA"));
	char* puerto_comanda = strdup(config_get_string_value(config, "PUERTO_COMANDA"));
	int socket_comanda = crear_conexion(ip_comanda, puerto_comanda);

	enviar_mensaje(socket_comanda, paquete_req);

	t_paquete* paquete_res = recibir_mensaje(socket_comanda);
	t_consultar_pedido_res* response = consultar_pedido_res_desempaquetar(paquete_res);

	char* response_str = consultar_pedido_res_to_string(response);
	log_info(logger, "[APP] - Socket: %d. Se va a enviar el mensaje: %s", socket_cliente, response_str);

	enviar_mensaje(socket_cliente, paquete_res);

	paquete_destroy(paquete_req);
	consultar_pedido_req_destroy(request);
	free(request_str);
	free(ip_comanda);
	free(puerto_comanda);
	paquete_destroy(paquete_res);
	consultar_pedido_res_destroy(response);
	free(response_str);
	liberar_conexion(socket_cliente);
	liberar_conexion(socket_comanda);
}



















