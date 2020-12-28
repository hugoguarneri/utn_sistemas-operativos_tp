#include "handshake-cliente-app.h"

t_handshake_cliente_app_req* handshake_cliente_app_req_create(t_info_cliente* info_cliente){
	t_handshake_cliente_app_req* handshake = malloc(sizeof(t_handshake_cliente_app_req));

	handshake->info_cliente = info_cliente_create(
			strdup(info_cliente->ip),
			strdup(info_cliente->puerto),
			strdup(info_cliente->id_cliente),
			info_cliente->pos_x,
			info_cliente->pos_y);

	return handshake;
}

void handshake_cliente_app_req_destroy(t_handshake_cliente_app_req* request){
	info_cliente_destroy(request->info_cliente);
	free(request);
}

t_paquete* handshake_cliente_app_req_empaquetar(t_handshake_cliente_app_req* request){
	uint32_t size = handshake_cliente_app_req_size(request);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t puerto_length = strlen(request->info_cliente->puerto);
	memcpy(stream, &(puerto_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->info_cliente->puerto, puerto_length);
	bytes_escritos += puerto_length;

	memcpy(stream + bytes_escritos, &(request->info_cliente->pos_x), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, &(request->info_cliente->pos_y), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	uint32_t ip_length = strlen(request->info_cliente->ip);
	memcpy(stream + bytes_escritos, &(ip_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->info_cliente->ip, ip_length);
	bytes_escritos += ip_length;

	uint32_t id_cliente_length = strlen(request->info_cliente->id_cliente);
	memcpy(stream + bytes_escritos, &(id_cliente_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->info_cliente->id_cliente, id_cliente_length);
	bytes_escritos += id_cliente_length;

	t_paquete* paquete = paquete_create(HANDSHAKE, size, stream);

	free(stream);
	return paquete;
}

t_handshake_cliente_app_req* handshake_cliente_app_req_desempaquetar(t_paquete* paquete){
	t_handshake_cliente_app_req* request = malloc(sizeof(t_handshake_cliente_app_req));
	request->info_cliente = malloc(sizeof(t_info_cliente));

    void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t puerto_length;
	memcpy(&(puerto_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->info_cliente->puerto = calloc(sizeof(char),puerto_length+1);
    memcpy(request->info_cliente->puerto, stream + bytes_leidos, puerto_length);
    bytes_leidos += puerto_length;

	memcpy(&(request->info_cliente->pos_x), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	memcpy(&(request->info_cliente->pos_y), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	uint32_t ip_length;
	memcpy(&(ip_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->info_cliente->ip = calloc(sizeof(char),ip_length+1);
    memcpy(request->info_cliente->ip, stream + bytes_leidos, ip_length);
    bytes_leidos += ip_length;

	uint32_t id_cliente_length;
	memcpy(&(id_cliente_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->info_cliente->id_cliente = calloc(sizeof(char),id_cliente_length+1);
    memcpy(request->info_cliente->id_cliente, stream + bytes_leidos, id_cliente_length);
    bytes_leidos += id_cliente_length;

	return request;
}

char* handshake_cliente_app_req_to_string(t_handshake_cliente_app_req* request){
	char* request_str = string_from_format("{op_code: %s, size: %d, data: { ip: %s, puerto: %s, id_cliente: %s, pos_x: %d, pos_y: %d }}",
			"HANDSHAKE",
			handshake_cliente_app_req_size(request),
			request->info_cliente->ip,
			request->info_cliente->puerto,
			request->info_cliente->id_cliente,
			request->info_cliente->pos_x,
			request->info_cliente->pos_y);

	return request_str;
}

uint32_t handshake_cliente_app_req_size(t_handshake_cliente_app_req* request){
	return sizeof(uint32_t) + sizeof(uint32_t) +
		   sizeof(uint32_t) + strlen(request->info_cliente->ip) +
		   sizeof(uint32_t) + strlen(request->info_cliente->puerto) +
		   sizeof(uint32_t) + strlen(request->info_cliente->id_cliente);
}


t_handshake_cliente_app_res* handshake_cliente_app_res_create(status_code code){
	t_handshake_cliente_app_res* response = malloc(sizeof(t_handshake_cliente_app_res));
	response->code = code;

	return response;
}

void handshake_cliente_app_res_destroy(t_handshake_cliente_app_res* response){
	free(response);
}

t_paquete* handshake_cliente_app_res_empaquetar(t_handshake_cliente_app_res* response){
	uint32_t size = handshake_cliente_app_res_size(response);

	void* stream = malloc(size);
	memcpy(stream, &(response->code), sizeof(response->code));

	t_paquete* paquete = paquete_create(HANDSHAKE, size, stream);

	free(stream);
	return paquete;
}

t_handshake_cliente_app_res* handshake_cliente_app_res_desempaquetar(t_paquete* paquete){
	t_handshake_cliente_app_res* response = malloc(sizeof(t_handshake_cliente_app_res));

	void* stream = paquete->buffer->stream;
	memcpy(&(response->code), stream, sizeof(response->code));

	return response;
}

char* handshake_cliente_app_res_to_string(t_handshake_cliente_app_res* response){
	char* response_str = string_from_format("{op_code: %s, size: %d, data: { code: %s }}",
			"HANDSHAKE",
			handshake_cliente_app_res_size(response),
			response->code == OK_ ? "OK" : "FAIL");

	return response_str;
}

uint32_t handshake_cliente_app_res_size(t_handshake_cliente_app_res* response){
	return sizeof(response->code);
}

