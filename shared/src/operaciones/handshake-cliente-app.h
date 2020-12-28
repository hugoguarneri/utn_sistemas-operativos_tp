#ifndef HANDSHAKE_CLIENTE_APP_H_
#define HANDSHAKE_CLIENTE_APP_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"

#include "structs/info-cliente.h"

typedef struct {
	t_info_cliente* info_cliente;
} t_handshake_cliente_app_req;

t_handshake_cliente_app_req* handshake_cliente_app_req_create(t_info_cliente* info_cliente);

void handshake_cliente_app_req_destroy(t_handshake_cliente_app_req* request);

t_paquete* handshake_cliente_app_req_empaquetar(t_handshake_cliente_app_req* request);

t_handshake_cliente_app_req* handshake_cliente_app_req_desempaquetar(t_paquete* paquete);

char* handshake_cliente_app_req_to_string(t_handshake_cliente_app_req* request);

uint32_t handshake_cliente_app_req_size(t_handshake_cliente_app_req* request);


typedef struct {
	status_code code;
} t_handshake_cliente_app_res;

t_handshake_cliente_app_res* handshake_cliente_app_res_create(status_code code);

void handshake_cliente_app_res_destroy(t_handshake_cliente_app_res* response);

t_paquete* handshake_cliente_app_res_empaquetar(t_handshake_cliente_app_res* response);

t_handshake_cliente_app_res* handshake_cliente_app_res_desempaquetar(t_paquete* paquete);

char* handshake_cliente_app_res_to_string(t_handshake_cliente_app_res* response);

uint32_t handshake_cliente_app_res_size(t_handshake_cliente_app_res* response);


#endif /* HANDSHAKE_CLIENTE_APP_H_ */
