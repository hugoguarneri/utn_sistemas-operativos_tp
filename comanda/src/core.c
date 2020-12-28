#include "core.h"

void core_allocate_fullspace() {

	memoria_ppal = calloc(sizeof(char), configuracion->tamanio_memoria_bytes);
	log_info(logger, "Creada la memoria");

//	se establecen para permitir la lectura y escritura por parte del propietario, y para permitir la lectura solo por miembros del grupo y otros.
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	swap_fd = open(".swapmem", O_RDWR | O_CREAT | O_TRUNC, mode);

	if (swap_fd == (-1)) {
		perror("open");
		exit_failure();
	}
	ftruncate(swap_fd,configuracion->tamanio_swap_bytes);
	memoria_virt = mmap(NULL, configuracion->tamanio_swap_bytes, PROT_WRITE, MAP_PRIVATE, swap_fd, 0);

	if (memoria_virt == MAP_FAILED) {
		perror("mmap");
		exit_failure();
	}
	log_info(logger, "Creada la swap");

	// creo las paginas de la memoria principal
	array_frame_ppal = calloc(sizeof(t_nodo_frame_ppal), configuracion->memoria_max_frames);

	for (int i = 0; i < configuracion->memoria_max_frames; i++) {
		array_frame_ppal[i].offset = i * 32;
		array_frame_ppal[i].ocupado = false;
	}
	// creo las paginas de la memoria swap
	array_frame_virt = calloc(sizeof(t_nodo_frame_virt), configuracion->swap_max_frames);
	for (int i = 0; i < configuracion->swap_max_frames; i++) {
		array_frame_virt[i].offset = i * 32;
		array_frame_virt[i].ocupado = false;
	}

	init_algoritmo_reemplazo();
}

t_segmentos* segmentos_new() {
	t_segmentos *segs = calloc(sizeof(t_segmentos), 1);

	pthread_mutex_init(&segs->mutex, NULL);
	segs->segmento_list = list_create();

	return segs;
}

t_paginas* paginas_new() {
	t_paginas *paginas = calloc(sizeof(t_paginas), 1);
	pthread_mutex_init(&paginas->mutex, NULL);
	paginas->pagina_list = list_create();

	return paginas;
}

void paginas_clear_memory(t_paginas *paginas) {
	void limpiar_memoria(t_nodo_pagina *nodo) {
		pthread_mutex_lock(&nodo->mutex);
		if (nodo->frame_principal != -1) {
			// Esta en memoria principal lo tengo que eliminar
			log_debug(logger, "Eliminando de memoria principal el frame %d", nodo->frame_principal);
			pthread_mutex_lock(&array_frame_ppal_mutex);
			array_frame_ppal[nodo->frame_principal].ocupado = false;
			array_frame_ppal[nodo->frame_principal].referencia = NULL;
			array_frame_ppal[nodo->frame_principal].updated_at = -1;
			bzero(memoria_ppal + array_frame_ppal[nodo->frame_principal].offset, 32);
			pthread_mutex_unlock(&array_frame_ppal_mutex);
		}

		if (nodo->frame_virtual != -1) {
			log_debug(logger, "Eliminando de memoria virtual el frame %d", nodo->frame_virtual);
			pthread_mutex_lock(&array_frame_virt_mutex);
			array_frame_virt[nodo->frame_virtual].ocupado = false;
			bzero(memoria_virt + array_frame_virt[nodo->frame_virtual].offset, 32);
			msync(memoria_virt + array_frame_virt[nodo->frame_virtual].offset, 32, MS_SYNC);
			pthread_mutex_unlock(&array_frame_virt_mutex);
		}

		pthread_mutex_unlock(&nodo->mutex);
		pthread_mutex_destroy(&nodo->mutex);

	}
	clear_cache_paginas(paginas);
	list_iterate(paginas->pagina_list, (void *)limpiar_memoria);
	list_destroy_and_destroy_elements(paginas->pagina_list, free);

}

void segmentos_agregar_pedido(t_segmentos *segmentos, int pedido_id) {
	pthread_mutex_lock(&segmentos->mutex);
	bool is_segmento(t_nodo_segmento *nodo) {
		return nodo->pedido_id == pedido_id;
	}
	t_nodo_segmento *segmento = list_find(segmentos->segmento_list, (void*) is_segmento);
	if (segmento == NULL) {
		segmento = calloc(sizeof(t_nodo_segmento), 1);
		segmento->estado = Pendiente;
		segmento->pedido_id = pedido_id;
		segmento->paginas = paginas_new();
		list_add(segmentos->segmento_list, segmento);
	}

	pthread_mutex_unlock(&segmentos->mutex);

}

