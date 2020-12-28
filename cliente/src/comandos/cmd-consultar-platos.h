#ifndef CMD_CONSULTAR_PLATOS_H_
#define CMD_CONSULTAR_PLATOS_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/consultar-platos.h"

#define CMD_CONSULTAR_PLATOS "ConsultarPlatos"

void cmd_consultar_platos_exec(t_command* comando);

bool cmd_consultar_platos_validate(t_command* comando);

char* cmd_consultar_platos_uso();


#endif /* CMD_CONSULTAR_PLATOS_H_ */
