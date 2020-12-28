#include "finalizar-pedido.h"

t_finalizar_pedido_req* finalizar_pedido_req_create(char *id_pedido, char *restaurante) {
	t_finalizar_pedido_req *request = malloc(sizeof(t_finalizar_pedido_req));
	request->id_pedido = strdup(id_pedido);
	request->id_pedido = strdup(id_pedido);
	request->restaurante = strdup(restaurante);

	return request;
}

void finalizar_pedido_req_destroy(t_finalizar_pedido_req *request) {
	free(request->id_pedido);
	free(request->restaurante);
	free(request);
}

t_paquete* finalizar_pedido_req_empaquetar(t_finalizar_pedido_req *request) {
	uint32_t size = finalizar_pedido_req_size(request);

	void *stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t id_pedido_length = strlen(request->id_pedido);
	memcpy(stream + bytes_escritos, &(id_pedido_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->id_pedido, id_pedido_length);
	bytes_escritos += id_pedido_length;

	uint32_t restaurante_length = strlen(request->restaurante);
	memcpy(stream + bytes_escritos, &(restaurante_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->restaurante, restaurante_length);
	bytes_escritos += restaurante_length;

	t_paquete *paquete = paquete_create(FINALIZAR_PEDIDO, size, stream);

	free(stream);
	return paquete;

}

t_finalizar_pedido_req* finalizar_pedido_req_desempaquetar(t_paquete *paquete) {
	t_finalizar_pedido_req *request = malloc(sizeof(t_finalizar_pedido_req));

		void *stream = paquete->buffer->stream;
		int bytes_leidos = 0;

		uint32_t id_pedido_length;
		memcpy(&(id_pedido_length), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);

		request->id_pedido = calloc(sizeof(char),id_pedido_length+1);
		memcpy(request->id_pedido, stream + bytes_leidos, id_pedido_length);
		bytes_leidos += id_pedido_length;

		uint32_t restaurante_length;
		memcpy(&(restaurante_length), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);

		request->restaurante = calloc(sizeof(char),restaurante_length+1);
		memcpy(request->restaurante, stream + bytes_leidos, restaurante_length);
		bytes_leidos += restaurante_length;

		return request;
}

char* finalizar_pedido_req_to_string(t_finalizar_pedido_req *request) {
	char *request_str = string_from_format("{op_code: %s, size: %d, data: { id_pedido: %s, restaurante: %s }}",
			"FINALIZAR_PEDIDO", finalizar_pedido_req_size(request), request->id_pedido, request->restaurante);
	return request_str;
}

void finalizar_pedido_req_to_log(char *accion, t_finalizar_pedido_req *request) {
	char *msg = finalizar_pedido_req_to_string(request);
	log_debug(logger, "%s mensaje %s", accion, msg);
	free(msg);
}

uint32_t finalizar_pedido_req_size(t_finalizar_pedido_req *request) {
	return sizeof(uint32_t) + strlen(request->id_pedido) + sizeof(uint32_t) + strlen(request->restaurante);
}

t_finalizar_pedido_res* finalizar_pedido_res_create(status_code code) {
	t_finalizar_pedido_res *response = malloc(sizeof(t_finalizar_pedido_res));
	response->code = code;

	return response;
}

void finalizar_pedido_res_destroy(t_finalizar_pedido_res *response) {
	free(response);
}

t_paquete* finalizar_pedido_res_empaquetar(t_finalizar_pedido_res *response) {
	uint32_t size = finalizar_pedido_res_size(response);

	void *stream = malloc(size);
	memcpy(stream, &(response->code), sizeof(response->code));

	t_paquete *paquete = paquete_create(FINALIZAR_PEDIDO, size, stream);

	free(stream);
	return paquete;
}

t_finalizar_pedido_res* finalizar_pedido_res_desempaquetar(t_paquete *paquete) {
	t_finalizar_pedido_res *response = malloc(sizeof(t_finalizar_pedido_res));
	void *stream = paquete->buffer->stream;
	memcpy(&(response->code), stream, sizeof(response->code));

	return response;
}

char* finalizar_pedido_res_to_string(t_finalizar_pedido_res *response) {
	char *response_str = string_from_format("{op_code: %s, size: %d, data: { code: %s }}", "FINALIZAR_PEDIDO",
			finalizar_pedido_res_size(response), response->code == OK_ ? "OK" : "FAIL");

	return response_str;
}

void finalizar_pedido_res_to_log(char *accion, t_finalizar_pedido_res *response) {
	char *msg = finalizar_pedido_res_to_string(response);
	log_debug(logger, "%s mensaje %s", accion, msg);
	free(msg);
}

uint32_t finalizar_pedido_res_size(t_finalizar_pedido_res *response) {
	return sizeof(response->code);
}

