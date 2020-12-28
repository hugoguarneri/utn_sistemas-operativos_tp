#include "handshake-restaurante-app.h"

t_handshake_restaurante_app_req* handshake_restaurante_app_req_create(t_info_restaurante* info_restaurante){
	t_handshake_restaurante_app_req* handshake = malloc(sizeof(t_handshake_restaurante_app_req));

	handshake->info_restaurante = info_restaurante_create(
			strdup(info_restaurante->ip),
			strdup(info_restaurante->puerto),
			strdup(info_restaurante->nombre),
			info_restaurante->pos_x,
			info_restaurante->pos_y);

	return handshake;
}

void handshake_restaurante_app_req_destroy(t_handshake_restaurante_app_req* request){
	info_restaurante_destroy(request->info_restaurante);
	free(request);
}

t_paquete* handshake_restaurante_app_req_empaquetar(t_handshake_restaurante_app_req* request){
	uint32_t size = handshake_restaurante_app_req_size(request);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t puerto_length = strlen(request->info_restaurante->puerto);
	memcpy(stream, &(puerto_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->info_restaurante->puerto, puerto_length);
	bytes_escritos += puerto_length;

	memcpy(stream + bytes_escritos, &(request->info_restaurante->pos_x), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, &(request->info_restaurante->pos_y), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	uint32_t ip_length = strlen(request->info_restaurante->ip);
	memcpy(stream + bytes_escritos, &(ip_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->info_restaurante->ip, ip_length);
	bytes_escritos += ip_length;

	uint32_t nombre_length = strlen(request->info_restaurante->nombre);
	memcpy(stream + bytes_escritos, &(nombre_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->info_restaurante->nombre, nombre_length);
	bytes_escritos += nombre_length;

	t_paquete* paquete = paquete_create(HANDSHAKE, size, stream);

	free(stream);
	return paquete;
}

t_handshake_restaurante_app_req* handshake_restaurante_app_req_desempaquetar(t_paquete* paquete){
	t_handshake_restaurante_app_req* request = malloc(sizeof(t_handshake_restaurante_app_req));
	request->info_restaurante = malloc(sizeof(t_info_restaurante));

    void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t puerto_length;
	memcpy(&(puerto_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->info_restaurante->puerto = calloc(sizeof(char),puerto_length+1);
    memcpy(request->info_restaurante->puerto, stream + bytes_leidos, puerto_length);
    bytes_leidos += puerto_length;

	memcpy(&(request->info_restaurante->pos_x), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	memcpy(&(request->info_restaurante->pos_y), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	uint32_t ip_length;
	memcpy(&(ip_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->info_restaurante->ip = calloc(sizeof(char),ip_length+1);
    memcpy(request->info_restaurante->ip, stream + bytes_leidos, ip_length);
    bytes_leidos += ip_length;

	uint32_t nombre_length;
	memcpy(&(nombre_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->info_restaurante->nombre = calloc(sizeof(char),nombre_length+1);
    memcpy(request->info_restaurante->nombre, stream + bytes_leidos, nombre_length);
    bytes_leidos += nombre_length;

	return request;
}

char* handshake_restaurante_app_req_to_string(t_handshake_restaurante_app_req* request){
	char* request_str = string_from_format("{op_code: %s, size: %d, data: { ip: %s, puerto: %s, nombre: %s, pos_x: %d, pos_y: %d }}",
			"HANDSHAKE",
			handshake_restaurante_app_req_size(request),
			request->info_restaurante->ip,
			request->info_restaurante->puerto,
			request->info_restaurante->nombre,
			request->info_restaurante->pos_x,
			request->info_restaurante->pos_y);

	return request_str;
}

uint32_t handshake_restaurante_app_req_size(t_handshake_restaurante_app_req* request){
	return sizeof(uint32_t) + sizeof(uint32_t) +
		   sizeof(uint32_t) + strlen(request->info_restaurante->ip) +
		   sizeof(uint32_t) + strlen(request->info_restaurante->puerto) +
		   sizeof(uint32_t) + strlen(request->info_restaurante->nombre);
}


t_handshake_restaurante_app_res* handshake_restaurante_app_res_create(status_code code){
	t_handshake_restaurante_app_res* response = malloc(sizeof(t_handshake_restaurante_app_res));
	response->code = code;

	return response;
}

void handshake_restaurante_app_res_destroy(t_handshake_restaurante_app_res* response){
	free(response);
}

t_paquete* handshake_restaurante_app_res_empaquetar(t_handshake_restaurante_app_res* response){
	uint32_t size = handshake_restaurante_app_res_size(response);

	void* stream = malloc(size);
	memcpy(stream, &(response->code), sizeof(response->code));

	t_paquete* paquete = paquete_create(HANDSHAKE, size, stream);

	free(stream);
	return paquete;
}

t_handshake_restaurante_app_res* handshake_restaurante_app_res_desempaquetar(t_paquete* paquete){
	t_handshake_restaurante_app_res* response = malloc(sizeof(t_handshake_restaurante_app_res));

	void* stream = paquete->buffer->stream;
	memcpy(&(response->code), stream, sizeof(response->code));

	return response;
}

char* handshake_restaurante_app_res_to_string(t_handshake_restaurante_app_res* response){
	char* response_str = string_from_format("{op_code: %s, size: %d, data: { code: %s }}",
			"HANDSHAKE",
			handshake_restaurante_app_res_size(response),
			response->code == OK_ ? "OK" : "FAIL");

	return response_str;
}

uint32_t handshake_restaurante_app_res_size(t_handshake_restaurante_app_res* response){
	return sizeof(response->code);
}

