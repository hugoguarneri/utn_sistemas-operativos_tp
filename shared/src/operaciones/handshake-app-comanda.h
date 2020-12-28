#ifndef HANDSHAKE_APP_COMANDA_H_
#define HANDSHAKE_APP_COMANDA_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>

#include "opcode.h"
#include "paquete.h"


t_paquete* handshake_app_comanda_req_empaquetar();

char* handshake_app_comanda_req_to_string();


typedef struct {
	status_code code;
} t_handshake_app_comanda_res;

t_handshake_app_comanda_res* handshake_app_comanda_res_create(status_code code);

void handshake_app_comanda_res_destroy(t_handshake_app_comanda_res* response);

t_paquete* handshake_app_comanda_res_empaquetar(t_handshake_app_comanda_res* response);

t_handshake_app_comanda_res* handshake_app_comanda_res_desempaquetar(t_paquete* paquete);

char* handshake_app_comanda_res_to_string(t_handshake_app_comanda_res* response);

uint32_t handshake_app_comanda_res_size(t_handshake_app_comanda_res* response);


#endif /* HANDSHAKE_APP_COMANDA_H_ */
