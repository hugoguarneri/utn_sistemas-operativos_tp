#ifndef PLATO_LISTO_H_
#define PLATO_LISTO_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"


typedef struct {
	char* restaurante;
	char* id_pedido;
	char* comida;
} t_plato_listo_req;

t_plato_listo_req* plato_listo_req_create(char* restaurante, char* id_pedido, char* comida);

void plato_listo_req_destroy(t_plato_listo_req* request);

t_paquete* plato_listo_req_empaquetar(t_plato_listo_req* request);

t_plato_listo_req* plato_listo_req_desempaquetar(t_paquete* paquete);

char* plato_listo_req_to_string(t_plato_listo_req* request);

uint32_t plato_listo_req_size(t_plato_listo_req* request);

typedef struct {
	status_code code;
} t_plato_listo_res;

t_plato_listo_res* plato_listo_res_create(status_code code);

void plato_listo_res_destroy(t_plato_listo_res* response);

t_paquete* plato_listo_res_empaquetar(t_plato_listo_res* response);

t_plato_listo_res* plato_listo_res_desempaquetar(t_paquete* paquete);

char* plato_listo_res_to_string(t_plato_listo_res* response);

uint32_t plato_listo_res_size(t_plato_listo_res* response);

#endif /* PLATO_LISTO_H_ */