t_nodo_segmento* segmentos_get_by_id(t_segmentos *segmentos, int pedido_id) {
	pthread_mutex_lock(&segmentos->mutex);
	bool is_segmento(t_nodo_segmento *nodo) {
		return nodo->pedido_id == pedido_id;
	}
	t_nodo_segmento *segmento = list_find(segmentos->segmento_list, (void*) is_segmento);
	pthread_mutex_unlock(&segmentos->mutex);
	return segmento;
}

void segmentos_remove_reference(t_segmentos *segmentos, int pedido_id) {
	pthread_mutex_lock(&segmentos->mutex);
	bool is_segmento(t_nodo_segmento *nodo) {
		return nodo->pedido_id == pedido_id;
	}
	list_remove_by_condition(segmentos->segmento_list, (void*) is_segmento);
	pthread_mutex_unlock(&segmentos->mutex);
}

t_nodo_pagina* platos_buscar_plato(t_paginas *paginas, char *comida) {
	t_nodo_pagina *pagina = NULL;

	// Recorro los platos buscando por nombre
	pthread_mutex_lock(&paginas->mutex);
	int size = list_size(paginas->pagina_list);
	for (int i = 0; i < size; i++) {
		pagina = list_get(paginas->pagina_list, i);
		if (pagina->frame_principal == -1) {
			memoria_page_swap_in(pagina);
		}
		// Obtengo de la memoria el plato
		t_plato_comanda *plato = memoria_get_plato(pagina->frame_principal);
		if (strcmp(plato->plato, comida) == 0) {
			add_page(pagina->frame_principal,true);
			break;
		} else {
			pagina = NULL;
		}
		free(plato);
	}

	pthread_mutex_unlock(&paginas->mutex);
	return pagina;
}

void segmento_destroy(t_nodo_segmento* nodo){
	free(nodo->paginas);
	free(nodo);
}

void platos_agregar_cantidad(t_nodo_pagina *pagina, int cantidad) {
	t_plato_comanda plato_tmp;
	pthread_mutex_lock(&array_frame_ppal_mutex);
	log_debug(logger, "Agregando cantidad pagina: %d", pagina->frame_principal);
	uint32_t offset = array_frame_ppal[pagina->frame_principal].offset;
	memcpy(&plato_tmp, memoria_ppal + offset, FRAME_SIZE);

	plato_tmp.cantidad += cantidad;
	log_debug(logger, "Cantidad agregada plato: %s cantidad_actual: %d", plato_tmp.plato, plato_tmp.cantidad);

	memcpy(memoria_ppal + offset, &plato_tmp, FRAME_SIZE);

	pthread_mutex_unlock(&array_frame_ppal_mutex);
}

void platos_agregar_cantidad_lista(t_nodo_pagina *pagina) {
	t_plato_comanda plato_tmp;
	pthread_mutex_lock(&array_frame_ppal_mutex);
	log_debug(logger, "Agregando cantidad lista en pagina: %d", pagina->frame_principal);
	uint32_t offset = array_frame_ppal[pagina->frame_principal].offset;
	memcpy(&plato_tmp, memoria_ppal + offset, FRAME_SIZE);

	plato_tmp.cantidad_lista += 1;
	log_debug(logger, "Cantidad lista agregada plato: %s cantidad_lista actual: %d", plato_tmp.plato, plato_tmp.cantidad_lista);

	memcpy(memoria_ppal + offset, &plato_tmp, FRAME_SIZE);

	pthread_mutex_unlock(&array_frame_ppal_mutex);
}

bool platos_agregar(t_paginas *paginas, char *comida, uint32_t cantidad) {
	t_plato_comanda plato_tmp;
	t_nodo_pagina *nodo_pagina;
	bzero(&plato_tmp, FRAME_SIZE);

	// copiar a estructura temporal
	strcpy(plato_tmp.plato, comida);
	plato_tmp.cantidad = cantidad;

	// lockear el mutex del swap
	pthread_mutex_lock(&array_frame_virt_mutex);

	// buscar un espacio libre

	uint32_t index = array_frame_virt_libre();

	// EXCEPCION: SI NO HAY MAS ESPACIO RETORNO FALSE
	if (index == -1) {
		//NO HAY ESPACIO SUFICIENTE
		log_warning(logger, "La memoria virtual se quedo sin espacio de almacenamiento");
		pthread_mutex_unlock(&array_frame_virt_mutex);
		return false;
	}

	// Copiar la estructura en el espacio libre
	uint32_t offset = array_frame_virt[index].offset;
	memcpy(memoria_virt + offset, &plato_tmp, FRAME_SIZE);
	msync(memoria_virt + offset, 32, MS_SYNC);

	// Crear la nueva pagina apuntando al nuevo frame

	nodo_pagina = calloc(sizeof(t_nodo_pagina), 1);
	nodo_pagina->frame_virtual = index;
	nodo_pagina->frame_principal = -1;
	pthread_mutex_init(&nodo_pagina->mutex, NULL);
	pthread_mutex_lock(&paginas->mutex);
	list_add(paginas->pagina_list, nodo_pagina);
	pthread_mutex_unlock(&paginas->mutex);

	// desbloquear mutex de swap
	pthread_mutex_unlock(&array_frame_virt_mutex);

	return true;
}

