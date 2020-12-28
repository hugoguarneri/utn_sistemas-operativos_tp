#include "info-cliente.h"

t_info_cliente* info_cliente_create(char* ip, char* puerto, char* id_cliente, uint32_t pos_x, uint32_t pos_y){
	t_info_cliente* info_cliente = malloc(sizeof(t_info_cliente));
	info_cliente->pos_x = pos_x;
	info_cliente->pos_y = pos_y;
	info_cliente->ip = strdup(ip);
	info_cliente->puerto = strdup(puerto);
	info_cliente->id_cliente = strdup(id_cliente);

	return info_cliente;
}

void info_cliente_destroy(t_info_cliente* info_cliente){
	free(info_cliente->ip);
	free(info_cliente->puerto);
	free(info_cliente->id_cliente);

	free(info_cliente);
}
