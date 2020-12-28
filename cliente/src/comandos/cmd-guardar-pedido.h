#ifndef CMD_GUARDAR_PEDIDO_H_
#define CMD_GUARDAR_PEDIDO_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/guardar-pedido.h"


#define CMD_GUARDAR_PEDIDO "GuardarPedido"

void cmd_guardar_pedido_exec(t_command* comando);

bool cmd_guardar_pedido_validate(t_command* comando);

char* cmd_guardar_pedido_uso();


#endif /* CMD_GUARDAR_PEDIDO_H_ */
