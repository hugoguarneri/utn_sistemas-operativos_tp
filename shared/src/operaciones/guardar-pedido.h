#ifndef GUARDAR_PEDIDO_H_
#define GUARDAR_PEDIDO_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"


typedef struct {
	char* restaurante;
	char* id_pedido;
} t_guardar_pedido_req;

t_guardar_pedido_req* guardar_pedido_req_create(char* restaurante, char* id_pedido);

void guardar_pedido_req_destroy(t_guardar_pedido_req* request);

t_paquete* guardar_pedido_req_empaquetar(t_guardar_pedido_req* request);

t_guardar_pedido_req* guardar_pedido_req_desempaquetar(t_paquete* paquete);

uint32_t guardar_pedido_req_size(t_guardar_pedido_req* request);

char* guardar_pedido_req_to_string(t_guardar_pedido_req* request);


typedef struct {
	status_code code;
} t_guardar_pedido_res;

t_guardar_pedido_res* guardar_pedido_res_create(status_code code);

void guardar_pedido_res_destroy(t_guardar_pedido_res* response);

t_paquete* guardar_pedido_res_empaquetar(t_guardar_pedido_res* response);

t_guardar_pedido_res* guardar_pedido_res_desempaquetar(t_paquete* paquete);

char* guardar_pedido_res_to_string(t_guardar_pedido_res* response);

uint32_t guardar_pedido_res_size(t_guardar_pedido_res* response);

#endif /* GUARDAR_PEDIDO_H_ */
