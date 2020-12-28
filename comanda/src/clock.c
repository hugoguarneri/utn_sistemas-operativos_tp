#include "clock.h"


void clock_init() {
	clock_modificado = malloc(sizeof(cache_clock));
	clock_modificado->clock_list = list_create();
	//((t_clock*)clock_modificado->clock_list->head->data)->page = -1;
	pthread_mutex_init(&clock_modificado->clock_lock,NULL);
	TAMANIO_CLOCK = configuracion->memoria_max_frames;
	POINTER_INDEX = 0;
	/*_Bool init_page(void *data) {
		t_clock *clock = (t_clock*)data;
		clock->page = -1;
	}
	list_iterate(clock_modificado,)*/
}

void clock_add_updated(int page,bool modificado) {

	pthread_mutex_lock(&clock_modificado->clock_lock);

	//busco si existe
	int index = -1;
	for(int i=0;i<clock_modificado->clock_list->elements_count;i++){
		t_clock *clock = (t_clock*)list_get(clock_modificado->clock_list,i);
		if(clock->page == (uint32_t)page){
			index = i;
			log_debug(logger, "[CMJ] frame encontrado N %d ",page);
		}

	}
	if(index == -1 && list_size(clock_modificado->clock_list) < TAMANIO_CLOCK){
		list_add(clock_modificado->clock_list,new_clock(page,modificado));
		log_debug(logger, "[CMJ] agrego el frame %d ",page);
	}
	if(index != -1 && TAMANIO_CLOCK < list_size(clock_modificado->clock_list)){
		t_clock* clock_en_uso = (t_clock*)list_get(clock_modificado->clock_list,index);
		clock_en_uso->uso = true;
		clock_en_uso->modificado = modificado;
		log_debug(logger, "[CMJ] Agrego el uso en el frame %d ",page);

	}

	pthread_mutex_unlock(&clock_modificado->clock_lock);
}

int clock_get_candidate(){
	_Bool sin_modificacion_y_sin_uso(void *clock_data){
		t_clock *cacheClock = (t_clock*)clock_data;
		return(!cacheClock->modificado && !cacheClock->uso);
	}
	_Bool con_modificacion_y_sin_uso(void* clock_data){
		t_clock *cacheClock = (t_clock*)clock_data;
		return(cacheClock->modificado && !cacheClock->uso);
	}
	pthread_mutex_lock(&clock_modificado->clock_lock);
	int index = find_element_page(sin_modificacion_y_sin_uso);
	if(index != -1){
		log_debug(logger, "[CMJ] Page index N %d ,elegido para reemplazo",index);
	}
	if(index == -1){
		//TODO
		//no hay ninguno que este listo para reemplazar
		//entonces recorro en busca de que halla alguno sin uso pero modificado
		log_debug(logger, "[CMJ] Buscando sin uso");
		index = find_element_page(con_modificacion_y_sin_uso);
		if(index != -1){
			log_debug(logger, "[CMJ] Page index N %d ,elegido para reemplazo",index);
		}
	}
	if(index == -1){
		//TODO
		//si sigue estando en null es porque estan todos en uso, entonces, doy una vuelta en toda la lista
		//y pongo todo en los elementos sin uso y selecciono al que este parado en la t_list
		_Bool set_usos(t_clock* data){
			data->uso = false;
			return false;
		}
		find_element_page((void*)set_usos);
		log_debug(logger, "[CMJ] No hay sin uso, haciendo clock");
		index = find_element_page(con_modificacion_y_sin_uso);
		if(index != -1){
			log_debug(logger, "[CMJ] Page index N %d ,elegido para reemplazo",index);
		}
	}
	if(index == -1){
		index = find_element_page(sin_modificacion_y_sin_uso);
		log_debug(logger, "[CMJ] Page index N %d ,elegido para reemplazo",index);
	}
	t_clock *clock_victima = (t_clock*) list_get(clock_modificado->clock_list,index);
	clock_victima->uso =true;
	clock_victima->modificado = false;
	POINTER_INDEX = index + 1;

	if(index == -1){
		log_debug(logger, "[CMJ] Problemas al seleccionar la victima");
	}
	pthread_mutex_unlock(&clock_modificado->clock_lock);

	return clock_victima->page;
}

int find_element_page(bool (*condition)(void*)){
	int index = -1;

	/*
	 * Recorro desde el puntero hasta el final de la lista
	 */
	for(int i=POINTER_INDEX;i<clock_modificado->clock_list->elements_count;i++){
		t_clock *clock = (t_clock*)list_get(clock_modificado->clock_list,i);
		if(condition(clock)){
			index = i;
			break;
		}
	}
	/*
	 * Luego recorro desde el principio hasta el puntero para terminar de dar una vuelta.
	 */
	if(index == -1){
		for(int i=0;i<POINTER_INDEX;i++){
			t_clock *clock = (t_clock*)list_get(clock_modificado->clock_list,i);
			if(condition(clock)){
				index = i;
				break;
			}
		}
	}

	return index;
}

void *new_clock(int page, bool modificado){
	t_clock *new = calloc(sizeof(t_clock),1);
	new->modificado = modificado;
	new->uso = true;
	new->page = (uint32_t) page;
	return (void*)new;
}

void clock_remove(int page){
	//todo remover el data de un elemento de la lista del clock_modificado
	void search_page(void* data){
		t_clock *clock = ((t_clock*)data);
		if(clock->page == page){
			clock->modificado = false;
			clock->uso = false;
		}
	}
	pthread_mutex_lock(&clock_modificado->clock_lock);
	list_iterate(clock_modificado->clock_list,search_page);
	pthread_mutex_unlock(&clock_modificado->clock_lock);
}

void clock_destroy(){
	//destroy cache clock
}
