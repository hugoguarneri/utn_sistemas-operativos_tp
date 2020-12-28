#include "consultar-info-cliente.h"

t_paquete* consultar_info_cliente_req_empaquetar(){
	char* stream = string_new();
	t_paquete* paquete = paquete_create(HANDSHAKE, sizeof(stream), stream);

	free(stream);
	return paquete;
}

char* consultar_info_cliente_req_to_string(){
	uint32_t size = strlen("");
	char* request_str = string_from_format("{op_code: %s, size: %d}", "HANDSHAKE", size);

	return request_str;
}


t_paquete* consultar_info_cliente_res_empaquetar(t_info_cliente* response){
	uint32_t size = consultar_info_cliente_res_size(response);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t puerto_length = strlen(response->puerto);
	memcpy(stream, &(puerto_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, response->puerto, puerto_length);
	bytes_escritos += puerto_length;

	memcpy(stream + bytes_escritos, &(response->pos_x), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, &(response->pos_y), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	uint32_t ip_length = strlen(response->ip);
	memcpy(stream + bytes_escritos, &(ip_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, response->ip, ip_length);
	bytes_escritos += ip_length;

	uint32_t id_cliente_length = strlen(response->id_cliente);
	memcpy(stream + bytes_escritos, &(id_cliente_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, response->id_cliente, id_cliente_length);
	bytes_escritos += id_cliente_length;

	t_paquete* paquete = paquete_create(HANDSHAKE, size, stream);

	free(stream);
	return paquete;
}

t_info_cliente* consultar_info_cliente_res_desempaquetar(t_paquete* paquete){
	t_info_cliente* response = malloc(sizeof(t_info_cliente));

    void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t puerto_length;
	memcpy(&(puerto_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	response->puerto = calloc(sizeof(char),puerto_length+1);
    memcpy(response->puerto, stream + bytes_leidos, puerto_length);
    bytes_leidos += puerto_length;

	memcpy(&(response->pos_x), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	memcpy(&(response->pos_y), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	uint32_t ip_length;
	memcpy(&(ip_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	response->ip = calloc(sizeof(char),ip_length+1);
    memcpy(response->ip, stream + bytes_leidos, ip_length);
    bytes_leidos += ip_length;

	uint32_t id_cliente_length;
	memcpy(&(id_cliente_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	response->id_cliente = calloc(sizeof(char),id_cliente_length+1);
    memcpy(response->id_cliente, stream + bytes_leidos, id_cliente_length);
    bytes_leidos += id_cliente_length;

	return response;
}

char* consultar_info_cliente_res_to_string(t_info_cliente* response){
	char* request_str = string_from_format("{op_code: %s, size: %d, data: { ip: %s, puerto: %s, id_cliente: %s, pos_x: %d, pos_y: %d }}",
			"HANDSHAKE", consultar_info_cliente_res_size(response),
			response->ip, response->puerto, response->id_cliente, response->pos_x, response->pos_y);

	return request_str;
}

uint32_t consultar_info_cliente_res_size(t_info_cliente* response){
	return sizeof(uint32_t) + sizeof(uint32_t) +
		   sizeof(uint32_t) + strlen(response->ip) +
		   sizeof(uint32_t) + strlen(response->puerto) +
		   sizeof(uint32_t) + strlen(response->id_cliente);
}
