#ifndef CMD_PLATO_LISTO_H_
#define CMD_PLATO_LISTO_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/plato-listo.h"

#define CMD_PLATO_LISTO "PlatoListo"

void cmd_plato_listo_exec(t_command* comando);

bool cmd_plato_listo_validate(t_command* comando);

char* cmd_plato_listo_uso();


#endif /* CMD_PLATO_LISTO_H_ */
