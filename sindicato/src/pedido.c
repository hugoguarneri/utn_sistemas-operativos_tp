#include "pedido.h"

void pedido_data_destroy(t_pedido_data *data) {
	free(data->estado);
	list_destroy_and_destroy_elements(data->platos, (void*) estado_plato_destroy);
	free(data);
}

status_code afip_guardar_pedido(char *restaurante, char *id_pedido) {
	//Verificar si el Restaurante existe dentro del File System
	if (!existe_restaurante(restaurante)) {
		// En caso de no existir restaurante se deberá informar dicha situación
		return FAIL_;
	}

	//armo la clave del pedido
	char *clave = string_from_format("%s_pedido%s", restaurante, id_pedido);

	sync_dictionary_lock(pedidos_dict);

	//Verificar que el ID de pedido no exista para dicho restaurante.
	if (dictionary_has_key(pedidos_dict->dict, clave)) {
		//En caso de existir se deberá informar sobre dicha situación.
		log_warning(logger, "El pedido %s del restaurante %s ya existia, no se puede crear", id_pedido, restaurante);
		sync_dictionary_unlock(pedidos_dict);
		free(clave);
		return FAIL_;
	}

	//armo el nombre de archivo del pedido
	char *archivo_pedido = get_path_pedido(restaurante, id_pedido);
	if (is_file_accesible(archivo_pedido)) {
		sync_dictionary_unlock(pedidos_dict);
		log_warning(logger, "El archivo pedido %s ya existe", archivo_pedido);
		free(archivo_pedido);
		free(clave);
		return FAIL_;
	}

	t_pedido_item *value = pedido_value_create();
	pthread_rwlock_wrlock(value->rwlock);
	dictionary_put(pedidos_dict->dict, clave, value);
	sync_dictionary_unlock(pedidos_dict);

	// Creo un contenido vacio para el archivo

	char *stream = string_from_format(
			"ESTADO_PEDIDO=%s\nLISTA_PLATOS=%s\nCANTIDAD_PLATOS=%s\nCANTIDAD_LISTA=%s\nPRECIO_TOTAL=%d",
			STR_ESTADO_PENDIENTE,
			ARRAY_VACIO,
			ARRAY_VACIO,
			ARRAY_VACIO, 0);

	// En caso de que no exista, se deberá crear el archivo
	int stream_length = strlen(stream);
	log_debug(logger, "El largo del stream [%s] es %d", stream, stream_length);

	int cantidad_bloques = afip_calcular_cantidad_bloques(stream_length);

	// Obtener los numeros de bloques
	log_debug(logger, "El archivo [%s] se guardara en %d bloque/s", archivo_pedido, cantidad_bloques);
	t_list *bloques = afip_block_array_insert(cantidad_bloques);

	// Guardar los bloques
	afip_guardar_bloques(bloques, stream);

	// Al terminar de guardar todo se guarda el archivo bin inicial
	// Guardar en el archivo los datos para acceder al primer lugar
	int * first = list_get(bloques, 0);
	afip_guardar_inicio(true, archivo_pedido, *first, stream_length);

	pthread_rwlock_unlock(value->rwlock);

	free(archivo_pedido);
	free(clave);
	free(stream);
	list_destroy_and_destroy_elements(bloques, free);

	// Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	return OK_;
}

char* get_path_pedido(char *restaurante, char *id_pedido) {
	char *binFile = string_from_format("%s/%s/pedido%s%s", afip_context.restaurantes_dir, restaurante, id_pedido,
	AFIP_EXTENSION);
	return binFile;
}

