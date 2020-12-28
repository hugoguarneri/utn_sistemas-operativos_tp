#ifndef HANDSHAKE_RESTAURANTE_APP_H_
#define HANDSHAKE_RESTAURANTE_APP_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"

#include "structs/info-restaurante.h"

typedef struct {
	t_info_restaurante* info_restaurante;
} t_handshake_restaurante_app_req;

t_handshake_restaurante_app_req* handshake_restaurante_app_req_create(t_info_restaurante* info_restaurante);

void handshake_restaurante_app_req_destroy(t_handshake_restaurante_app_req* request);

t_paquete* handshake_restaurante_app_req_empaquetar(t_handshake_restaurante_app_req* request);

t_handshake_restaurante_app_req* handshake_restaurante_app_req_desempaquetar(t_paquete* paquete);

char* handshake_restaurante_app_req_to_string(t_handshake_restaurante_app_req* request);

uint32_t handshake_restaurante_app_req_size(t_handshake_restaurante_app_req* request);


typedef struct {
	status_code code;
} t_handshake_restaurante_app_res;

t_handshake_restaurante_app_res* handshake_restaurante_app_res_create(status_code code);

void handshake_restaurante_app_res_destroy(t_handshake_restaurante_app_res* response);

t_paquete* handshake_restaurante_app_res_empaquetar(t_handshake_restaurante_app_res* response);

t_handshake_restaurante_app_res* handshake_restaurante_app_res_desempaquetar(t_paquete* paquete);

char* handshake_restaurante_app_res_to_string(t_handshake_restaurante_app_res* response);

uint32_t handshake_restaurante_app_res_size(t_handshake_restaurante_app_res* response);

#endif /* HANDSHAKE_RESTAURANTE_APP_H_ */
