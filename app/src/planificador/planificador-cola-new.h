#ifndef PLANIFICADOR_COLA_NEW_H_
#define PLANIFICADOR_COLA_NEW_H_

#include "planificador-app.h"

#include "../../../shared/src/opcode.h"
#include "../../../shared/src/paquete.h"
#include "../../../shared/src/sockets.h"
#include "../structs/monitors/monitor-clientes-restaurantes.h"
#include "../structs/monitors/monitor-info-clientes.h"
#include "../structs/monitors/monitor-info-restaurantes.h"
#include "../structs/monitors/monitor-repartidores.h"


void cola_new_listener();


#endif /* PLANIFICADOR_COLA_NEW_H_ */
