#ifndef MONITOR_CLIENTES_RESTAURANTES_H_
#define MONITOR_CLIENTES_RESTAURANTES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>


void  monitor_clientes_restaurantes_init();

void monitor_clientes_restaurantes_destroy();

bool monitor_clientes_restaurantes_has_key(char* id_cliente);

void monitor_clientes_restaurantes_put(char* id_cliente, char* restaurante);

char* monitor_clientes_restaurantes_get(char* id_cliente);

t_list* monitor_clientes_restaurantes_get_all();

void monitor_clientes_restaurantes_remove(char* id_cliente);


#endif /* MONITOR_CLIENTES_RESTAURANTES_H_ */
