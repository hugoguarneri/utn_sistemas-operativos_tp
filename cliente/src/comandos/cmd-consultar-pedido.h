#ifndef CMD_CONSULTAR_PEDIDO_H_
#define CMD_CONSULTAR_PEDIDO_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/consultar-pedido.h"

#define CMD_CONSULTAR_PEDIDO "ConsultarPedido"

void cmd_consultar_pedido_exec(t_command* comando);

bool cmd_consultar_pedido_validate(t_command* comando);

char* cmd_consultar_pedido_uso();

#endif /* CMD_CONSULTAR_PEDIDO_H_ */