status_code afip_guardar_plato(char *restaurante, char *id_pedido, char *comida, int cantidad) {
	//Verificar si el Restaurante existe dentro del File System
	if (!existe_restaurante(restaurante)) {
		// En caso de no existir restaurante se deberá informar dicha situación
		return FAIL_;
	}

	//armo la clave del pedido
	char *clave = string_from_format("%s_pedido%s", restaurante, id_pedido);

	sync_dictionary_lock(pedidos_dict);

	if (!dictionary_has_key(pedidos_dict->dict, clave)) {
		//En caso de existir se deberá informar sobre dicha situación.
		log_warning(logger, "El pedido %s del restaurante %s no existe", id_pedido, restaurante);
		sync_dictionary_unlock(pedidos_dict);
		free(clave);
		return FAIL_;
	}

	//armo el nombre de archivo del pedido
	char *archivo_pedido = get_path_pedido(restaurante, id_pedido);
	if (!is_file_accesible(archivo_pedido)) {
		sync_dictionary_unlock(pedidos_dict);
		log_warning(logger, "El archivo pedido %s no existe", archivo_pedido);
		free(archivo_pedido);
		free(clave);
		return FAIL_;
	}

	t_pedido_item *value = dictionary_get(pedidos_dict->dict, clave);
	if (value->estado != Pendiente) {
		sync_dictionary_unlock(pedidos_dict);
		log_warning(logger, "El estado del pedido %s no es pendiente", archivo_pedido);
		free(archivo_pedido);
		free(clave);
		return FAIL_;
	}

	pthread_rwlock_wrlock(value->rwlock);
	sync_dictionary_unlock(pedidos_dict);

	t_archivo_bloques *data = afip_leer_archivo_con_bloques(archivo_pedido);

	t_pedido_data *pedido_data = stream_to_pedido(data->stream);
	free(data->stream);

	pedido_data_agregar_plato(restaurante, pedido_data, comida, cantidad);

	data->stream = pedido_to_stream(pedido_data);

	int stream_length = strlen(data->stream);
	int cantidad_bloques = afip_calcular_cantidad_bloques(stream_length);
	afip_block_array_update(data->bloques, cantidad_bloques);
	afip_guardar_bloques(data->bloques, data->stream);

	int * first = list_get(data->bloques, 0);
	afip_guardar_inicio(false, archivo_pedido, *first, stream_length);

	pthread_rwlock_unlock(value->rwlock);
	list_destroy_and_destroy_elements(data->bloques, free);
	free(data->stream);
	free(data);
	free(archivo_pedido);
	free(clave);
	return OK_;
}

void pedido_data_agregar_plato(char *restaurante, t_pedido_data *pedido_data, char *comida, int cantidad) {
	t_estado_plato *plato = NULL;
	log_debug(logger, "Buscando la comida %s en el pedido", comida);
	// Busco el nodo para la comida propuesta.
	bool condicion_busqueda(t_estado_plato *item) {
		return strcmp(item->comida, comida) == 0;
	}

	plato = list_find(pedido_data->platos, (void*) condicion_busqueda);

	if (plato == NULL) {
		// Si no existe le agrego el plato a la lista con el valor de cantidad y terminado en 0
		log_debug(logger, "No encontre la comida %s en el pedido", comida);
		plato = calloc(sizeof(t_estado_plato), 1);
		plato->comida = strdup(comida);
		plato->cant_total = cantidad;
		plato->cant_lista = 0;
		list_add(pedido_data->platos, plato);
	} else {
		// Si existe le agrego la cantidad
		log_debug(logger, "Encontre la comida %s en el pedido", comida);
		plato->cant_total += cantidad;
	}

	// Busco y sumo el precio de lo agregado con el existente
	int precio_unitario = precios_get_precio(restaurante, comida);
	pedido_data->precio_total += (precio_unitario * cantidad);

}

