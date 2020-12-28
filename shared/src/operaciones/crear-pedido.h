#ifndef CREAR_PEDIDO_H_
#define CREAR_PEDIDO_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"

t_paquete* crear_pedido_req_empaquetar();

char* crear_pedido_req_to_string();

typedef struct {
	char* id_pedido;
} t_crear_pedido_res;

t_crear_pedido_res* crear_pedido_res_create(char* id_pedido);

void crear_pedido_res_destroy(t_crear_pedido_res* response);

t_paquete* crear_pedido_res_empaquetar(t_crear_pedido_res* response);

t_crear_pedido_res* crear_pedido_res_desempaquetar(t_paquete* paquete);

char* crear_pedido_res_to_string(t_crear_pedido_res* response);

uint32_t crear_pedido_res_size(t_crear_pedido_res* response);

#endif /* CREAR_PEDIDO_H_ */
