#include "plato-listo.h"


t_plato_listo_req* plato_listo_req_create(char* restaurante, char* id_pedido, char* comida){
	t_plato_listo_req* request = malloc(sizeof(t_plato_listo_req));
	request->restaurante = strdup(restaurante);
	request->id_pedido = strdup(id_pedido);
	request->comida = strdup(comida);

	return request;
}

void plato_listo_req_destroy(t_plato_listo_req* request){
	free(request->restaurante);
	free(request->id_pedido);
	free(request->comida);
	free(request);
}

t_paquete* plato_listo_req_empaquetar(t_plato_listo_req* request){
	uint32_t size = plato_listo_req_size(request);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t restaurante_length = strlen(request->restaurante);
	memcpy(stream + bytes_escritos, &(restaurante_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->restaurante, restaurante_length);
	bytes_escritos += restaurante_length;

	uint32_t id_pedido_length = strlen(request->id_pedido);
	memcpy(stream + bytes_escritos, &(id_pedido_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->id_pedido, id_pedido_length);
	bytes_escritos += id_pedido_length;

	uint32_t comida_length = strlen(request->comida);
	memcpy(stream + bytes_escritos, &(comida_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->comida, comida_length);
	bytes_escritos += comida_length;

	t_paquete* paquete = paquete_create(PLATO_LISTO, size, stream);

	free(stream);
	return paquete;
}

t_plato_listo_req* plato_listo_req_desempaquetar(t_paquete* paquete){
	t_plato_listo_req* request = malloc(sizeof(t_plato_listo_req));

	void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t restaurante_length;
	memcpy(&(restaurante_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->restaurante = calloc(sizeof(char),restaurante_length+1);
	memcpy(request->restaurante, stream + bytes_leidos, restaurante_length);
	bytes_leidos += restaurante_length;

	uint32_t id_pedido_length;
	memcpy(&(id_pedido_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->id_pedido = calloc(sizeof(char),id_pedido_length+1);
	memcpy(request->id_pedido, stream + bytes_leidos, id_pedido_length);
	bytes_leidos += id_pedido_length;

	uint32_t comida_length;
	memcpy(&(comida_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->comida = calloc(sizeof(char),comida_length+1);
	memcpy(request->comida, stream + bytes_leidos, comida_length);
	bytes_leidos += comida_length;

	return request;
}

char* plato_listo_req_to_string(t_plato_listo_req* request){
	char* request_str = string_from_format("{op_code: %s, size: %d, data: { restaurante: %s, id_pedido: %s, comida: %s }}",
			"PLATO_LISTO",
			plato_listo_req_size(request),
			request->restaurante,
			request->id_pedido,
			request->comida);

	return request_str;
}

uint32_t plato_listo_req_size(t_plato_listo_req* request){
	return sizeof(uint32_t) + strlen(request->restaurante) +
		   sizeof(uint32_t) + strlen(request->id_pedido) +
		   sizeof(uint32_t) + strlen(request->comida);
}


t_plato_listo_res* plato_listo_res_create(status_code code){
	t_plato_listo_res* response = malloc(sizeof(t_plato_listo_res));
	response->code = code;

	return response;
}

void plato_listo_res_destroy(t_plato_listo_res* response){
	free(response);
}

t_paquete* plato_listo_res_empaquetar(t_plato_listo_res* response){
	uint32_t size = plato_listo_res_size(response);

	void* stream = malloc(size);
	memcpy(stream, &(response->code), sizeof(response->code));

	t_paquete* paquete = paquete_create(PLATO_LISTO, size, stream);

	free(stream);
	return paquete;
}

t_plato_listo_res* plato_listo_res_desempaquetar(t_paquete* paquete){
	t_plato_listo_res* response = malloc(sizeof(t_plato_listo_res));

	void* stream = paquete->buffer->stream;
	memcpy(&(response->code), stream, sizeof(response->code));

	return response;
}

char* plato_listo_res_to_string(t_plato_listo_res* response){
	char* response_str = string_from_format("{op_code: %s, size: %d, data: { code: %s }}",
			"PLATO_LISTO",
			plato_listo_res_size(response),
			response->code == OK_ ? "OK" : "FAIL");

	return response_str;
}

uint32_t plato_listo_res_size(t_plato_listo_res* response){
	return sizeof(response->code);
}

