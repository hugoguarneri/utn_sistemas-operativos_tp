#ifndef CONSULTAR_PLATOS_H_
#define CONSULTAR_PLATOS_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "utils.h"
#include "opcode.h"
#include "paquete.h"


typedef struct {
	/*opcional*/
	char* restaurante;
} t_consultar_platos_req;

t_consultar_platos_req* consultar_platos_req_create(char* restaurante);

void consultar_platos_req_destroy(t_consultar_platos_req* request);

t_paquete* consultar_platos_req_empaquetar(t_consultar_platos_req* request);

t_consultar_platos_req* consultar_platos_req_desempaquetar(t_paquete* paquete);

char* consultar_platos_req_to_string(t_consultar_platos_req* request);

uint32_t consultar_platos_req_size(t_consultar_platos_req* request);


typedef struct {
	t_list* platos;
} t_consultar_platos_res;

t_consultar_platos_res* consultar_platos_res_create(t_list* comida);

void consultar_platos_res_destroy(t_consultar_platos_res* response);

t_paquete* consultar_platos_res_empaquetar(t_consultar_platos_res* response);

t_consultar_platos_res* consultar_platos_res_desempaquetar(t_paquete* paquete);

char* consultar_platos_res_to_string(t_consultar_platos_res* response);

uint32_t consultar_platos_res_size(t_consultar_platos_res *response);


t_paquete* consutar_platos_res_data_empaquetar(char *response);

#endif /* CONSULTAR_PLATOS_H_ */
