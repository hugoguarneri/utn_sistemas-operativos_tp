#include "restaurante.h"

// [NOMBRE] [CANTIDAD_COCINEROS] [POSICION] [AFINIDAD_COCINEROS] [PLATOS] [PRECIO_PLATOS] [CANTIDAD_HORNOS]
void free_crear_rest_ctx(t_crear_rest_ctx *datos) {
	if (datos->nombre != NULL)
		free(datos->nombre);
	if (datos->afinidades != NULL)
		free(datos->afinidades);
	if (datos->platos != NULL)
		free(datos->platos);
	if (datos->precios != NULL)
		free(datos->precios);
	if (datos->posicion != NULL)
		free(datos->posicion);
	free(datos);
}

char* ejecutar_crear_restaurante(char *nombre, char *cantidad_cocineros, char *posicion, char *afinidad, char *platos,
		char *precio_platos, char *cantidad_hornos) {

	if (!string_is_alpha(nombre)) {
		log_info(logger, "El formato del nombre es incorrecto");
		return FAIL;
	}

	if (!string_is_number(cantidad_cocineros)) {
		log_info(logger, "El formato de cantidad de cocineros es incorrecto");
		return FAIL;
	}

	if (!is_comma_separated_number(posicion)) {
		log_info(logger, "El formato de la posicion es incorrecto");
		return FAIL;
	}

	if (!is_comma_separated_string(afinidad)) {
		log_info(logger, "El formato de la afinidad es incorrecto");
		return FAIL;
	}

	if (!is_comma_separated_string(platos)) {
		log_info(logger, "El formato de los platos es incorrecto");
		return FAIL;
	}

	if (!is_comma_separated_number(precio_platos)) {
		log_info(logger, "El formato de los precios de platos son incorrectos");
		return FAIL;
	}

	if (!string_is_number(cantidad_hornos)) {
		log_info(logger, "El formato de la cantidad de hornos es incorrecto");
		return FAIL;
	}

	t_crear_rest_ctx *datos = malloc(sizeof(t_crear_rest_ctx));
	log_debug(logger, "interpretando crearrestaurante %s %s %s %s %s %s %s", nombre, cantidad_cocineros, posicion,
			afinidad, platos, precio_platos, cantidad_hornos);
	datos->nombre = string_duplicate(nombre);
	datos->cantidad_cocineros = atoi(cantidad_cocineros);
	datos->posicion = string_duplicate(posicion);
	datos->afinidades = string_duplicate(afinidad);
	datos->platos = string_duplicate(platos);
	datos->precios = string_duplicate(precio_platos);
	datos->cantidad_hornos = atoi(cantidad_hornos);

	if (datos->cantidad_cocineros < 1) {
		log_info(logger, "Cantidad de cocineros invalida");
		free_crear_rest_ctx(datos);
		return FAIL;
	}

	if (datos->cantidad_hornos < 1) {
		log_info(logger, "Cantidad de hornos invalida");
		free_crear_rest_ctx(datos);
		return FAIL;
	}

	int count_afinidades = string_count_char(afinidad, ',');

	if ((datos->cantidad_cocineros - 1) < count_afinidades) {
		log_info(logger, "La cantidad de cocineros no coincide con las afinidades");
		free_crear_rest_ctx(datos);
		return FAIL;
	}

	int count_platos = string_count_char(platos, ',') + 1;
	int count_precios = string_count_char(precio_platos, ',') + 1;

	if (count_platos < 1 || count_precios < 1) {
		log_info(logger, "Cantidad de platos o precios invalidos");
		free_crear_rest_ctx(datos);
		return FAIL;
	}

	if (count_platos != count_precios) {
		log_info(logger, "La cantidad de platos no coincide con la cantidad de precios");
		free_crear_rest_ctx(datos);
		return FAIL;
	}

	char *result = afip_crear_restaurante(datos);
	free_crear_rest_ctx(datos);

	return result;
}

