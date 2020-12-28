#include "obtener-restaurante.h"

t_obtener_restaurante_req* obtener_restaurante_req_create(char *restaurante) {
	t_obtener_restaurante_req *result = NULL;
	result = calloc(1, sizeof(t_obtener_restaurante_req));
	result->restaurante = string_duplicate(restaurante);
	return result;
}

void obtener_restaurante_req_destroy(t_obtener_restaurante_req *request) {
	if (request != NULL) {
		if (request->restaurante != NULL)
			free(request->restaurante);
		free(request);
	}
}

t_paquete* obtener_restaurante_req_empaquetar(t_obtener_restaurante_req *request) {
	t_paquete *paquete = NULL;
	paquete = calloc(1, sizeof(t_paquete));
	paquete->codigo_operacion = OBTENER_RESTAURANTE;
	paquete->buffer = buffer_create(strlen(request->restaurante), request->restaurante);
	obtener_restaurante_req_destroy(request);
	return paquete;
}

t_obtener_restaurante_req* obtener_restaurante_req_desempaquetar(t_paquete *paquete) {
	t_obtener_restaurante_req *result = NULL;
	result = calloc(1, sizeof(t_obtener_restaurante_req));
	result->restaurante = string_ncopy_from(paquete->buffer->size, paquete->buffer->stream);
	return result;
}

char* obtener_restaurante_req_to_string(t_obtener_restaurante_req *request) {
	uint32_t size = strlen(request->restaurante);

	char *request_str = string_from_format("{op_code: %s, size: %d, t_obtener_restaurante_req: { restaurante: %s }}",
			"OBTENER_RESTAURANTE", size, request->restaurante);

	return request_str;
}

t_receta_precio* receta_precio_create(char *receta, char *precio) {
	t_receta_precio *result = NULL;
	result = calloc(sizeof(t_receta_precio), 1);
	result->receta = string_duplicate(receta);
	result->precio = atoi(precio);
	return result;
}

void receta_precio_destroy(t_receta_precio *receta_precio) {
	free(receta_precio->receta);
	free(receta_precio);
}

int receta_precio_size(t_receta_precio *receta_precio) {
	uint32_t receta_length = strlen(receta_precio->receta);
	uint32_t size = sizeof(uint32_t) + receta_length + sizeof(uint32_t);
	return size;
}

t_obtener_restaurante_res* obtener_restaurante_res_create(t_list *afinidad, uint32_t pos_x, uint32_t pos_y,
		t_list *receta_precio, uint32_t cant_hornos, uint32_t cant_pedidos, uint32_t cant_cocineros) {
	t_obtener_restaurante_res *aux = calloc(sizeof(t_obtener_restaurante_res), 1);
	aux->afinidad = afinidad;
	aux->pos_x = pos_x;
	aux->pos_y = pos_y;
	aux->receta_precio = receta_precio;
	aux->cant_hornos = cant_hornos;
	aux->cant_pedidos = cant_pedidos;
	aux->cant_cocineros = cant_cocineros;
	return aux;
}

void obtener_restaurante_res_destroy(t_obtener_restaurante_res *response) {
	list_destroy_and_destroy_elements(response->afinidad, (void*) free);
	list_destroy_and_destroy_elements(response->receta_precio, (void*) receta_precio_destroy);
	free(response);
}

t_paquete* obtener_restaurante_res_data_empaquetar(char *response, uint32_t cantidad_pedidos) {
	t_dictionary *dictionary = dictionary_create();
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

	t_obtener_restaurante_res *aux = calloc(sizeof(t_obtener_restaurante_res), 1);
	aux->cant_hornos = dictionary_get_int_value(dictionary, "CANTIDAD_HORNOS");
	aux->cant_pedidos = cantidad_pedidos;
	aux->cant_cocineros = dictionary_get_int_value(dictionary, "CANTIDAD_COCINEROS");
	dictionary_get_position_value(dictionary, "POSICION", &(aux->pos_x), &(aux->pos_y));
	char **array_afinidades = dictionary_get_string_array_value(dictionary, "AFINIDAD_COCINEROS");
	char **array_platos = dictionary_get_string_array_value(dictionary, "PLATOS");
	char **array_precio_platos = dictionary_get_string_array_value(dictionary, "PRECIO_PLATOS");

	aux->afinidad = list_create();
	int i = 0;
	while (array_afinidades[i] != NULL) {
		char *afinidad = string_duplicate(array_afinidades[i]);
		list_add(aux->afinidad, afinidad);
		i++;
	}

	aux->receta_precio = list_create();
	i = 0;
	while (array_platos[i] != NULL) {
		list_add(aux->receta_precio, receta_precio_create(string_duplicate(array_platos[i]), array_precio_platos[i]));
		i++;
	}
	t_paquete *paquete = obtener_restaurante_res_empaquetar(aux);
	obtener_restaurante_res_destroy(aux);
	free_string_array(array_afinidades);
	free_string_array(array_platos);
	free_string_array(array_precio_platos);
	return paquete;
}

