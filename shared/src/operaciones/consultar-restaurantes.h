#ifndef CONSULTAR_RESTAURANTES_H_
#define CONSULTAR_RESTAURANTES_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include "opcode.h"
#include "paquete.h"

t_paquete* consultar_restaurantes_req_empaquetar();

char* consultar_restaurantes_req_to_string();

typedef struct {
	t_list* nombres;
} t_consultar_restaurantes_res;

t_consultar_restaurantes_res* consultar_restaurantes_res_create_from_list(t_list* nombres);

void consultar_restaurantes_res_destroy(t_consultar_restaurantes_res* response);

t_paquete* consultar_restaurantes_res_empaquetar(t_consultar_restaurantes_res* response);

t_consultar_restaurantes_res* consultar_restaurantes_res_desempaquetar(t_paquete* paquete);

char* consultar_restaurantes_res_to_string(t_consultar_restaurantes_res* response);

uint32_t consultar_restaurantes_res_size(t_consultar_restaurantes_res* response);

#endif /* CONSULTAR_RESTAURANTES_H_ */
