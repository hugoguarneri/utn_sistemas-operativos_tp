#ifndef SRC_RESTAURANTE_DEFAULT_H_
#define SRC_RESTAURANTE_DEFAULT_H_

#include "app.h"

#include "../../../shared/src/opcode.h"
#include "../../../shared/src/paquete.h"
#include "../../../shared/src/sockets.h"

#include "../../../shared/src/structs/info-restaurante.h"
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

pthread_mutex_t mutex_id_pedido_count;
int id_pedido_count;

t_info_restaurante* info_resto_default;

t_list* platos_resto_default;

void exec_restaurante_default();


#endif /* SRC_RESTAURANTE_DEFAULT_H_ */
