#include "consultar-restaurantes.h"

t_paquete* consultar_restaurantes_req_empaquetar(){
	char* stream = string_new();
	t_paquete* paquete = paquete_create(CONSULTAR_RESTAURANTES, sizeof(stream), stream);

	free(stream);
	return paquete;
}

char* consultar_restaurantes_req_to_string(){
	uint32_t size = strlen("");
	char* request_str = string_from_format("{op_code: %s, size: %d}", "CONSULTAR_RESTAURANTES", size);

	return request_str;
}


t_consultar_restaurantes_res* consultar_restaurantes_res_create_from_list(t_list* nombres){
	t_consultar_restaurantes_res* response = malloc(sizeof(t_consultar_restaurantes_res));

	t_list* nombres_list = list_create();
	for(int i = 0; i < nombres->elements_count; i++){
		char* nombre = string_duplicate(list_get(nombres, i));
		list_add(nombres_list, nombre);
	}

	response->nombres = nombres_list;
	return response;
}

void consultar_restaurantes_res_destroy(t_consultar_restaurantes_res* response){
	list_destroy_and_destroy_elements(response->nombres, (void*) free);
	free(response);
}

t_paquete* consultar_restaurantes_res_empaquetar(t_consultar_restaurantes_res* response){
	uint32_t size = consultar_restaurantes_res_size(response);

	void* stream = malloc(size);
	int bytes_escritos = 0;

	uint32_t nombres_elements = response->nombres->elements_count;
	memcpy(stream + bytes_escritos, &(nombres_elements), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);

	for(int i = 0; i < nombres_elements; i++){
		char* nombre = list_get(response->nombres, i);

		uint32_t nombre_length = strlen(nombre);
		memcpy(stream + bytes_escritos, &(nombre_length), sizeof(uint32_t));
		bytes_escritos += sizeof(uint32_t);

		memcpy(stream + bytes_escritos, nombre, nombre_length);
		bytes_escritos += nombre_length;
	}

	t_paquete* paquete = paquete_create(CONSULTAR_RESTAURANTES, size, stream);

	free(stream);
	return paquete;
}

t_consultar_restaurantes_res* consultar_restaurantes_res_desempaquetar(t_paquete* paquete){
	t_consultar_restaurantes_res* response = malloc(sizeof(t_consultar_restaurantes_res));
	t_list* nombres = list_create();

    void* stream = paquete->buffer->stream;
	int bytes_leidos = 0;

	uint32_t nombres_elements;
	memcpy(&(nombres_elements), stream + bytes_leidos, sizeof(uint32_t));
	bytes_leidos += sizeof(uint32_t);

	for(int i = 0; i < nombres_elements; i++){
		uint32_t nombre_length;
		memcpy(&(nombre_length), stream + bytes_leidos, sizeof(uint32_t));
		bytes_leidos += sizeof(uint32_t);

		char* nombre = calloc(sizeof(char),nombre_length+1);
		memcpy(nombre, stream + bytes_leidos, nombre_length);
		bytes_leidos += nombre_length;

		list_add(nombres, nombre);
	}
	response->nombres = nombres;
	return response;

}

char* consultar_restaurantes_res_to_string(t_consultar_restaurantes_res* response){
	uint32_t size = consultar_restaurantes_res_size(response);
	char* stream = string_new();

	for(int i = 0; i < response->nombres->elements_count; i++){
		char* nombre = list_get(response->nombres, i);

		if(i == response->nombres->elements_count -1)
			string_append_with_format(&stream, "%s", nombre);
		else
			string_append_with_format(&stream, "%s, ", nombre);
	}

	char* request_str = string_from_format("{op_code: %s, size: %d, data: [%s] }",
			"CONSULTAR_RESTAURANTES", size, stream);
	return request_str;
}

uint32_t consultar_restaurantes_res_size(t_consultar_restaurantes_res* response){
	uint32_t size = sizeof(uint32_t);

	for(int i = 0; i < response->nombres->elements_count; i++){
		size += sizeof(uint32_t);
		size += strlen(list_get(response->nombres, i));
	}
	return size;
}

