#include "aniadir-plato.h"


t_aniadir_plato_req* aniadir_plato_req_create(char* plato, char* id_pedido){
	t_aniadir_plato_req* request = malloc(sizeof(t_aniadir_plato_req));
	request->plato = strdup(plato);
	request->id_pedido = strdup(id_pedido);

	return request;
}

void aniadir_plato_req_destroy(t_aniadir_plato_req* request){
	free(request->plato);
	free(request->id_pedido);
	free(request);
}

t_paquete* aniadir_plato_req_empaquetar(t_aniadir_plato_req* request){
	uint32_t size = aniadir_plato_req_size(request);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t plato_length = strlen(request->plato);
	memcpy(stream + bytes_escritos, &(plato_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->plato, plato_length);
	bytes_escritos += plato_length;

	uint32_t id_pedido_length = strlen(request->id_pedido);
	memcpy(stream + bytes_escritos, &(id_pedido_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->id_pedido, id_pedido_length);
	bytes_escritos += id_pedido_length;

	t_paquete* paquete = paquete_create(ANIADIR_PLATO, size, stream);

	free(stream);
	return paquete;
}

t_aniadir_plato_req* aniadir_plato_req_desempaquetar(t_paquete* paquete){
	t_aniadir_plato_req* request = malloc(sizeof(t_aniadir_plato_req));

    void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t plato_length;
	memcpy(&(plato_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->plato = calloc(sizeof(char),plato_length+1);
    memcpy(request->plato, stream + bytes_leidos, plato_length);
    bytes_leidos += plato_length;

	uint32_t id_pedido_length;
	memcpy(&(id_pedido_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->id_pedido = calloc(sizeof(char),id_pedido_length+1);
    memcpy(request->id_pedido, stream + bytes_leidos, id_pedido_length);
    bytes_leidos += id_pedido_length;

	return request;
}

char* aniadir_plato_req_to_string(t_aniadir_plato_req* request){
	char* request_str = string_from_format("{op_code: %s, size: %d, data: { plato: %s, id_pedido: %s }}",
			"ANIADIR_PLATO",
			aniadir_plato_req_size(request),
			request->plato,
			request->id_pedido);

	return request_str;
}

uint32_t aniadir_plato_req_size(t_aniadir_plato_req* request){
	return sizeof(uint32_t) + strlen(request->plato) +
		   sizeof(uint32_t) + strlen(request->id_pedido);
}


t_aniadir_plato_res* aniadir_plato_res_create(status_code code){
	t_aniadir_plato_res* response = malloc(sizeof(t_aniadir_plato_res));
	response->code = code;

	return response;
}

void aniadir_plato_res_destroy(t_aniadir_plato_res* response){
	free(response);
}

t_paquete* aniadir_plato_res_empaquetar(t_aniadir_plato_res* response){
	uint32_t size = aniadir_plato_res_size(response);

	void* stream = malloc(size);
	memcpy(stream, &(response->code), sizeof(response->code));

	t_paquete* paquete = paquete_create(ANIADIR_PLATO, size, stream);

	free(stream);
	return paquete;
}

t_aniadir_plato_res* aniadir_plato_res_desempaquetar(t_paquete* paquete){
	t_aniadir_plato_res* response = malloc(sizeof(t_aniadir_plato_res));

	void* stream = paquete->buffer->stream;
	memcpy(&(response->code), stream, sizeof(response->code));

	return response;
}

char* aniadir_plato_res_to_string(t_aniadir_plato_res* response){
	char* response_str = string_from_format("{op_code: %s, size: %d, data: { code: %s }}",
			"ANIADIR_PLATO",
			aniadir_plato_res_size(response),
			response->code == OK_ ? "OK" : "FAIL");

	return response_str;
}

uint32_t aniadir_plato_res_size(t_aniadir_plato_res* response){
	return sizeof(response->code);
}
