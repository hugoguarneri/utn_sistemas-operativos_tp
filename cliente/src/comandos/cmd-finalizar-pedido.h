#ifndef CMD_FINALIZAR_PEDIDO_H_
#define CMD_FINALIZAR_PEDIDO_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/finalizar-pedido.h"


#define CMD_FINALIZAR_PEDIDO "FinalizarPedido"

void cmd_finalizar_pedido_exec(t_command* comando);

bool cmd_finalizar_pedido_validate(t_command* comando);

char* cmd_finalizar_pedido_uso();


#endif /* CMD_FINALIZAR_PEDIDO_H_ */
