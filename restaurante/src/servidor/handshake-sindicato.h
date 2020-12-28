#ifndef SRC_SERVIDOR_HANDSHAKE_SINDICATO_H_
#define SRC_SERVIDOR_HANDSHAKE_SINDICATO_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "operaciones/obtener-restaurante.h"
#include "restaurante.h"

t_obtener_restaurante_res* obtener_restaurante(char* restaurante_name);

#endif /* SRC_SERVIDOR_HANDSHAKE_SINDICATO_H_ */
