#ifndef SRC_LRU_H_
#define SRC_LRU_H_
#include "comanda.h"

/**
 * El LRU se puede implementar utilizando un t_list sincronizado
 *
 * la idea es que cuando se accede a una pagina se agrega al final el numero en la t_list
 * si la pagina ya estaba guardada, se elimina y se vuelve a agregar al final.
 *
 * Para encontrar una pagina sin uso se puede obtener el primero de la t_list. Pq los mas usados terminan llegando al final de la lista
 */
pthread_mutex_t lru_lock;
t_list *lru_list;

void lru_init();
void lru_add_updated(int page);
void lru_remove(int page);
int lru_get_candidate();
void lru_destroy();

#endif /* SRC_LRU_H_ */

