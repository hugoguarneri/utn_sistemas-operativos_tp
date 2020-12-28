#ifndef CMD_TERMINAR_PEDIDO_H_
#define CMD_TERMINAR_PEDIDO_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/terminar-pedido.h"


#define CMD_TERMINAR_PEDIDO "TerminarPedido"

void cmd_terminar_pedido_exec(t_command* comando);

bool cmd_terminar_pedido_validate(t_command* comando);

char* cmd_terminar_pedido_uso();


#endif /* CMD_TERMINAR_PEDIDO_H_ */
