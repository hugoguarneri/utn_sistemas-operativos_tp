#ifndef CMD_OBTENER_PEDIDO_H_
#define CMD_OBTENER_PEDIDO_H_


#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/obtener-pedido.h"


#define CMD_OBTENER_PEDIDO "ObtenerPedido"

void cmd_obtener_pedido_exec(t_command* comando);

bool cmd_obtener_pedido_validate(t_command* comando);

char* cmd_obtener_pedido_uso();


#endif /* CMD_OBTENER_PEDIDO_H_ */
