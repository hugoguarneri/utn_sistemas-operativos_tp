#ifndef PLATOS_COMANDA_H
#define PLATOS_COMANDA_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "comanda.h"

t_dictionary *diccionario;

t_plato_comanda* crear_plato_comanda(char* plato,uint32_t cantidad);
void destroy_plato_comanda(t_plato_comanda* miPlato);

#endif
