#ifndef SRC_clock_H_
#define SRC_clock_H_
#include "comanda.h"

/**
 * El clock se puede implementar utilizando un t_list sincronizado con un bit de modificado,
 *
 * la idea es que cada pagina nueva se agrega a clock y se le van seteando el bit de modificado y de uso,
 * a medida que cambia el estado de la pagina lo voy actualiznado para luego poder elegir el que no esta modificado ni en uso.
 * Si no existe alguno que no tenga eso se busca uno que tenga el bit de uso en 0, y se elige como victim.
 * Si no existe ninguno que no este como modificado = 0 y uso = 0, ¿saco el primero?
 *
 */

typedef struct {
	bool modificado;
	bool uso;
	uint32_t page;
} t_clock;

typedef struct {
	pthread_mutex_t clock_lock;
	t_list *clock_list;
}cache_clock;

cache_clock* clock_modificado;
uint32_t POINTER_INDEX;
uint32_t TAMANIO_CLOCK;


void *new_clock(int page, bool modificado);

void clock_init();

void clock_add_updated(int page,bool modificado);

void clock_remove(int page);

int clock_get_candidate();

void clock_destroy();

int find_element_page(bool (*condition)(void*));

void clock_a_modificadar(int page);

#endif /* SRC_clock_H_ */

