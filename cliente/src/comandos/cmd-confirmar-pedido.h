#ifndef CMD_CONFIRMAR_PEDIDO_H_
#define CMD_CONFIRMAR_PEDIDO_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/confirmar-pedido.h"
#include "operaciones/consultar-info-cliente.h"


#define CMD_CONFIRMAR_PEDIDO "ConfirmarPedido"

void cmd_confirmar_pedido_exec(t_command* comando);

bool cmd_confirmar_pedido_validate(t_command* comando);

char* cmd_confirmar_pedido_uso();


#endif /* CMD_CONFIRMAR_PEDIDO_H_ */
