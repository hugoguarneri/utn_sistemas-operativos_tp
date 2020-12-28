#ifndef CMD_CONSULTAR_RESTAURANTES_H_
#define CMD_CONSULTAR_RESTAURANTES_H_

#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/consultar-restaurantes.h"


#define CMD_CONSULTAR_RESTAURANTES  "ConsultarRestaurantes"

void cmd_consultar_restaurantes_exec(t_command* comando);

bool cmd_consultar_restaurantes_validate(t_command* comando);

char* cmd_consultar_restaurantes_uso();

#endif /* CMD_CONSULTAR_RESTAURANTES_H_ */