t_paquete* obtener_restaurante_res_empaquetar(t_obtener_restaurante_res *response) {
	uint32_t recetas_precios_size = 0;
	uint32_t afinidad_size = 0;

	log_info_message("Empaquetando", obtener_restaurante_res_to_string(response));

	void sumar_tamanios_receta(t_receta_precio *value) {
		recetas_precios_size += receta_precio_size(value);
	}
	list_iterate(response->receta_precio, (void*) sumar_tamanios_receta);

	void sumar_tamanios_afinidad(char *value) {
		afinidad_size += sizeof(uint32_t) + strlen(value);
	}
	list_iterate(response->afinidad, (void*) sumar_tamanios_afinidad);

	// Agrego uno para el tamaño general
	recetas_precios_size += sizeof(uint32_t);
	afinidad_size += sizeof(uint32_t);

	uint32_t size = afinidad_size + sizeof(uint32_t) + sizeof(uint32_t) + recetas_precios_size + sizeof(uint32_t)
			+ sizeof(uint32_t) + sizeof(uint32_t);

	void *stream = malloc(size);
	int bytes_escritos = 0;

	// AFINIDAD
	int lsize = list_size(response->afinidad);
	memcpy(stream + bytes_escritos, &lsize, sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	void afinidad_to_stream(char *afinidad) {
		int length = strlen(afinidad);
		memcpy(stream + bytes_escritos, &length, sizeof(uint32_t));
		bytes_escritos += sizeof(uint32_t);
		memcpy(stream + bytes_escritos, afinidad, length);
		bytes_escritos += length;
	}
	list_iterate(response->afinidad, (void*) afinidad_to_stream);

	// POSICION
	memcpy(stream + bytes_escritos, &response->pos_x, sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);
	memcpy(stream + bytes_escritos, &response->pos_y, sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	// RECETA_PRECIO
	lsize = list_size(response->receta_precio);
	memcpy(stream + bytes_escritos, &lsize, sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	void receta_precio_to_stream(t_receta_precio *receta_precio) {
		int length = strlen(receta_precio->receta);
		memcpy(stream + bytes_escritos, &length, sizeof(uint32_t));
		bytes_escritos += sizeof(uint32_t);
		memcpy(stream + bytes_escritos, receta_precio->receta, length);
		bytes_escritos += length;
		memcpy(stream + bytes_escritos, &receta_precio->precio, sizeof(uint32_t));
		bytes_escritos += sizeof(uint32_t);
	}
	list_iterate(response->receta_precio, (void*) receta_precio_to_stream);

	// CANTIDAD_HORNOS
	memcpy(stream + bytes_escritos, &response->cant_hornos, sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	// CANTIDAD_PEDIDOS
	memcpy(stream + bytes_escritos, &response->cant_pedidos, sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	// CANTIDAD_HORNOS
	memcpy(stream + bytes_escritos, &response->cant_cocineros, sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	// TODO ver si el codigo de mensaje tendria que ser el mismo u otro
	t_paquete *paquete = paquete_create(OBTENER_RESTAURANTE, size, stream);

	free(stream);
	return paquete;

}

t_obtener_restaurante_res* obtener_restaurante_res_desempaquetar(t_paquete *paquete) {
	t_obtener_restaurante_res *response = malloc(sizeof(t_obtener_restaurante_res));
	int cantidad = 0;
	void *stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	memcpy(&(cantidad), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);
	response->afinidad = list_create();
	for (int i = 0; i < cantidad; i++) {
		int length = 0;
		memcpy(&(length), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);
		char *afinidad = strndup(stream + bytes_leidos, length);
		bytes_leidos += length;
		list_add(response->afinidad, afinidad);
	}

	memcpy(&(response->pos_x), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	memcpy(&(response->pos_y), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	memcpy(&(cantidad), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);
	response->receta_precio = list_create();
	for (int i = 0; i < cantidad; i++) {
		int length = 0;
		t_receta_precio *receta_precio = calloc(sizeof(t_receta_precio), 1);
		memcpy(&(length), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);
		receta_precio->receta = strndup(stream + bytes_leidos, length);
		bytes_leidos += length;
		memcpy(&(receta_precio->precio), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);
		list_add(response->receta_precio, receta_precio);
	}

	memcpy(&(response->cant_hornos), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	memcpy(&(response->cant_pedidos), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	memcpy(&(response->cant_cocineros), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	return response;
}

char* obtener_restaurante_res_to_string(t_obtener_restaurante_res *response) {
	char *afinidades = string_new();
	void dump_afinidades(char *afinidad) {
		string_append_with_format(&afinidades, "%s,", afinidad);
	}
	list_iterate(response->afinidad, (void*) dump_afinidades);

	char *recetas_precios = string_new();
	void dump_receta_precio(t_receta_precio *receta_precio) {
		string_append_with_format(&recetas_precios, "{%s, %d},", receta_precio->receta, receta_precio->precio);
	}
	list_iterate(response->receta_precio, (void*) dump_receta_precio);

	char *result = string_from_format(
			"{afinidad:[%s], posicion:[%d,%d], receta_precio:[%s], cant_hornos:%d, cant_pedidos:%d, cant_cocineros:%d", afinidades,
			response->pos_x, response->pos_y, recetas_precios, response->cant_hornos, response->cant_pedidos, response->cant_cocineros);
	return result;
}
