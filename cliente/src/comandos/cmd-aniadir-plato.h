#ifndef CMD_ANIADIR_PLATO_H_
#define CMD_ANIADIR_PLATO_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/aniadir-plato.h"
#include "operaciones/consultar-info-cliente.h"

#define CMD_ANIADIR_PLATO "AniadirPlato"

void cmd_aniadir_plato_exec(t_command* comando);

bool cmd_aniadir_plato_validate(t_command* comando);

char* cmd_aniadir_plato_uso();


#endif /* CMD_ANIADIR_PLATO_H_ */
