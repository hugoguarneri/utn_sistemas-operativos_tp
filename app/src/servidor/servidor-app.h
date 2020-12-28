#ifndef SERVIDOR_APP_H_
#define SERVIDOR_APP_H_

#include "app.h"

#include "../structs/monitors/monitor-info-clientes.h"
#include "../structs/monitors/monitor-info-restaurantes.h"
#include "../structs/monitors/monitor-clientes-restaurantes.h"

#include "../../../shared/src/opcode.h"
#include "../../../shared/src/paquete.h"
#include "../../../shared/src/sockets.h"

#include "../../../shared/src/structs/info-restaurante.h"
#include "../../../shared/src/structs/info-cliente.h"
#include "../../../shared/src/structs/estado-plato.h"

#include "../../../shared/src/operaciones/handshake-restaurante-app.h"
#include "../../../shared/src/operaciones/consultar-restaurantes.h"
#include "../../../shared/src/operaciones/seleccionar-restaurante.h"
#include "../../../shared/src/operaciones/consultar-platos.h"
#include "../../../shared/src/operaciones/crear-pedido.h"
#include "../../../shared/src/operaciones/guardar-pedido.h"
#include "../../../shared/src/operaciones/aniadir-plato.h"
#include "../../../shared/src/operaciones/guardar-plato.h"
#include "../../../shared/src/operaciones/plato-listo.h"
#include "../../../shared/src/operaciones/confirmar-pedido.h"
#include "../../../shared/src/operaciones/consultar-pedido.h"
#include "../../../shared/src/operaciones/obtener-pedido.h"
#include "../../../shared/src/operaciones/consultar-info-cliente.h"




void exec_servidor_app();

#endif /* SERVIDOR_APP_H_ */
