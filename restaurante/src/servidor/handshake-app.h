#ifndef SRC_SERVIDOR_HANDSHAKE_APP_H_
#define SRC_SERVIDOR_HANDSHAKE_APP_H_

#include <errno.h>
#include "operaciones/handshake-restaurante-app.h"
#include "operaciones/obtener-restaurante.h"
#include "restaurante.h"

void handshake_app(t_obtener_restaurante_res* t_obtener_restaurante_res, char* restaurante_name);

#endif /* SRC_SERVIDOR_HANDSHAKE_APP_H_ */
