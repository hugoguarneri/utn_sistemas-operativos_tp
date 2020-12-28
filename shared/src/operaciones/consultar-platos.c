#include "consultar-platos.h"

t_consultar_platos_req* consultar_platos_req_create(char *restaurante) {
	t_consultar_platos_req* result = malloc(sizeof(t_consultar_platos_req));
	result->restaurante = strdup(restaurante != NULL ? restaurante : "");
	return result;
}

void consultar_platos_req_destroy(t_consultar_platos_req *request) {
	free(request->restaurante);
	free(request);
}

t_paquete* consultar_platos_req_empaquetar(t_consultar_platos_req *request) {
	uint32_t size = consultar_platos_req_size(request);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t restaurante_length = strlen(request->restaurante);
	memcpy(stream + bytes_escritos, &(restaurante_length), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	memcpy(stream + bytes_escritos, request->restaurante, restaurante_length);
	bytes_escritos += restaurante_length;

	t_paquete* paquete = paquete_create(CONSULTAR_PLATOS, size, stream);

	free(stream);
	return paquete;
}

t_consultar_platos_req* consultar_platos_req_desempaquetar(t_paquete *paquete) {
	t_consultar_platos_req* request = malloc(sizeof(t_consultar_platos_req));

    void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t restaurante_length;
	memcpy(&(restaurante_length), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	request->restaurante = calloc(sizeof(char),restaurante_length+1);
	memcpy(request->restaurante, stream + bytes_leidos, restaurante_length);
	bytes_leidos += restaurante_length;

	return request;
}

char* consultar_platos_req_to_string(t_consultar_platos_req* request) {
	uint32_t size = consultar_platos_req_size(request);
	char *request_str = string_from_format("{op_code: %s, size: %d, data: { restaurante: %s }}",
			"CONSULTAR_PLATOS", size, request->restaurante);
	return request_str;
}

uint32_t consultar_platos_req_size(t_consultar_platos_req* request){
	return sizeof(uint32_t)+ strlen(request->restaurante);
}




t_consultar_platos_res* consultar_platos_res_create(t_list* platos) {
	t_consultar_platos_res* response = calloc(sizeof(t_consultar_platos_res), 1);

	t_list* platos_aux = list_create();
	for(int i = 0; i < platos->elements_count; i++){
		char* plato = list_get(platos, i);
		list_add(platos_aux, strdup(plato));
	}

	response->platos = platos_aux;
	return response;
}

void consultar_platos_res_destroy(t_consultar_platos_res* response) {
	list_destroy_and_destroy_elements(response->platos, (void*) free);
	free(response);
}

t_paquete* consultar_platos_res_empaquetar(t_consultar_platos_res *response) {
	uint32_t size = sizeof(uint32_t);

	void sumar_tamanios_plato(char* plato) {
		size += sizeof(uint32_t) + strlen(plato);
	}
	list_iterate(response->platos, (void*) sumar_tamanios_plato);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t platos_elements = response->platos->elements_count;
	memcpy(stream + bytes_escritos, &(platos_elements), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	for(int i = 0; i < platos_elements; i++){
		char* plato = list_get(response->platos, i);

		uint32_t plato_length = strlen(plato);
		memcpy(stream + bytes_escritos, &(plato_length), sizeof(uint32_t));
		bytes_escritos += sizeof(uint32_t);

		memcpy(stream + bytes_escritos, plato, plato_length);
		bytes_escritos += plato_length;
	}

	t_paquete *paquete = paquete_create(CONSULTAR_PLATOS, size, stream);

	free(stream);
	return paquete;

}

t_consultar_platos_res* consultar_platos_res_desempaquetar(t_paquete *paquete) {
	t_consultar_platos_res *response = malloc(sizeof(t_consultar_platos_res));

	void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t cantidad = 0;
	memcpy(&(cantidad), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);
	response->platos = list_create();

	for (int i = 0; i < cantidad; i++) {

		uint32_t plato_length = 0;
		memcpy(&(plato_length), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);

		char* plato = calloc(sizeof(char),plato_length+1);
		memcpy(plato, stream + bytes_leidos, plato_length);
		bytes_leidos += plato_length;

		list_add(response->platos, plato);
	}
	return response;
}

char* consultar_platos_res_to_string(t_consultar_platos_res *response) {
	char* platos = string_new();
	void dump_platos(char *plato) {
		string_append_with_format(&platos, "%s,", plato);
	}
	list_iterate(response->platos, (void*) dump_platos);
	string_substring_until(platos, strlen(platos)-1);
	platos = string_substring_until(platos, strlen(platos) -1);

	uint32_t size = consultar_platos_res_size(response);

	char *request_str = string_from_format("{op_code: %s, size: %d, data: { platos:[%s] }}", "CONSULTAR_PLATOS", size, platos);
	return request_str;

}

uint32_t consultar_platos_res_size(t_consultar_platos_res *response){
	uint32_t size = sizeof(uint32_t);

	for(int i = 0; i < response->platos->elements_count; i++){
		size += sizeof(uint32_t);
		size += strlen(list_get(response->platos, i));
	}
	return size;
}

t_paquete* consutar_platos_res_data_empaquetar(char *response) {
	t_dictionary* dictionary = dictionary_create();
	char **lines = string_split(response, "\n");

	void add_cofiguration(char *line) {
		if (!string_starts_with(line, "#")) {
			char **keyAndValue = string_n_split(line, 2, "=");
			dictionary_put(dictionary, keyAndValue[0], keyAndValue[1]);
			free(keyAndValue[0]);
			free(keyAndValue);
		}
	}
	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);
	free(lines);
	free(response);

	t_consultar_platos_res *aux = calloc(sizeof(t_consultar_platos_res), 1);
	char** array_platos = dictionary_get_string_array_value(dictionary, "PLATOS");
	aux->platos = list_create();
	int i = 0;
	while (array_platos[i] != NULL) {
		char *plato = string_duplicate(array_platos[i]);
		list_add(aux->platos, plato);
		i++;
	}

	t_paquete *paquete = consultar_platos_res_empaquetar(aux);
	consultar_platos_res_destroy(aux);
	free_string_array(array_platos);
	return paquete;
}

