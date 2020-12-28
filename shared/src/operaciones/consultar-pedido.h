#ifndef CONSULTAR_PEDIDO_H_
#define CONSULTAR_PEDIDO_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include "structs/estado-plato.h"
#include "structs/estado-pedido.h"

#include "opcode.h"
#include "paquete.h"


typedef struct {
	char* id_pedido;
} t_consultar_pedido_req;

t_consultar_pedido_req* consultar_pedido_req_create(char* id_pedido);

void consultar_pedido_req_destroy(t_consultar_pedido_req* request);

t_paquete* consultar_pedido_req_empaquetar(t_consultar_pedido_req* request);

t_consultar_pedido_req* consultar_pedido_req_desempaquetar(t_paquete* paquete);

char* consultar_pedido_req_to_string(t_consultar_pedido_req* request);

uint32_t consultar_pedido_req_size(t_consultar_pedido_req* request);


typedef struct {
	char* restaurante;
	e_estado_pedido estado;
	t_list* estado_platos;
} t_consultar_pedido_res;

t_consultar_pedido_res* consultar_pedido_res_create(char* restaurante, e_estado_pedido estado, t_list* estado_platos);

void consultar_pedido_res_destroy(t_consultar_pedido_res* response);

t_paquete* consultar_pedido_res_empaquetar(t_consultar_pedido_res* response);

t_consultar_pedido_res* consultar_pedido_res_desempaquetar(t_paquete* paquete);

char* consultar_pedido_res_to_string(t_consultar_pedido_res* response);

uint32_t consultar_pedido_res_size(t_consultar_pedido_res* response);

#endif /* CONSULTAR_PEDIDO_H_ */
