#ifndef OBTENER_PEDIDO_H_
#define OBTENER_PEDIDO_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include "structs/estado-plato.h"
#include "structs/estado-pedido.h"

#include "opcode.h"
#include "paquete.h"


typedef struct {
	char* restaurante;
	char* id_pedido;
} t_obtener_pedido_req;
t_obtener_pedido_req* obtener_pedido_req_create(char* restaurante, char* id_pedido);

void obtener_pedido_req_destroy(t_obtener_pedido_req* request);

t_paquete* obtener_pedido_req_empaquetar(t_obtener_pedido_req* request);

t_obtener_pedido_req* obtener_pedido_req_desempaquetar(t_paquete* paquete);

char* obtener_pedido_req_to_string(t_obtener_pedido_req* request);

uint32_t obtener_pedido_req_size(t_obtener_pedido_req* request);


typedef struct {
	e_estado_pedido estado;
	t_list* estado_platos;
} t_obtener_pedido_res;

t_obtener_pedido_res* obtener_pedido_res_create(e_estado_pedido estado, t_list* estado_platos);

void obtener_pedido_res_destroy(t_obtener_pedido_res* response);

t_paquete* obtener_pedido_res_empaquetar(t_obtener_pedido_res* response);

t_obtener_pedido_res* obtener_pedido_res_desempaquetar(t_paquete* paquete);

char* obtener_pedido_res_to_string(t_obtener_pedido_res* response);

uint32_t obtener_pedido_res_size(t_obtener_pedido_res* response);



#endif /* obtener_pedido_H_ */
