#include "crear-pedido.h"

t_paquete* crear_pedido_req_empaquetar(){
	char* stream = string_new();
	t_paquete* paquete = paquete_create(CREAR_PEDIDO, sizeof(stream), stream);

	free(stream);
	return paquete;
}

char* crear_pedido_req_to_string(){
	return string_from_format("{op_code: %s, size: %s}", "CREAR_PEDIDO", "");
}


t_crear_pedido_res* crear_pedido_res_create(char* id_pedido){
	t_crear_pedido_res* response = malloc(sizeof(t_crear_pedido_res));
	response->id_pedido = strdup(id_pedido);

	return response;
}

void crear_pedido_res_destroy(t_crear_pedido_res* response){
	free(response->id_pedido);
	free(response);
}

t_paquete* crear_pedido_res_empaquetar(t_crear_pedido_res* response){
	uint32_t size =  crear_pedido_res_size(response);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t id_pedido_length = strlen(response->id_pedido);
	memcpy(stream + bytes_escritos, &(id_pedido_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, response->id_pedido, id_pedido_length);
	bytes_escritos += id_pedido_length;

	t_paquete* paquete = paquete_create(CREAR_PEDIDO, size, stream);

	free(stream);
	return paquete;
}

t_crear_pedido_res* crear_pedido_res_desempaquetar(t_paquete* paquete){
	t_crear_pedido_res* response = malloc(sizeof(t_crear_pedido_res));

    void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t id_pedido_length;
	memcpy(&(id_pedido_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	response->id_pedido = calloc(sizeof(char),id_pedido_length+1);
	memcpy(response->id_pedido, stream + bytes_leidos, id_pedido_length);
    bytes_leidos += id_pedido_length;

	return response;
}

char* crear_pedido_res_to_string(t_crear_pedido_res* response){
	char* response_str = string_from_format("{op_code: %s, size: %d, data: { id_pedido: %s }}",
			"CREAR_PEDIDO",
			crear_pedido_res_size(response),
			response->id_pedido);

	return response_str;
}

uint32_t crear_pedido_res_size(t_crear_pedido_res* response){
	return sizeof(uint32_t) + strlen(response->id_pedido);
}