char* afip_crear_restaurante(t_crear_rest_ctx *data) {

	// Chequeo si existe el archivo info
	char *binFile = string_from_format("%s/%s/Info%s", afip_context.restaurantes_dir, data->nombre, AFIP_EXTENSION);
	if (is_file_accesible(binFile)) {
		// Si existe, salir por error
		log_warning(logger, "El archivo %s ya existe");
		free(binFile);
		return FAIL;
	}

	// Lock del registro de restaurantes
	sync_dictionary_lock(restaurantes_dict);

	// Verifico que no exista el restaurante en el registro, sino unlock y error
	bool exists = dictionary_has_key(restaurantes_dict->dict, data->nombre);
	if (exists) {
		sync_dictionary_unlock(restaurantes_dict);
		free(binFile);
		return FAIL;
	}
	pthread_rwlock_t *lock = malloc(sizeof(pthread_rwlock_t));
	pthread_rwlock_init(lock, NULL);
	pthread_rwlock_wrlock(lock);
	dictionary_put(restaurantes_dict->dict, data->nombre, lock);
	sync_dictionary_unlock(restaurantes_dict);

	// Creo el directorio
	char *dirFile = string_from_format("%s/%s", afip_context.restaurantes_dir, data->nombre);
	if (access(dirFile, R_OK | W_OK) != 0) {
		if (mkdir(dirFile, 0755) != 0) {
			free(dirFile);
			free(binFile);
			pthread_rwlock_unlock(lock);
			return FAIL;
		}
	}
	free(dirFile);

	// Creo el archivo con los datos
	char *stream =
			string_from_format(
					"CANTIDAD_COCINEROS=%d\nPOSICION=%s\nAFINIDAD_COCINEROS=%s\nPLATOS=%s\nPRECIO_PLATOS=%s\nCANTIDAD_HORNOS=%d",
					data->cantidad_cocineros, data->posicion, data->afinidades, data->platos, data->precios,
					data->cantidad_hornos);

	int stream_length = strlen(stream);
	log_debug(logger, "El largo del stream [%s] es %d", stream, stream_length);

	int cantidad_bloques = afip_calcular_cantidad_bloques(stream_length);

	// Obtener los numeros de bloques
	log_debug(logger, "El archivo [%s] se guardara en %d bloque/s", binFile, cantidad_bloques);
	t_list *bloques = afip_block_array_insert(cantidad_bloques);

	// Guardar los bloques
	afip_guardar_bloques(bloques, stream);

	uint32_t * first = list_get(bloques, 0);
	afip_guardar_inicio(true, binFile, *first, stream_length);

	char **array_platos = string_get_string_as_array(data->platos);
	char **array_precio_platos = string_get_string_as_array(data->precios);
	int i = 0;
	while (array_platos[i] != NULL) {
		precios_set_precio(data->nombre, array_platos[i], atoi(array_precio_platos[i]));
		i++;
	}

	free_string_array(array_platos);
	free_string_array(array_precio_platos);

	free(stream);
	list_destroy_and_destroy_elements(bloques, free);

	// Libero el lock para que puedan acceder
	pthread_rwlock_unlock(lock);

	return OK;
}

pthread_rwlock_t* afip_get_lock_restaurante(char *nombre) {
	pthread_rwlock_t *result = NULL;
	bool existe = false;
	sync_dictionary_lock(restaurantes_dict);
	existe = dictionary_has_key(restaurantes_dict->dict, nombre);
	if (!existe) {
		sync_dictionary_unlock(restaurantes_dict);
		return NULL;
	}
	result = dictionary_get(restaurantes_dict->dict, nombre);
	sync_dictionary_unlock(restaurantes_dict);

	return result;
}

char* afip_obtener_restaurante(char *nombre) {
	// Verifico si existe
	if (!existe_restaurante(nombre)) {
		return FAIL;
	}

	pthread_rwlock_t *lock = afip_get_lock_restaurante(nombre);

	if (lock == NULL)
		return FAIL;

	pthread_rwlock_rdlock(lock);

	char *binFile = string_from_format("%s/%s/Info%s", afip_context.restaurantes_dir, nombre, AFIP_EXTENSION);

	char *result = afip_leer_archivo(binFile);
	free(binFile);

	pthread_rwlock_unlock(lock);

	return result;
}

bool existe_restaurante(char *restaurante) {

	log_debug(logger, "Verificando la existencia del restaurante %s", restaurante);
	// Lock del registro de restaurantes
	sync_dictionary_lock(restaurantes_dict);

	// Verifico que no exista el restaurante en el registro, sino unlock y error
	bool exists = dictionary_has_key(restaurantes_dict->dict, restaurante);
	if (!exists) {
		sync_dictionary_unlock(restaurantes_dict);
		log_warning(logger, "El restaurante %s no existe en el directory", restaurante);
		return false;
	}

	sync_dictionary_unlock(restaurantes_dict);

	char *binFile = string_from_format("%s/%s/Info%s", afip_context.restaurantes_dir, restaurante, AFIP_EXTENSION);
	if (!is_file_accesible(binFile)) {
		// Si existe, salir por error
		log_warning(logger, "El archivo restaurante %s no existe");
		free(binFile);
		return false;
	}

	free(binFile);

	log_debug(logger, "Existe el restaurante %s", restaurante);

	return true;
}

uint32_t afip_obtener_cantidad_pedidos(char *restaurante) {
	uint32_t cantidad_pedidos = 0;
	log_debug(logger, "Obteniendo la cantidad de pedidos del restaurante %s", restaurante);

	DIR *dirp;
	struct dirent *entry;

	char *carpeta_restaurante = string_from_format("%s/%s", afip_context.restaurantes_dir, restaurante);

	dirp = opendir(carpeta_restaurante);
	while ((entry = readdir(dirp)) != NULL) {
		if (entry->d_type == DT_REG && !string_starts_with(entry->d_name, "pedido")) {
			cantidad_pedidos++;
		}
	}
	closedir(dirp);
	free(carpeta_restaurante);

	log_debug(logger, "Restaurante %s - cantidad de pedidos: %d", restaurante, cantidad_pedidos);
	return cantidad_pedidos;
}

