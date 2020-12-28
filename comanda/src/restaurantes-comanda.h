#ifndef RESTAURANTES_COMANDA_H_
#define RESTAURANTES_COMANDA_H_
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "comanda.h"

void restaurante_dictionary_init();
void restaurante_dictionary_destroy();
bool restaurante_dictionary_has_key(char *nombre);
void restaurante_dictionary_put(char *nombre, void* data);
void* restaurante_dictionary_get(char *nombre);

#endif

