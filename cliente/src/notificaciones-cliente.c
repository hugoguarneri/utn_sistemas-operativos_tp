#include "notificaciones-cliente.h"

static void imprimir_notificaciones();
static void procesar_mensajes(int* socket_cliente);
static void procesar_finalizar_pedido(int socket_cliente, t_paquete* paquete_req);

pthread_t thread_mensajes;
pthread_t thread_notificaciones;

int notificaicones_nuevas;
char* notificaciones;

void exec_notificaciones_cliente(){

	notificaciones = string_new();
	notificaicones_nuevas = 0;

	pthread_create(&thread_notificaciones, NULL, (void*) imprimir_notificaciones, NULL);
	pthread_detach(thread_notificaciones);

	char *ip = strdup(config_get_string_value(config, "IP_ESCUCHA"));
	char *puerto = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));

	log_info(logger, "Iniciando servidor de notificaciones - %s:%s", ip, puerto);
	int socket_servidor = iniciar_servidor(ip, puerto);

	free(ip);
	free(puerto);

	while(1){
		log_info(logger, "Esperando nueva conexion..");
		int socket_cliente = esperar_cliente_ (socket_servidor);

		pthread_create(&thread_mensajes, NULL, (void*) procesar_mensajes, &socket_cliente);
		pthread_detach(thread_mensajes);
	}

}

static void imprimir_notificaciones(){
	while(1){
		sem_wait(&sem_habilitar_notificaion);
		if(notificaicones_nuevas > 0){
			printf("\ncli > Notificaciones nuevas (%d): \n%s", notificaicones_nuevas, notificaciones);
			free(notificaciones);
			notificaciones = string_new();
			notificaicones_nuevas = 0;
		}
		sem_post(&sem_habilitar_consola);
	}
}

static void procesar_mensajes(int* socket) {
	int socket_cliente = *socket;
	t_paquete* paquete = recibir_mensaje(socket_cliente);

	switch (paquete->codigo_operacion) {
	case FINALIZAR_PEDIDO:;
		procesar_finalizar_pedido(socket_cliente, paquete);
		break;
	case ERROR:;
		log_error(logger, "Ocurrio un error al recibir el mensaje.");
		paquete_destroy(paquete);
		liberar_conexion(socket_cliente);
		break;
	default:;
		log_error(logger, "El tipo de mensaje recibido es incorrecto.");
		paquete_destroy(paquete);
		liberar_conexion(socket_cliente);
		break;
	}
}

static void procesar_finalizar_pedido(int socket_cliente, t_paquete* paquete_req){

	t_finalizar_pedido_req* request = finalizar_pedido_req_desempaquetar(paquete_req);

	char* request_str = finalizar_pedido_req_to_string(request);
	log_info(logger, "Se recibio el mensaje: %s", request_str);


	t_finalizar_pedido_res* response = finalizar_pedido_res_create(OK_);

	char* response_str = finalizar_pedido_res_to_string(response);
	log_info(logger, "Se va a enviar el mensaje: %s", response_str);

	t_paquete* paquete_res = finalizar_pedido_res_empaquetar(response);
	enviar_mensaje(socket_cliente,  paquete_res);


	string_append_with_format(&notificaciones, "   - Pedido finalizado - id_pedido: %s, restaurante: %s \n",
			request->id_pedido, request->restaurante);
	notificaicones_nuevas ++;

	paquete_destroy(paquete_req);
	finalizar_pedido_req_destroy(request);
	free(request_str);
	finalizar_pedido_res_destroy(response);
	free(response_str);
	paquete_destroy(paquete_res);
	liberar_conexion(socket_cliente);
}