char* pedido_to_stream(t_pedido_data *pedido_data) {
	char *platos = NULL;
	char *cant_total = NULL;
	char *cant_lista = NULL;
	char *stream = NULL;

	platos = string_new();
	cant_total = string_new();
	cant_lista = string_new();

	void armar_array_plato(t_estado_plato *item) {
		if (strlen(platos) > 0)
			string_append(&platos, ",");
		if (strlen(cant_total) > 0)
			string_append(&cant_total, ",");
		if (strlen(cant_lista) > 0)
			string_append(&cant_lista, ",");

		string_append(&platos, item->comida);
		string_append_with_format(&cant_total, "%d", item->cant_total);
		string_append_with_format(&cant_lista, "%d", item->cant_lista);
	}

	list_iterate(pedido_data->platos, (void*) armar_array_plato);

	stream = string_from_format(
			"ESTADO_PEDIDO=%s\nLISTA_PLATOS=[%s]\nCANTIDAD_PLATOS=[%s]\nCANTIDAD_LISTA=[%s]\nPRECIO_TOTAL=%d",
			pedido_data->estado, platos, cant_total, cant_lista, pedido_data->precio_total);

	pedido_data_destroy(pedido_data);

	free(platos);
	free(cant_total);
	free(cant_lista);

	return stream;
}

status_code afip_plato_listo(char *restaurante, char *id_pedido, char *comida) {
	//Verificar si el Restaurante existe dentro del File System
	if (!existe_restaurante(restaurante)) {
		// En caso de no existir restaurante se deberá informar dicha situación
		return FAIL_;
	}

	//armo la clave del pedido
	char *clave = string_from_format("%s_pedido%s", restaurante, id_pedido);

	sync_dictionary_lock(pedidos_dict);

	if (!dictionary_has_key(pedidos_dict->dict, clave)) {
		//En caso de existir se deberá informar sobre dicha situación.
		log_warning(logger, "El pedido %s del restaurante %s no existe", id_pedido, restaurante);
		sync_dictionary_unlock(pedidos_dict);
		free(clave);
		return FAIL_;
	}

	//armo el nombre de archivo del pedido
	char *archivo_pedido = get_path_pedido(restaurante, id_pedido);
	if (!is_file_accesible(archivo_pedido)) {
		sync_dictionary_unlock(pedidos_dict);
		log_warning(logger, "El archivo pedido %s no existe", archivo_pedido);
		free(archivo_pedido);
		free(clave);
		return FAIL_;
	}

	t_pedido_item *value = dictionary_get(pedidos_dict->dict, clave);
	if (value->estado != Confirmado) {
		sync_dictionary_unlock(pedidos_dict);
		log_warning(logger, "El estado del pedido %s no es pendiente", archivo_pedido);
		free(archivo_pedido);
		free(clave);
		return FAIL_;
	}

	pthread_rwlock_wrlock(value->rwlock);
	sync_dictionary_unlock(pedidos_dict);

	t_archivo_bloques *data = afip_leer_archivo_con_bloques(archivo_pedido);

	t_pedido_data *pedido_data = stream_to_pedido(data->stream);
	free(data->stream);

	//TODO
	if (!pedido_data_plato_listo(pedido_data, comida)) {
		log_error(logger, "RARO!!! el pedido no tiene esa comida %s", comida);
		pthread_rwlock_unlock(value->rwlock);
		list_destroy_and_destroy_elements(data->bloques, free);
		free(data);
		free(archivo_pedido);
		free(clave);
		return FAIL_;
	}

	data->stream = pedido_to_stream(pedido_data);

	int stream_length = strlen(data->stream);
	int cantidad_bloques = afip_calcular_cantidad_bloques(stream_length);
	afip_block_array_update(data->bloques, cantidad_bloques);
	afip_guardar_bloques(data->bloques, data->stream);

	int * first = list_get(data->bloques, 0);
	afip_guardar_inicio(false, archivo_pedido, *first, stream_length);

	pthread_rwlock_unlock(value->rwlock);
	list_destroy_and_destroy_elements(data->bloques, free);
	free(data->stream);
	free(data);
	free(archivo_pedido);
	free(clave);
	return OK_;
}

