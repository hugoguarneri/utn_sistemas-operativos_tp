#ifndef SRC_CORE_H_
#define SRC_CORE_H_
#include "comanda.h"
#include "lru.h"
#include "algoritmo-reemplazo.h"

int swap_fd;

char* memoria_ppal;

char* memoria_virt;

pthread_mutex_t array_frame_ppal_mutex;
t_nodo_frame_ppal* array_frame_ppal;

pthread_mutex_t array_frame_virt_mutex;
t_nodo_frame_virt* array_frame_virt;


void core_allocate_fullspace();

t_segmentos* segmentos_new();
void paginas_clear_memory(t_paginas *paginas);
void segmentos_agregar_pedido(t_segmentos* segmentos, int pedido_id);
void platos_agregar_cantidad_lista(t_nodo_pagina *pagina);
t_nodo_pagina* platos_buscar_plato(t_paginas* paginas, char* comida);
void segmento_destroy(t_nodo_segmento* nodo);
void platos_agregar_cantidad(t_nodo_pagina* pagina, int cantidad);
bool platos_agregar(t_paginas *paginas, char *comida, uint32_t cantidad);

void memoria_page_swap_in(t_nodo_pagina* pagina);

t_nodo_segmento* segmentos_get_by_id(t_segmentos *segmentos, int pedido_id);

void segmentos_remove_reference(t_segmentos *segmentos, int pedido_id);

t_plato_comanda* memoria_get_plato(uint32_t frame_principal);

uint32_t array_frame_ppal_libre();

uint32_t array_frame_virt_libre();

t_list* platos_obtener_platos(t_paginas *paginas);


#endif /* SRC_CORE_H_ */
