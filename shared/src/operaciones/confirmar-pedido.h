#ifndef CONFIRMAR_PEDIDO_H_
#define CONFIRMAR_PEDIDO_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"


typedef struct {
	char* id_pedido;
	/*opcional*/
	char* restaurante;
} t_confirmar_pedido_req;

t_confirmar_pedido_req* confirmar_pedido_req_create(char* id_pedido, char* restaurante);

void confirmar_pedido_req_destroy(t_confirmar_pedido_req* request);

t_paquete* confirmar_pedido_req_empaquetar(t_confirmar_pedido_req* request);

t_confirmar_pedido_req* confirmar_pedido_req_desempaquetar(t_paquete* paquete);

char* confirmar_pedido_req_to_string(t_confirmar_pedido_req* request);

void confirmar_pedido_req_to_log(char* accion, t_confirmar_pedido_req* request);

uint32_t confirmar_pedido_req_size(t_confirmar_pedido_req* request);

typedef struct {
	status_code code;
} t_confirmar_pedido_res;

t_confirmar_pedido_res* confirmar_pedido_res_create(status_code code);

void confirmar_pedido_res_destroy(t_confirmar_pedido_res* response);

t_paquete* confirmar_pedido_res_empaquetar(t_confirmar_pedido_res* response);

t_confirmar_pedido_res* confirmar_pedido_res_desempaquetar(t_paquete* paquete);

char* confirmar_pedido_res_to_string(t_confirmar_pedido_res* response);

void confirmar_pedido_res_to_log(char* accion, t_confirmar_pedido_res* response);

uint32_t confirmar_pedido_res_size(t_confirmar_pedido_res* response);

#endif /* CONFIRMAR_PEDIDO_H_ */
