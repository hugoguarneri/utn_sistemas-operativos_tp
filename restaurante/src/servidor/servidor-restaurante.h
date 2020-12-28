#ifndef SRC_SERVIDOR_SERVIDOR_RESTAURANTE_H_
#define SRC_SERVIDOR_SERVIDOR_RESTAURANTE_H_

#include "structs/colas-planificacion.h"
#include "structs/plato-control-block.h"
#include "restaurante.h"

#include "opcode.h"
#include "paquete.h"
#include "sockets.h"

#include "operaciones/consultar-platos.h"
#include "operaciones/crear-pedido.h"
#include "operaciones/aniadir-plato.h"
#include "operaciones/confirmar-pedido.h"
#include "operaciones/consultar-pedido.h"
#include "operaciones/guardar-pedido.h"
#include "operaciones/guardar-plato.h"
#include "operaciones/obtener-pedido.h"
#include "operaciones/obtener-receta.h"

void exec_servidor_restaurante();

#endif /* SRC_SERVIDOR_SERVIDOR_RESTAURANTE_H_ */
