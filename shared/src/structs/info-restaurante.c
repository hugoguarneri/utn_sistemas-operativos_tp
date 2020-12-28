#include "info-restaurante.h"

t_info_restaurante* info_restaurante_create(char* ip, char* puerto, char* nombre, uint32_t pos_x, uint32_t pos_y){
	t_info_restaurante* info_restaurante = malloc(sizeof(t_info_restaurante));
	info_restaurante->pos_x = pos_x;
	info_restaurante->pos_y = pos_y;
	info_restaurante->ip = strdup(ip);
	info_restaurante->puerto = strdup(puerto);
	info_restaurante->nombre = strdup(nombre);

	return info_restaurante;
}

void info_restaurante_destroy(t_info_restaurante* info_restaurante){
	free(info_restaurante->ip);
	free(info_restaurante->puerto);
	free(info_restaurante->nombre);

	free(info_restaurante);
}

char* info_restaurante_to_string(t_info_restaurante* info_restaurante){
	char* str = string_from_format("{ ip: %s, puerto: %s, nombre: %s, pos_x: %d, pos_y: %d }",
			info_restaurante->ip, info_restaurante->puerto, info_restaurante->nombre,info_restaurante->pos_x, info_restaurante->pos_y);
	return str;
}
