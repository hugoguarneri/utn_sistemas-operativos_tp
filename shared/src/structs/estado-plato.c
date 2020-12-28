#include "estado-plato.h"

t_estado_plato* estado_plato_create(char* comida, uint32_t cant_total, uint32_t cant_lista){
	t_estado_plato* estado_plato = malloc(sizeof(t_estado_plato));
	estado_plato->comida = strdup(comida);
	estado_plato->cant_total = cant_total;
	estado_plato->cant_lista = cant_lista;

	return estado_plato;
}

void estado_plato_destroy(t_estado_plato* estado_plato){
	free(estado_plato->comida);
	free(estado_plato);
}

char* estado_plato_to_string(t_estado_plato* estado_plato){
	char* estado_plato_str = string_new();

	string_append_with_format(&estado_plato_str, "{%s, ", estado_plato->comida);
	string_append_with_format(&estado_plato_str, "%d, ", estado_plato->cant_total);
	string_append_with_format(&estado_plato_str, "%d}", estado_plato->cant_lista);

	return estado_plato_str;
}

uint32_t estado_plato_size(t_estado_plato* estado_plato){
	uint32_t size = sizeof(estado_plato->cant_total) +
					sizeof(estado_plato->cant_lista) +
					strlen(estado_plato->comida);
	return size;
}
