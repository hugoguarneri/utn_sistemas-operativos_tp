#ifndef PLANIFICADOR_COLA_EXIT_H_
#define PLANIFICADOR_COLA_EXIT_H_

#include "planificador-app.h"


#include "../../../shared/src/opcode.h"
#include "../../../shared/src/paquete.h"
#include "../../../shared/src/sockets.h"
#include "../../../shared/src/operaciones/finalizar-pedido.h"
#include "../structs/monitors/monitor-clientes-restaurantes.h"
#include "../structs/monitors/monitor-info-clientes.h"
#include "../structs/monitors/monitor-info-restaurantes.h"
#include "../structs/monitors/monitor-repartidores.h"


void cola_exit_listener();


#endif /* PLANIFICADOR_COLA_EXIT_H_ */
