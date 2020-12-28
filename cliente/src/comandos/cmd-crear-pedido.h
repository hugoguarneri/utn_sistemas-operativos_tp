#ifndef CMD_CREAR_PEDIDO_H_
#define CMD_CREAR_PEDIDO_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/crear-pedido.h"
#include "operaciones/consultar-info-cliente.h"


#define CMD_CREAR_PEDIDO "CrearPedido"

void cmd_crear_pedido_exec(t_command* comando);

bool cmd_crear_pedido_validate(t_command* comando);

char* cmd_crear_pedido_uso();


#endif /* CMD_CREAR_PEDIDO_H_ */