bool pedido_data_plato_listo(t_pedido_data *pedido_data, char *comida) {
	t_estado_plato *plato = NULL;
	log_debug(logger, "Buscando la comida %s en el pedido",comida);
	// Busco el nodo para la comida propuesta.
	bool condicion_busqueda(t_estado_plato *item) {
		return strcmp(item->comida, comida) == 0;
	}

	plato = list_find(pedido_data->platos, (void*) condicion_busqueda);

	if (plato != NULL) {
		// Si existe le agrego la cantidad
		log_debug(logger, "Encontre la comida %s en el pedido",comida);
		plato->cant_lista += 1;
	} else
		return false;

	return true;
}

status_code afip_cambiar_estado_pedido(char *restaurante, char *id_pedido, estado_pedido actual, estado_pedido futuro,
		char *nombre_futuro) {
	//Verificar si el Restaurante existe dentro del File System
	if (!existe_restaurante(restaurante)) {
		// En caso de no existir restaurante se deberá informar dicha situación
		return FAIL_;
	}

	//armo la clave del pedido
	char *clave = string_from_format("%s_pedido%s", restaurante, id_pedido);

	sync_dictionary_lock(pedidos_dict);

	if (!dictionary_has_key(pedidos_dict->dict, clave)) {
		//En caso de existir se deberá informar sobre dicha situación.
		log_warning(logger, "El pedido %s del restaurante %s no existe", id_pedido, restaurante);
		sync_dictionary_unlock(pedidos_dict);
		free(clave);
		return FAIL_;
	}

	//armo el nombre de archivo del pedido
	char *archivo_pedido = get_path_pedido(restaurante, id_pedido);
	if (!is_file_accesible(archivo_pedido)) {
		sync_dictionary_unlock(pedidos_dict);
		log_warning(logger, "El archivo pedido %s no existe", archivo_pedido);
		free(archivo_pedido);
		free(clave);
		return FAIL_;
	}

	t_pedido_item *value = dictionary_get(pedidos_dict->dict, clave);
	if (value->estado != actual) {
		sync_dictionary_unlock(pedidos_dict);
		log_warning(logger, "El estado del pedido %s no es %d", archivo_pedido, actual);
		free(archivo_pedido);
		free(clave);
		return FAIL_;
	}

	pthread_rwlock_wrlock(value->rwlock);
	sync_dictionary_unlock(pedidos_dict);

	t_archivo_bloques *data = afip_leer_archivo_con_bloques(archivo_pedido);

	t_pedido_data *pedido_data = stream_to_pedido(data->stream);
	free(data->stream);

	//
	free(pedido_data->estado);
	pedido_data->estado = strdup(nombre_futuro);
	value->estado = futuro;

	data->stream = pedido_to_stream(pedido_data);

	int stream_length = strlen(data->stream);
	int cantidad_bloques = afip_calcular_cantidad_bloques(stream_length);
	afip_block_array_update(data->bloques, cantidad_bloques);
	afip_guardar_bloques(data->bloques, data->stream);

	int * first = list_get(data->bloques, 0);
	afip_guardar_inicio(false, archivo_pedido, *first, stream_length);

	pthread_rwlock_unlock(value->rwlock);
	list_destroy_and_destroy_elements(data->bloques, free);
	free(data->stream);
	free(data);
	free(archivo_pedido);
	free(clave);
	return OK_;

}

t_obtener_pedido_res* afip_recuperar_pedido(char *restaurante, char *id_pedido) {
	if (!existe_restaurante(restaurante)) {
		// En caso de no existir restaurante se deberá informar dicha situación
		return NULL;
	}

	//armo la clave del pedido
	char *clave = string_from_format("%s_pedido%s", restaurante, id_pedido);

	//armo el nombre de archivo del pedido
	char *archivo_pedido = get_path_pedido(restaurante, id_pedido);
	if (!is_file_accesible(archivo_pedido)) {
		log_warning(logger, "El archivo pedido %s no existe", archivo_pedido);
		free(archivo_pedido);
		free(clave);
		return NULL;
	}

	char *stream = afip_leer_archivo(archivo_pedido);

	t_pedido_data *pedido_data = stream_to_pedido(stream);
	t_obtener_pedido_res *result = NULL;
	if(strcmp(pedido_data->estado, STR_ESTADO_TERMINADO) != 0){
		e_estado_pedido estado = pedido_get_e_estado_pedido_from_string(pedido_data->estado);
		result = obtener_pedido_res_create(estado, pedido_data->platos);
	}


	free(pedido_data);
	free(stream);
	free(archivo_pedido);
	free(clave);

	return result;
}

