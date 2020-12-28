#ifndef CMD_OBTENER_RECETA_H_
#define CMD_OBTENER_RECETA_H_


#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/obtener-receta.h"


#define CMD_OBTENER_RECETA "ObtenerReceta"

void cmd_obtener_receta_exec(t_command* comando);

bool cmd_obtener_receta_validate(t_command* comando);

char* cmd_obtener_receta_uso();


#endif /* CMD_OBTENER_RECETA_H_ */
