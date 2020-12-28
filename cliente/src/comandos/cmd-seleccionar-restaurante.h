#ifndef CMD_SELECCIONAR_RESTAURANTE_H_
#define CMD_SELECCIONAR_RESTAURANTE_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/seleccionar-restaurante.h"


#define CMD_SELECCIONAR_RESTAURANTE "SeleccionarRestaurante"

void cmd_seleccionar_restaurante_exec(t_command* comando);

bool cmd_seleccionar_restaurante_validate(t_command* comando);

char* cmd_seleccionar_restaurante_uso();


#endif /* CMD_SELECCIONAR_RESTAURANTE_H_ */