t_obtener_pedido_res* afip_obtener_pedido(char *restaurante, char *id_pedido) {
	//Verificar si el Restaurante existe dentro del File System
	if (!existe_restaurante(restaurante)) {
		// En caso de no existir restaurante se deberá informar dicha situación
		return NULL;
	}

	//armo la clave del pedido
	char *clave = string_from_format("%s_pedido%s", restaurante, id_pedido);

	sync_dictionary_lock(pedidos_dict);

	if (!dictionary_has_key(pedidos_dict->dict, clave)) {
		//En caso de existir se deberá informar sobre dicha situación.
		log_warning(logger, "El pedido %s del restaurante %s no existe", id_pedido, restaurante);
		sync_dictionary_unlock(pedidos_dict);
		free(clave);
		return NULL;
	}

	//armo el nombre de archivo del pedido
	char *archivo_pedido = get_path_pedido(restaurante, id_pedido);
	if (!is_file_accesible(archivo_pedido)) {
		sync_dictionary_unlock(pedidos_dict);
		log_warning(logger, "El archivo pedido %s no existe", archivo_pedido);
		free(archivo_pedido);
		free(clave);
		return NULL;
	}

	t_pedido_item *value = dictionary_get(pedidos_dict->dict, clave);

	pthread_rwlock_rdlock(value->rwlock);
	sync_dictionary_unlock(pedidos_dict);

	char *stream = afip_leer_archivo(archivo_pedido);

	t_pedido_data *pedido_data = stream_to_pedido(stream);

	e_estado_pedido estado = pedido_get_e_estado_pedido_from_string(pedido_data->estado);
	t_obtener_pedido_res *result = obtener_pedido_res_create(estado, pedido_data->platos);

	free(pedido_data);
	pthread_rwlock_unlock(value->rwlock);
	free(stream);
	free(archivo_pedido);
	free(clave);

	return result;
}

// ESTADO_PEDIDO=%s\nLISTA_PLATOS=%s\nCANTIDAD_PLATOS=%s\nCANTIDAD_LISTA=%s\nPRECIO_TOTAL=%d
t_pedido_data* stream_to_pedido(char *data) {
	t_dictionary *dictionary = stream_to_dictionary(data);
	t_pedido_data *pedido_data = NULL;
	pedido_data = calloc(sizeof(t_pedido_data), 1);

	pedido_data->estado = strdup(dictionary_get(dictionary, "ESTADO_PEDIDO"));
	pedido_data->precio_total = dictionary_get_int_value(dictionary, "PRECIO_TOTAL");
	pedido_data->platos = list_create();
	char **lista_platos = dictionary_get_string_array_value(dictionary, "LISTA_PLATOS");
	char **cantidad_platos = dictionary_get_string_array_value(dictionary, "CANTIDAD_PLATOS");
	char **cantidad_lista = dictionary_get_string_array_value(dictionary, "CANTIDAD_LISTA");

	int index = 0;
	while (lista_platos[index] != NULL) {
		list_add(pedido_data->platos,
				estado_plato_create(lista_platos[index], atoi(cantidad_platos[index]), atoi(cantidad_lista[index])));
		index++;
	}
	free_string_array(lista_platos);
	free_string_array(cantidad_platos);
	free_string_array(cantidad_lista);
	dictionary_destroy_and_destroy_elements(dictionary, free);

	return pedido_data;
}
