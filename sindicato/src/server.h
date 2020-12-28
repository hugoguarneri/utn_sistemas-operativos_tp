#ifndef SERVER_H
#define SERVER_H
#include "opcode.h"
#include "paquete.h"
#include "sockets.h"
#include "operaciones/obtener-restaurante.h"
#include "operaciones/obtener-receta.h"
#include "operaciones/consultar-platos.h"

#include "operaciones/obtener-pedido.h"
#include "operaciones/guardar-plato.h"
#include "operaciones/plato-listo.h"
#include "operaciones/confirmar-pedido.h"
#include "operaciones/obtener-pedido.h"
#include "operaciones/terminar-pedido.h"
#include "operaciones/guardar-pedido.h"
#include "sindicato.h"
#include "restaurante.h"
#include "receta.h"
#include "pedido.h"


void* exec_servidor(void *ptr);

#endif
