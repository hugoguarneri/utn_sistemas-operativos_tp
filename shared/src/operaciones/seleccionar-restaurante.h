#ifndef SELECCIONAR_RESTAURANTE_H_
#define SELECCIONAR_RESTAURANTE_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"


typedef struct {
	char* cliente;
	char* restaurante;
} t_seleccionar_restaurante_req;

t_seleccionar_restaurante_req* seleccionar_restaurante_req_create(char* cliente, char* restaurante);

void seleccionar_restaurante_req_destroy(t_seleccionar_restaurante_req* request);

t_paquete* seleccionar_restaurante_req_empaquetar(t_seleccionar_restaurante_req* request);

t_seleccionar_restaurante_req* seleccionar_restaurante_req_desempaquetar(t_paquete* paquete);

char* seleccionar_restaurante_req_to_string(t_seleccionar_restaurante_req* request);

uint32_t seleccionar_restaurante_req_size(t_seleccionar_restaurante_req* request);


typedef struct {
	status_code code;
} t_seleccionar_restaurante_res;

t_seleccionar_restaurante_res* seleccionar_restaurante_res_create(status_code code);

void seleccionar_restaurante_res_destroy(t_seleccionar_restaurante_res* response);

t_paquete* seleccionar_restaurante_res_empaquetar(t_seleccionar_restaurante_res* response);

t_seleccionar_restaurante_res* seleccionar_restaurante_res_desempaquetar(t_paquete* paquete);

char* seleccionar_restaurante_res_to_string(t_seleccionar_restaurante_res* response);

uint32_t seleccionar_restaurante_res_size(t_seleccionar_restaurante_res* response);

#endif /* SELECCIONAR_RESTAURANTE_H_ */
