#ifndef CMD_OBTENER_RESTAURANTE_H_
#define CMD_OBTENER_RESTAURANTE_H_


#include "cliente.h"
#include "command.h"
#include "opcode.h"
#include "paquete.h"
#include "operaciones/obtener-restaurante.h"


#define CMD_OBTENER_RESTAURANTE "ObtenerRestaurante"

void cmd_obtener_restaurante_exec(t_command* comando);

bool cmd_obtener_restaurante_validate(t_command* comando);

char* cmd_obtener_restaurante_uso();


#endif /* CMD_OBTENER_RESTAURANTE_H_ */
