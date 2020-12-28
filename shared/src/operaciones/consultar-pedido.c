#include "consultar-pedido.h"


t_consultar_pedido_req* consultar_pedido_req_create(char* id_pedido){
	t_consultar_pedido_req* request = malloc(sizeof(t_consultar_pedido_req));
	request->id_pedido = strdup(id_pedido);

	return request;
}

void consultar_pedido_req_destroy(t_consultar_pedido_req* request){
	free(request->id_pedido);
	free(request);
}

t_paquete* consultar_pedido_req_empaquetar(t_consultar_pedido_req* request){
	uint32_t size = consultar_pedido_req_size(request);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t id_pedido_length = strlen(request->id_pedido);
	memcpy(stream + bytes_escritos, &(id_pedido_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->id_pedido, id_pedido_length);
	bytes_escritos += id_pedido_length;

	t_paquete* paquete = paquete_create(CONSULTAR_PEDIDO, size, stream);

	free(stream);
	return paquete;
}

t_consultar_pedido_req* consultar_pedido_req_desempaquetar(t_paquete* paquete){
	t_consultar_pedido_req* request = malloc(sizeof(t_consultar_pedido_req));

    void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t id_pedido_length;
	memcpy(&(id_pedido_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->id_pedido = calloc(sizeof(char),id_pedido_length+1);
    memcpy(request->id_pedido, stream + bytes_leidos, id_pedido_length);
    bytes_leidos += id_pedido_length;

	return request;
}

char* consultar_pedido_req_to_string(t_consultar_pedido_req* request){
	char* request_str = string_from_format("{op_code: %s, size: %d, data: { id_pedido: %s }}",
			"CONSULTAR_PEDIDO",
			consultar_pedido_req_size(request),
			request->id_pedido);

	return request_str;
}

uint32_t consultar_pedido_req_size(t_consultar_pedido_req* request){
	return sizeof(uint32_t) + strlen(request->id_pedido);
}



t_consultar_pedido_res* consultar_pedido_res_create(char* restaurante, e_estado_pedido estado, t_list* estado_platos){
	t_consultar_pedido_res* response = malloc(sizeof(t_consultar_pedido_res));
	response->restaurante = strdup(restaurante);
	response->estado = estado;

	t_list* estado_platos_list = list_create();
	for(int i = 0; i < estado_platos->elements_count; i++){
		t_estado_plato* estado_plato = list_get(estado_platos, i);
		t_estado_plato* estado_plato_aux = estado_plato_create(
				string_duplicate(estado_plato->comida),
				estado_plato->cant_total,
				estado_plato->cant_lista);

		list_add(estado_platos_list, estado_plato_aux);
	}
	response->estado_platos = estado_platos_list;

	return response;
}

void consultar_pedido_res_destroy(t_consultar_pedido_res* response){
	list_destroy_and_destroy_elements(response->estado_platos, (void*)estado_plato_destroy);
	free(response->restaurante);
	free(response);
}

t_paquete* consultar_pedido_res_empaquetar(t_consultar_pedido_res* response){
	uint32_t size = consultar_pedido_res_size(response);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t restaurante_length = strlen(response->restaurante);
	memcpy(stream + bytes_escritos, &(restaurante_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, response->restaurante, restaurante_length);
	bytes_escritos += restaurante_length;


	memcpy(stream + bytes_escritos, &(response->estado), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);


	uint32_t estado_platos_elements = response->estado_platos->elements_count;
	memcpy(stream + bytes_escritos, &(estado_platos_elements), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	for(int i = 0; i < estado_platos_elements; i++){
		t_estado_plato* estado_plato = list_get(response->estado_platos, i);

		uint32_t comida_length = strlen(estado_plato->comida);
		memcpy(stream + bytes_escritos, &(comida_length), sizeof(uint32_t));
		bytes_escritos += sizeof(uint32_t);

		memcpy(stream + bytes_escritos, estado_plato->comida, comida_length);
		bytes_escritos += comida_length;


		memcpy(stream + bytes_escritos, &(estado_plato->cant_total), sizeof(uint32_t));
		bytes_escritos += sizeof(uint32_t);

		memcpy(stream + bytes_escritos, &(estado_plato->cant_lista), sizeof(uint32_t));
		bytes_escritos += sizeof(uint32_t);
	}

	t_paquete* paquete = paquete_create(CONSULTAR_PEDIDO, size, stream);

	free(stream);
	return paquete;
}

t_consultar_pedido_res* consultar_pedido_res_desempaquetar(t_paquete* paquete){
	t_consultar_pedido_res* response = malloc(sizeof(t_consultar_pedido_res));
	t_list* estado_platos = list_create();

    void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t restaurante_length = 0;
	memcpy(&(restaurante_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	response->restaurante = calloc(sizeof(char), restaurante_length+1);
    memcpy(response->restaurante, stream + bytes_leidos, restaurante_length);
    bytes_leidos += restaurante_length;

    memcpy(&(response->estado), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	uint32_t estado_platos_elements = 0;
	memcpy(&(estado_platos_elements), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	for(int i = 0; i < estado_platos_elements; i++){
		t_estado_plato* estado_plato;

		uint32_t comida_length = 0;
		memcpy(&(comida_length), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);

		char* comida = calloc(sizeof(char),comida_length+1);
	    memcpy(comida, stream + bytes_leidos, comida_length);
	    bytes_leidos += comida_length;

		uint32_t cant_total = 0;
		memcpy(&(cant_total), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);

		uint32_t cant_lista = 0;
		memcpy(&(cant_lista), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);

		estado_plato = estado_plato_create(comida, cant_total, cant_lista);
		list_add(estado_platos, estado_plato);
	}
	response->estado_platos = estado_platos;
	return response;
}

char* consultar_pedido_res_to_string(t_consultar_pedido_res* response){
	uint32_t size = consultar_pedido_res_size(response);

	char* estado_platos_str = string_new();
	for(int i = 0; i < response->estado_platos->elements_count; i++){
		t_estado_plato* estado_plato = list_get(response->estado_platos, i);

		string_append_with_format
		(&estado_platos_str, "%s, ", estado_plato_to_string(estado_plato));
	}
	estado_platos_str = string_substring(estado_platos_str, 0, strlen(estado_platos_str) -1);

	char* response_str = string_from_format("{op_code: %s, size: %d, data: \
			{ restaurante: %s, estado: %s, estado_platos: [%s] }}",
			"CONSULTAR_PEDIDO",
			size,
			response->restaurante,
			response->estado == 0 ? "PENDIENTE" : "CONFIRMADO",
			estado_platos_str);

	free(estado_platos_str);
	return response_str;
}

uint32_t consultar_pedido_res_size(t_consultar_pedido_res* response){
	uint32_t size = sizeof(uint32_t) + strlen(response->restaurante) +
					sizeof(uint32_t) +
					sizeof(uint32_t);

	for(int i = 0; i < response->estado_platos->elements_count; i++){
		size += sizeof(uint32_t);
		size += estado_plato_size(list_get(response->estado_platos, i));
	}
	return size;
}