void memoria_page_swap_in(t_nodo_pagina *pagina) {
	t_plato_comanda plato_tmp_swap;
	log_info(logger, "Realizando swap in desde %d", pagina->frame_virtual);
	pthread_mutex_lock(&pagina->mutex);
	// Obtengo de la memoria swap el dato
	uint32_t offset = array_frame_virt[pagina->frame_virtual].offset;
	memcpy(&plato_tmp_swap, memoria_virt + offset, FRAME_SIZE);

	pthread_mutex_lock(&array_frame_ppal_mutex);
	// Busco un frame libre

	uint32_t index = array_frame_ppal_libre();
	t_nodo_frame_ppal* frame = &array_frame_ppal[index];

	if(index == -1) {
		log_debug(logger, "Si frame esta nulo ejecuto la seleccion de victima copio el frame a memoria principal");
		int page = selection_victim_cache();
		frame = &array_frame_ppal[page];
		uint32_t offset = array_frame_virt[frame->referencia->frame_virtual].offset;
		memcpy(memoria_virt + offset, memoria_ppal + frame->offset, FRAME_SIZE);
		msync(memoria_virt + offset, 32, MS_SYNC);

		pthread_mutex_lock(&frame->referencia->mutex);
		frame->referencia->frame_principal = -1;
		pthread_mutex_unlock(&frame->referencia->mutex);
		frame->referencia = pagina;
		pagina->frame_principal = page;
		memcpy(memoria_ppal + frame->offset, &plato_tmp_swap, FRAME_SIZE);

		log_debug(logger, "Se selecciona el frame nro: %d para intercambiar en mem ppal", page);
	}

	if (index != -1) {
		frame->referencia = pagina;
		pagina->frame_principal = index;
		memcpy(memoria_ppal + frame->offset, &plato_tmp_swap, FRAME_SIZE);
	}

	pthread_mutex_unlock(&array_frame_ppal_mutex);

	add_page(frame->referencia->frame_principal,true);

	pthread_mutex_unlock(&pagina->mutex);
	log_info(logger, "Terminado swap in desde %d hacia %d", pagina->frame_virtual, pagina->frame_principal);
}

t_plato_comanda* memoria_get_plato(uint32_t frame_index) {
	t_plato_comanda *plato_tmp = NULL;
	plato_tmp = calloc(FRAME_SIZE, 1);
	log_debug(logger, "Obtengo el plato frame_index %d", frame_index);
	memcpy(plato_tmp, memoria_ppal + array_frame_ppal[frame_index].offset, FRAME_SIZE);
	if(string_is_empty(plato_tmp->plato)){
		log_error(logger, "Plato vacio");
	}

	log_debug(logger, "Plato leido memoria_get_plato %d %d %s", plato_tmp->cantidad, plato_tmp->cantidad_lista,
			plato_tmp->plato);

	return plato_tmp;
}

uint32_t array_frame_ppal_libre() {
	log_debug(logger, "Buscando frame de memoria principal libre");
	uint32_t result = -1;
	for (int i = 0; i < configuracion->memoria_max_frames; i++) {
		if (!array_frame_ppal[i].ocupado) {
			array_frame_ppal[i].ocupado = true;
			result = i;
			log_debug(logger, "Encontre un resultado en MP buscando frame. N %d", i);
			break;
		}
	}

	return result;
}

uint32_t array_frame_virt_libre() {
	log_debug(logger, "Buscando frame de memoria virtual libre");
	uint32_t result = -1;
	for (int i = 0; i < configuracion->swap_max_frames; i++) {
		if (!array_frame_virt[i].ocupado) {
			array_frame_virt[i].ocupado = true;
			result = i;
			log_debug(logger, "Encontre un resultado en MV buscando frame. N %d", i);
			break;
		}
	}
	return result;
}

t_list* platos_obtener_platos(t_paginas *paginas) {
	t_list *platos = list_create();

	// Recorro los platos buscando por nombre
	pthread_mutex_lock(&paginas->mutex);
	int size = list_size(paginas->pagina_list);
	for (int i = 0; i < size; i++) {
		t_nodo_pagina *pagina = NULL;
		pagina = list_get(paginas->pagina_list, i);
		if (pagina->frame_principal == -1) {
			memoria_page_swap_in(pagina);
		}
		// Obtengo de la memoria el plato
		t_plato_comanda *plato = memoria_get_plato(pagina->frame_principal);

		t_estado_plato * estado_plato = estado_plato_create(plato->plato, plato->cantidad, plato->cantidad_lista);
		list_add(platos, estado_plato);
		free(plato);
	}
	pthread_mutex_unlock(&paginas->mutex);
	return platos;
}


