#ifndef CONSULTAR_INFO_CLIENTE_H_
#define CONSULTAR_INFO_CLIENTE_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include "opcode.h"
#include "paquete.h"
#include "structs/info-cliente.h"


t_paquete* consultar_info_cliente_req_empaquetar();

char* consultar_info_cliente_req_to_string();


t_paquete* consultar_info_cliente_res_empaquetar(t_info_cliente* response);

t_info_cliente* consultar_info_cliente_res_desempaquetar(t_paquete* paquete);

char* consultar_info_cliente_res_to_string(t_info_cliente* response);

uint32_t consultar_info_cliente_res_size(t_info_cliente* response);

#endif /* CONSULTAR_PEDIDO_H_ */
