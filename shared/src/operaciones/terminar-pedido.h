#ifndef TERMINAR_PEDIDO_H_
#define TERMINAR_PEDIDO_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include "opcode.h"
#include "paquete.h"

//TODO: esto es igual que confirmar-pedido

typedef struct {
	char* restaurante;
	char* id_pedido;
} t_terminar_pedido_req;

t_terminar_pedido_req* terminar_pedido_req_create(char* restaurante, char* id_pedido);

void terminar_pedido_req_destroy(t_terminar_pedido_req* request);

t_paquete* terminar_pedido_req_empaquetar(t_terminar_pedido_req* request);

t_terminar_pedido_req* terminar_pedido_req_desempaquetar(t_paquete* paquete);

char* terminar_pedido_req_to_string(t_terminar_pedido_req* request);

uint32_t terminar_pedido_req_size(t_terminar_pedido_req *request);

typedef struct {
	status_code code;
} t_terminar_pedido_res;

t_terminar_pedido_res* terminar_pedido_res_create(status_code code);

void terminar_pedido_res_destroy(t_terminar_pedido_res* response);

t_paquete* terminar_pedido_res_empaquetar(t_terminar_pedido_res* response);

t_terminar_pedido_res* terminar_pedido_res_desempaquetar(t_paquete* paquete);

char*  terminar_pedido_res_to_string(t_terminar_pedido_res* response);

uint32_t terminar_pedido_res_size(t_terminar_pedido_res* response);

#endif /* TERMINAR_PEDIDO_H_ */
