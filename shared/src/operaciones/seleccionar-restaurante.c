#include "seleccionar-restaurante.h"


t_seleccionar_restaurante_req* seleccionar_restaurante_req_create(char* cliente, char* restaurante){
	t_seleccionar_restaurante_req* request = malloc(sizeof(t_seleccionar_restaurante_req));
	request->cliente = strdup(cliente);
	request->restaurante = strdup(restaurante);

	return request;
}

void seleccionar_restaurante_req_destroy(t_seleccionar_restaurante_req* request){
	free(request->cliente);
	free(request->restaurante);
	free(request);
}

t_paquete* seleccionar_restaurante_req_empaquetar(t_seleccionar_restaurante_req* request){
	uint32_t size = seleccionar_restaurante_req_size(request);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t cliente_length = strlen(request->cliente);
	memcpy(stream + bytes_escritos, &(cliente_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->cliente, cliente_length);
	bytes_escritos += cliente_length;

	uint32_t restaurante_length = strlen(request->restaurante);
	memcpy(stream + bytes_escritos, &(restaurante_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->restaurante, restaurante_length);
	bytes_escritos += restaurante_length;

	t_paquete* paquete = paquete_create(SELECCIONAR_RESTAURANTE, size, stream);

	free(stream);
	return paquete;
}

t_seleccionar_restaurante_req* seleccionar_restaurante_req_desempaquetar(t_paquete* paquete){
	t_seleccionar_restaurante_req* request = malloc(sizeof(t_seleccionar_restaurante_req));

    void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t cliente_length;
	memcpy(&(cliente_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->cliente = calloc(sizeof(char),cliente_length+1);
    memcpy(request->cliente, stream + bytes_leidos, cliente_length);
    bytes_leidos += cliente_length;

    uint32_t restaurante_length;
	memcpy(&(restaurante_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->restaurante = calloc(sizeof(char),restaurante_length+1);
    memcpy(request->restaurante, stream + bytes_leidos, restaurante_length);
    bytes_leidos += restaurante_length;


	return request;
}

char* seleccionar_restaurante_req_to_string(t_seleccionar_restaurante_req* request){
	char* request_str = string_from_format("{op_code: %s, size: %d, data: { cliente: %s, restaurante: %s }}",
			"SELECCIONAR_RESTAURANTE",
			seleccionar_restaurante_req_size(request),
			request->cliente,
			request->restaurante);

	return request_str;
}

uint32_t seleccionar_restaurante_req_size(t_seleccionar_restaurante_req* request){
	return sizeof(uint32_t) + strlen(request->cliente) +
		   sizeof(uint32_t) + strlen(request->restaurante);
}


t_seleccionar_restaurante_res* seleccionar_restaurante_res_create(status_code code){
	t_seleccionar_restaurante_res* response = malloc(sizeof(t_seleccionar_restaurante_res));
	response->code = code;

	return response;
}

void seleccionar_restaurante_res_destroy(t_seleccionar_restaurante_res* response){
	free(response);
}

t_paquete* seleccionar_restaurante_res_empaquetar(t_seleccionar_restaurante_res* response){
	uint32_t size = seleccionar_restaurante_res_size(response);

	void* stream = malloc(size);
	memcpy(stream, &(response->code), sizeof(response->code));

	t_paquete* paquete = paquete_create(SELECCIONAR_RESTAURANTE, size, stream);

	free(stream);
	return paquete;

}

t_seleccionar_restaurante_res* seleccionar_restaurante_res_desempaquetar(t_paquete* paquete){
	t_seleccionar_restaurante_res* response = malloc(sizeof(t_seleccionar_restaurante_res));

	void* stream = paquete->buffer->stream;
	memcpy(&(response->code), stream, sizeof(response->code));

	return response;
}

char* seleccionar_restaurante_res_to_string(t_seleccionar_restaurante_res* response){
	char* response_str = string_from_format("{op_code: %s, size: %d, data: { code: %s }}",
			"SELECCIONAR_RESTAURANTE",
			seleccionar_restaurante_res_size(response),
			response->code == OK_ ? "OK" : "FAIL");

	return response_str;
}

uint32_t seleccionar_restaurante_res_size(t_seleccionar_restaurante_res* response){
	return sizeof(response->code);
}
