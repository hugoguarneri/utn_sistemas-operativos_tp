#include "algoritmo-reemplazo.h"


void init_algoritmo_reemplazo(){
	cache_system = configuracion->algoritmo;
	if(cache_system == LRU){
		lru_init();
		log_info(logger,"Algoritmo de reemplazo seleccionado LRU");
	}
	if(cache_system == CMJ){
		clock_init();
		log_info(logger,"Algoritmo de reemplazo seleccionado: CMJ");
	}
}

void clear_cache_paginas(t_paginas *paginas){
	void remove_cache(t_nodo_pagina* nodo){
		if(cache_system == LRU){
			pthread_mutex_lock(&nodo->mutex);
			if(nodo->frame_principal != -1){
				lru_remove((int)nodo->frame_principal);
				log_debug(logger, "[LRU] Pagina Nro: %d removida",nodo->frame_principal);
			}
			pthread_mutex_unlock(&nodo->mutex);
		}
		if(cache_system == CMJ){
			pthread_mutex_lock(&nodo->mutex);
			if(nodo->frame_principal != -1){
				clock_remove((int)nodo->frame_principal);
				log_debug(logger, "[CMJ] Pagina Nro: %d removida",nodo->frame_principal);
			}
			pthread_mutex_unlock(&nodo->mutex);
		}
	}
	list_iterate(paginas->pagina_list,(void*)remove_cache);
}

uint32_t selection_victim_cache(){
	uint32_t page = -1;
	if(cache_system==LRU){
		page = (uint32_t) lru_get_candidate();
		log_debug(logger, "[LRU] Candidato elegido Nro: %d",page);
		return page;
	}
	if(cache_system==CMJ){
		page = clock_get_candidate();
		return page;
	}
	if(page == -1){
		log_error(logger,"Hubo un error al seleccionar la victima");
	}
	return page;
}

void add_page(int32_t pagina,bool modificado){

	if(cache_system == CMJ){
		clock_add_updated((int) pagina,modificado);
	}
	if(cache_system == LRU){
		lru_add_updated((int) pagina);
		log_debug(logger, "[LRU] agrego la pagina %d ",pagina);
	}


}


