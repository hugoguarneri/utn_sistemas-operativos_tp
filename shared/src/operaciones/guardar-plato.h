#ifndef GUARDAR_PLATO_H_
#define GUARDAR_PLATO_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"


typedef struct {
	char* restaurante;
	char* id_pedido;
	char* comida;
	uint32_t cantidad;
} t_guardar_plato_req;

t_guardar_plato_req* guardar_plato_req_create(char* restaurante, char* id_pedido, char* comida, uint32_t cantidad);

void guardar_plato_req_destroy(t_guardar_plato_req* request);

t_paquete* guardar_plato_req_empaquetar(t_guardar_plato_req* request);

t_guardar_plato_req* guardar_plato_req_desempaquetar(t_paquete* paquete);


char* guardar_plato_req_to_string(t_guardar_plato_req* request);

uint32_t guardar_plato_req_size(t_guardar_plato_req* request);


typedef struct {
	status_code code;
} t_guardar_plato_res;

t_guardar_plato_res* guardar_plato_res_create(status_code code);

void guardar_plato_res_destroy(t_guardar_plato_res* response);

t_paquete* guardar_plato_res_empaquetar(t_guardar_plato_res* response);

t_guardar_plato_res* guardar_plato_res_desempaquetar(t_paquete* paquete);

char* guardar_plato_res_to_string(t_guardar_plato_res*);

uint32_t guardar_plato_res_size(t_guardar_plato_res* response);

#endif /* GUARDAR_PLATO_H_ */
