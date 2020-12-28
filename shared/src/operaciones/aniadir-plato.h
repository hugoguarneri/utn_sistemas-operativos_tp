#ifndef ANIADIR_PLATO_H_
#define ANIADIR_PLATO_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"


typedef struct {
	char* plato;
	char* id_pedido;
} t_aniadir_plato_req;

t_aniadir_plato_req* aniadir_plato_req_create(char* plato, char* id_pedido);

void aniadir_plato_req_destroy(t_aniadir_plato_req* request);

t_paquete* aniadir_plato_req_empaquetar(t_aniadir_plato_req* request);

t_aniadir_plato_req* aniadir_plato_req_desempaquetar(t_paquete* paquete);

char* aniadir_plato_req_to_string(t_aniadir_plato_req* request);

uint32_t aniadir_plato_req_size(t_aniadir_plato_req* request);

typedef struct {
	status_code code;
} t_aniadir_plato_res;

t_aniadir_plato_res* aniadir_plato_res_create(status_code code);

void aniadir_plato_res_destroy(t_aniadir_plato_res* response);

t_paquete* aniadir_plato_res_empaquetar(t_aniadir_plato_res* c);

t_aniadir_plato_res* aniadir_plato_res_desempaquetar(t_paquete* paquete);

char* aniadir_plato_res_to_string(t_aniadir_plato_res* response);

uint32_t aniadir_plato_res_size(t_aniadir_plato_res* response);

#endif /* ANIADIR_PLATO_H_ */
