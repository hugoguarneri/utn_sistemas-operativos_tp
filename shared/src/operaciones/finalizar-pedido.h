#ifndef FINALIZAR_PEDIDO_H_
#define FINALIZAR_PEDIDO_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"


typedef struct {
	char* id_pedido;
	char* restaurante;
} t_finalizar_pedido_req;

t_finalizar_pedido_req* finalizar_pedido_req_create(char* id_pedido, char* restaurante);

void finalizar_pedido_req_destroy(t_finalizar_pedido_req* request);

t_paquete* finalizar_pedido_req_empaquetar(t_finalizar_pedido_req* request);

t_finalizar_pedido_req* finalizar_pedido_req_desempaquetar(t_paquete* paquete);

char* finalizar_pedido_req_to_string(t_finalizar_pedido_req* request);

void finalizar_pedido_req_to_log(char* accion, t_finalizar_pedido_req* request);

uint32_t finalizar_pedido_req_size(t_finalizar_pedido_req* request);



typedef struct {
	status_code code;
} t_finalizar_pedido_res;

t_finalizar_pedido_res* finalizar_pedido_res_create(status_code code);

void finalizar_pedido_res_destroy(t_finalizar_pedido_res* response);

t_paquete* finalizar_pedido_res_empaquetar(t_finalizar_pedido_res* response);

t_finalizar_pedido_res* finalizar_pedido_res_desempaquetar(t_paquete* paquete);

char* finalizar_pedido_res_to_string(t_finalizar_pedido_res* response);

void finalizar_pedido_res_to_log(char* accion, t_finalizar_pedido_res* response);

uint32_t finalizar_pedido_res_size(t_finalizar_pedido_res* response);


#endif /* FINALIZAR_PEDIDO_H_ */
