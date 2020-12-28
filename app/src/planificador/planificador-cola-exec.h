#ifndef PLANIFICADOR_COLA_EXEC_H_
#define PLANIFICADOR_COLA_EXEC_H_

#include "planificador-app.h"

#include "../../../shared/src/opcode.h"
#include "../../../shared/src/paquete.h"
#include "../../../shared/src/sockets.h"
#include "../../../shared/src/operaciones/obtener-pedido.h"

#include "../structs/monitors/monitor-clientes-restaurantes.h"
#include "../structs/monitors/monitor-info-clientes.h"
#include "../structs/monitors/monitor-info-restaurantes.h"
#include "../structs/monitors/monitor-repartidores.h"


void cola_exec_listener();


#endif /*PLANIFICADOR_COLA_EXEC_H_ */
