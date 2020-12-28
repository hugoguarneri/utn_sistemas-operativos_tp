#ifndef PEDIDO_DEFAULT_H_
#define PEDIDO_DEFAULT_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>


typedef struct {
	char* id_pedido;
	t_list* platos;
} t_pedido_default;


t_pedido_default* pedido_default_create(char* id_pedido);

void pedido_default_aniadir_plato(t_pedido_default* pedido, char* plato);

void pedido_default_destroy(t_pedido_default* pedido);


#endif /* PEDIDO_DEFAULT_H_ */
