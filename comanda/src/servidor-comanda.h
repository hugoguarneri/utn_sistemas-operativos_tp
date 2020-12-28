#ifndef SERVIDOR_COMANDA_H
#define SERVIDOR_COMANDA_H
#include "comanda.h"
#include "operaciones/plato-listo.h"
#include "operaciones/handshake-app-comanda.h"
#include "operaciones/aniadir-plato.h"
#include "operaciones/guardar-plato.h"
#include "operaciones/obtener-pedido.h"
#include "operaciones/consultar-platos.h"
#include "operaciones/confirmar-pedido.h"
#include "operaciones/guardar-pedido.h"
#include "operaciones/finalizar-pedido.h"

void exec_servidor_comanda();

#endif
