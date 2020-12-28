#ifndef CMD_GUARDAR_PLATO_H_
#define CMD_GUARDAR_PLATO_H_


#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/guardar-plato.h"


#define CMD_GUARDAR_PLATO "GuardarPlato"

void cmd_guardar_plato_exec(t_command* comando);

bool cmd_guardar_plato_validate(t_command* comando);

char* cmd_guardar_plato_uso();


#endif /* CMD_GUARDAR_PLATO_H_ */
