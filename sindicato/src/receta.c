#include "receta.h"

void crear_receta_ctx(t_crear_receta_ctx *datos) {
	if (datos->nombre != NULL)
		free(datos->nombre);
	if (datos->pasos != NULL)
		free(datos->pasos);
	if (datos->tiempos != NULL)
		free(datos->tiempos);
	free(datos);
}

void free_crear_receta_ctx(t_crear_receta_ctx *data) {
	if (data->nombre != NULL)
		free(data->nombre);
	if (data->pasos != NULL)
		free(data->pasos);
	if (data->tiempos != NULL)
		free(data->tiempos);
	free(data);
}

char* ejecutar_crear_receta(char *nombre, char *pasos, char *tiempo_pasos) {

	if (!string_is_alpha(nombre)) {
		printf("El formato del nombre es incorrecto");
		return FAIL;
	}

	if (!is_comma_separated_string(pasos)) {
		printf("El formato de los pasos es incorrecto");
		return FAIL;
	}

	if (!is_comma_separated_number(tiempo_pasos)) {
		printf("El formato del tiempo de los pasos es incorrecto");
		return FAIL;
	}
	t_crear_receta_ctx *datos = malloc(sizeof(t_crear_receta_ctx));
	log_debug(logger, "interpretando crearreceta %s %s %s", nombre, pasos, tiempo_pasos);
	// parseo los pasos y sus tiempos

	datos->nombre = string_duplicate(nombre);
	datos->pasos = string_duplicate(pasos);
	datos->tiempos = string_duplicate(tiempo_pasos);
	int count_pasos = string_count_char(pasos, ',');
	int count_tiempos = string_count_char(tiempo_pasos, ',');

	if (count_pasos < 1) {
		free_crear_receta_ctx(datos);
		log_warning(logger, "La cantidad pasos es insuficiente");
		return FAIL;
	}
	if (count_tiempos != count_pasos) {
		log_warning(logger, "La cantidad pasos no coincide con la cantidad de tiempos %d %d", count_pasos,
				count_tiempos);
		free_crear_receta_ctx(datos);
		printf("La cantidad de pasos y tiempos no coinciden\n");
		return FAIL;
	}

	char *result = afip_crear_receta(datos);

	free_crear_receta_ctx(datos);
	return result;

}

char* afip_crear_receta(t_crear_receta_ctx *data) {
	// Chequeo si existe el archivo
	char *binFile = string_from_format("%s/%s%s", afip_context.recetas_dir, data->nombre, AFIP_EXTENSION);
	if (is_file_accesible(binFile)) {
		// Si existe, salir por error
		log_warning(logger, "El archivo %s ya existe");
		return FAIL;
	}

	// lock mutex directorio
	log_debug(logger, "Tomando lock de recetas dictionary");
	sync_dictionary_lock(recetas_dict);

	// verifico existencia
	if (dictionary_has_key(recetas_dict->dict, data->nombre)) {
		// Si existe, libero mutex y devuelvo error
		sync_dictionary_unlock(recetas_dict);
		log_warning(logger, "El archivo %s ya existe, se esta creando", data->nombre);
		return FAIL;
	}

	// inicializo la condicion e inserto nodo
	pthread_cond_t *condition = NULL;
	condition = malloc(sizeof(pthread_cond_t));
	pthread_cond_init(condition, NULL);
	dictionary_put(recetas_dict->dict, data->nombre, condition);
	log_debug(logger, "Receta insertada en ");
	// unlock mutex directorio
	sync_dictionary_unlock(recetas_dict);

	// armar el stream de datos que va a ir al archivo
	char *stream = string_from_format("PASOS=%s\nTIEMPO_PASOS=%s", data->pasos, data->tiempos);

	// Calcular la cantidad de bloques a utilizar
	int stream_length = strlen(stream);
	log_debug(logger, "El largo del stream [%s] es %d", stream, stream_length);

	int cantidad_bloques = afip_calcular_cantidad_bloques(stream_length);

	// Obtener los numeros de bloques
	log_debug(logger, "El archivo [%s] se guardara en %d bloque/s", binFile, cantidad_bloques);
	t_list *bloques = afip_block_array_insert(cantidad_bloques);

	// Guardar los bloques
	afip_guardar_bloques(bloques, stream);

	// Al terminar de guardar todo se guarda el archivo bin inicial
	// Guardar en el archivo los datos para acceder al primer lugar
	uint32_t * first = list_get(bloques, 0);
	afip_guardar_inicio(true, binFile, *first, stream_length);

	free(stream);
	list_destroy_and_destroy_elements(bloques, free);

	// Elimino el registro del archivo que estaba creando
	sync_dictionary_lock(recetas_dict);
	dictionary_remove(recetas_dict->dict, data->nombre);
	sync_dictionary_unlock(recetas_dict);

	// Libero la condicion para que despierten los que estan esperando.
	pthread_cond_broadcast(condition);
	free(condition);

	return OK;

}

char* afip_obtener_receta(char *nombre) {

	// Mutex lock del directorio
	log_debug(logger, "Tomando lock de recetas dictionary");
	sync_dictionary_lock(recetas_dict);

	// Verifico si existe en el directorio
	if (dictionary_has_key(recetas_dict->dict, nombre)) {
		// Existe, entonces espero la condicion para esperar que creen el archivo
		pthread_cond_t* condicion = dictionary_get(recetas_dict->dict, nombre);
		log_warning(logger, "El archivo %s se esta creando", nombre);
		pthread_cond_wait(condicion, &(recetas_dict->mutex));
	}

	// libero el mutex, porque ya se puede continuar
	sync_dictionary_unlock(recetas_dict);

	char *binFile = string_from_format("%s/%s%s", afip_context.recetas_dir, nombre, AFIP_EXTENSION);
	if (!is_file_accesible(binFile)) {
		// Si NO existe, salir por error
		log_warning(logger, "El archivo %s no ya existe ", binFile);
		free(binFile);
		return FAIL;
	}

	char *result = afip_leer_archivo(binFile);

	free(binFile);

	return result;
}

