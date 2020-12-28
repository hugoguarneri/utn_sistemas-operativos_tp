#include "obtener-receta.h"

t_obtener_receta_req* obtener_receta_req_create(char *receta) {
	t_obtener_receta_req *request = calloc(sizeof(t_obtener_receta_req), 1);
	request->receta = string_duplicate(receta);
	return request;
}

void obtener_receta_req_destroy(t_obtener_receta_req *request) {
	free(request->receta);
	free(request);
}

t_paquete* obtener_receta_req_empaquetar(t_obtener_receta_req *request) {
	t_paquete *paquete = NULL;
	paquete = calloc(1, sizeof(t_paquete));
	paquete->codigo_operacion = OBTENER_RECETA;
	paquete->buffer = buffer_create(strlen(request->receta), request->receta);
	obtener_receta_req_destroy(request);
	return paquete;
}

t_obtener_receta_req* obtener_receta_req_desempaquetar(t_paquete *paquete) {
	t_obtener_receta_req *result = NULL;
	result = calloc(sizeof(t_obtener_receta_req), 1);
	result->receta = string_ncopy_from(paquete->buffer->size, paquete->buffer->stream);
	return result;
}

char* obtener_receta_req_to_string(t_obtener_receta_req *request) {
	uint32_t size = strlen(request->receta);
	char *request_str = string_from_format("{op_code: %s, size: %d, payload: { receta: %s }}", "OBTENER_RECETA", size,
			request->receta);
	return request_str;
}

t_paso_tiempo* paso_tiempo_create(char *receta, char *tiempo) {
	t_paso_tiempo *result = calloc(sizeof(t_paso_tiempo), 1);
	result->paso = string_duplicate(receta);
	result->tiempo = atoi(tiempo);
	return result;
}

void paso_tiempo_destroy(t_paso_tiempo *paso_tiempo) {
	free(paso_tiempo->paso);
	free(paso_tiempo);
}

int paso_tiempo_size(t_paso_tiempo *paso_tiempo) {
	uint32_t length = strlen(paso_tiempo->paso);
	uint32_t size = sizeof(uint32_t) + length + sizeof(uint32_t);
	return size;
}

void obtener_receta_resp_destroy(t_obtener_receta_resp *response) {
	list_destroy_and_destroy_elements(response->pasos_tiempo, (void*) paso_tiempo_destroy);
	free(response);
}

t_paquete* obtener_receta_resp_data_empaquetar(char *data) {

	t_dictionary *dictionary = dictionary_create();
	char **lines = string_split(data, "\n");

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
	free(data);

	t_obtener_receta_resp *aux = calloc(sizeof(t_obtener_receta_resp), 1);
	char **array_pasos = dictionary_get_string_array_value(dictionary, "PASOS");
	char **array_tiempo = dictionary_get_string_array_value(dictionary, "TIEMPO_PASOS");

	aux->pasos_tiempo = list_create();
	int i = 0;
	while (array_pasos[i] != NULL) {
		t_paso_tiempo *item = paso_tiempo_create(array_pasos[i], array_tiempo[i]);
		list_add(aux->pasos_tiempo, item);
		i++;
	}

	t_paquete *paquete = obtener_receta_resp_empaquetar(aux);
	obtener_receta_resp_destroy(aux);
	free_string_array(array_pasos);
	free_string_array(array_tiempo);
	return paquete;
}

t_paquete* obtener_receta_resp_empaquetar(t_obtener_receta_resp *response) {

	uint32_t size = 0;
	void sumar_pasos_tiempo(t_paso_tiempo *item) {
		size += paso_tiempo_size(item);
	}
	list_iterate(response->pasos_tiempo, (void*) sumar_pasos_tiempo);

	// Agrego uno para el tamaño general
	size += sizeof(uint32_t);

	void *stream = malloc(size);
	int bytes_escritos = 0;

	// AFINIDAD
	int lsize = list_size(response->pasos_tiempo);
	memcpy(stream + bytes_escritos, &lsize, sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	void paso_tiempo_to_stream(t_paso_tiempo *paso_tiempo) {
		int length = strlen(paso_tiempo->paso);
		memcpy(stream + bytes_escritos, &length, sizeof(uint32_t));
		bytes_escritos += sizeof(uint32_t);
		memcpy(stream + bytes_escritos, paso_tiempo->paso, length);
		bytes_escritos += length;
		memcpy(stream + bytes_escritos, &paso_tiempo->tiempo, sizeof(uint32_t));
		bytes_escritos += sizeof(uint32_t);
	}
	list_iterate(response->pasos_tiempo, (void*) paso_tiempo_to_stream);

	// TODO ver si el codigo de mensaje tendria que ser el mismo u otro
	t_paquete *paquete = paquete_create(OBTENER_RECETA, size, stream);

	free(stream);
	return paquete;
}

t_obtener_receta_resp* obtener_receta_resp_desempaquetar(t_paquete *paquete) {

	t_obtener_receta_resp *response = malloc(sizeof(t_obtener_receta_resp));
	int cantidad = 0;
	void *stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	memcpy(&(cantidad), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);
	response->pasos_tiempo = list_create();
	for (int i = 0; i < cantidad; i++) {
		int length = 0;
		t_paso_tiempo* paso_tiempo = calloc(sizeof(t_paso_tiempo), 1);
		memcpy(&(length), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);
		paso_tiempo->paso = strndup(stream + bytes_leidos, length);
		bytes_leidos += length;
		memcpy(&(paso_tiempo->tiempo), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);
		list_add(response->pasos_tiempo, paso_tiempo);
	}

	return response;
}

char* obtener_receta_resp_to_string(t_obtener_receta_resp *response) {
	uint32_t size = 0;
	void sumar_pasos_tiempo(t_paso_tiempo *item) {
		size += paso_tiempo_size(item);
	}
	list_iterate(response->pasos_tiempo, (void *) sumar_pasos_tiempo);

	char* pasos_tiempos = string_new();
	void dump_paso_tiempo(t_paso_tiempo* item){
		string_append_with_format(&pasos_tiempos, "{paso: %s, tiempo: %d},", item->paso, item->tiempo);
	}
	list_iterate(response->pasos_tiempo, (void *)dump_paso_tiempo);

	char *request_str = string_from_format("{op_code: %s, size: %d, payload: [ %s ]}", "OBTENER_RECETA", size, pasos_tiempos);
	return request_str;
}
