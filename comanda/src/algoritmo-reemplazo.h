#ifndef ALGORITMO_REEMPLAZO_H_
#define ALGORITMO_REEMPLAZO_H_
#include "comanda.h"
#include "lru.h"
#include "clock.h"
#include "core.h"


#endif /* SRC_ALGORITMO-REEMPLAZO_H_ */

uint32_t cache_system;

void init_config_algoritmo();

void init_algoritmo_reemplazo();

void clear_cache_paginas(t_paginas *paginas);

void add_page(int32_t pagina,bool modificado);

uint32_t selection_victim_cache();

int get_page(int page);

