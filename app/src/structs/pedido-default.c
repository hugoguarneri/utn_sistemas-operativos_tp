#include "pedido-default.h"

t_pedido_default* pedido_default_create(char* id_pedido){
	t_pedido_default* pedido = malloc(sizeof(t_pedido_default));
	pedido->id_pedido = strdup(id_pedido);
	pedido->platos = list_create();
	return pedido;
}

void pedido_default_aniadir_plato(t_pedido_default* pedido, char* plato){
	list_add(pedido->platos, plato);
}

void pedido_default_destroy(t_pedido_default* pedido){
	list_destroy_and_destroy_elements(pedido->platos, free);
	free(pedido->id_pedido);
	free(pedido);
}

