#include "filesystem.h"
#include "utils.h"

void check_directory(char *basedir, char *ask);
void check_readable_file(char *basedir, char *ask);
_Bool is_file_accesible(char *path);
void afip_block_save();

void afip_check_directories_permissions(char *mount_point) {
	// chequeo que el punto de montaje exista y tenga permiso
	log_info(logger, "Chequeando punto de montaje %s", mount_point);

	log_debug(logger, "Chequeando directorio %s", mount_point);

	if (access(mount_point, R_OK | W_OK) != 0) {
		log_error(logger, "No se puede acceder al directorio %s", mount_point);
		exit(0);
	}

	check_directory(mount_point, DIR_METADATA);
	check_directory(mount_point, DIR_BLOCKS);
	check_directory(mount_point, DIR_RECETAS);
	check_directory(mount_point, DIR_RESTAURANTE);

	//chequeo si existe el archivo Metadata.AFIP
	check_readable_file(mount_point, FILE_METADATA);

}

void check_directory(char *basedir, char *ask) {
	char *aux = NULL;
	aux = string_duplicate(basedir);
	string_append_with_format(&aux, "/%s", ask);

	if (access(aux, R_OK | W_OK) != 0) {
		log_error(logger, "No se puede acceder al directorio %s", aux);
		free(aux);
		exit(EXIT_FAILURE);
	}
	free(aux);
}

void check_readable_file(char *basedir, char *ask) {
	char *aux = NULL;
	aux = string_duplicate(basedir);
	string_append_with_format(&aux, "/%s", ask);

	if (access(aux, R_OK) != 0) {
		log_error(logger, "No se puede leer el archivo %s", aux);
		free(aux);
		exit(EXIT_FAILURE);
	}
	free(aux);
}

void afip_initialize(char *basedir) {

	char *metadata_path = NULL;
	metadata_path = string_duplicate(basedir);
	string_append_with_format(&metadata_path, "/%s", FILE_METADATA);

	t_config *metadata_config = config_create(metadata_path);
	afip_context.metadata_path = metadata_path;

	char *magic = config_get_string_value(metadata_config, METADATA_MAGIC_NUMBER_KEY);
	if (strcmp(magic, "AFIP") != 0) {
		free(metadata_path);
		free(magic);
		config_destroy(metadata_config);
		log_error(logger, "Magic number invalido");
		exit(EXIT_FAILURE);
	}

	afip_context.block_size = config_get_int_value(metadata_config, METADATA_BLOCK_SIZE_KEY);
	afip_context.block_count = config_get_int_value(metadata_config, METADATA_BLOCKS_KEY);
	afip_context.bitmap_path = string_from_format("%s/%s/%s", basedir, DIR_METADATA, BITMAP_BIN);
	afip_context.blocks_dir = string_from_format("%s/%s", basedir, DIR_BLOCKS);
	afip_context.recetas_dir = string_from_format("%s/%s", basedir, DIR_RECETAS);
	afip_context.restaurantes_dir = string_from_format("%s/%s", basedir, DIR_RESTAURANTE);

	// si existe el archivo bitmap recupero su contenido.
	bzero(&afip_blocks, sizeof(t_afip_block_array));
	pthread_mutex_init(&afip_blocks.mutex, NULL);

	if (is_file_accesible(afip_context.bitmap_path)) {
		log_info(logger, "Recuperando archivo bitmap");
		struct stat info;
		bzero(&info, sizeof(struct stat));
		FILE *fptr;
		if (stat(afip_context.bitmap_path, &info) != 0) {
			log_error(logger, "No se puede hacer stat en el bitmap file");
			exit_failure();
		}
		if (info.st_size > 0) {
			char *content = calloc(info.st_size, sizeof(char));
			if ((fptr = fopen(afip_context.bitmap_path, "rb+")) != NULL) {
				size_t blocks_read = fread(content, info.st_size, 1, fptr);
				if (blocks_read != 1) {
					log_error(logger, "Error al leer el bitmap file");
					exit_failure();
				}
				afip_blocks.blocks = bitarray_create_with_mode(content, info.st_size, LSB_FIRST);
				afip_blocks.block_file = fptr;
			} else {
				log_error(logger, "Error al abrir el bitmap file");
				exit_failure();
			}
		}
	} else {
		log_info(logger, "Armando bitmap");
		char *content = calloc(afip_context.block_count, sizeof(char));
		afip_blocks.blocks = bitarray_create_with_mode(content, afip_context.block_count, LSB_FIRST);
		FILE *fptr;
		if ((fptr = fopen(afip_context.bitmap_path, "wb")) != NULL) {
			afip_blocks.block_file = fptr;
			afip_block_save();
		} else {
			log_error(logger, "Error al abrir el bitmap file");
			exit_failure();
		}
	}

	// Armo los archivos de bloques
	log_debug(logger, "Creando archivos de bloques");
	for (int i = 0; i < afip_context.block_count; i++) {
		char *block_file = string_from_format("%s/%s/%d%s", basedir, DIR_BLOCKS, i, AFIP_EXTENSION);
		if (!is_file_accesible(block_file)) {
			if (!create_empty_file(block_file)) {
				log_error(logger, "Error creando archivo de bloque");
				exit_failure();
			}
		}
		free(block_file);
	}
	log_debug(logger, "%d archivos de bloques creados", afip_context.block_count);

	// Creo diccionarios
	recetas_dict = create_sync_dictionary();

	restaurantes_dict = create_sync_dictionary();

	pedidos_dict = create_sync_dictionary();

}

void restaurantes_crear_item_restaurante(char *nombre) {
	pthread_rwlock_t *lock = malloc(sizeof(pthread_rwlock_t));
	pthread_rwlock_init(lock, NULL);
	dictionary_put(restaurantes_dict->dict, nombre, lock);
}

char* pedido_get_id_from_filename(char *filename) {
	uint32_t last = indexof(filename, '.');
	return string_substring(filename, 6, last - 6);
}

estado_pedido pedido_get_estado_from_string(char *estado_pedido_str) {
	if (string_equals_ignore_case(estado_pedido_str, STR_ESTADO_PENDIENTE)) {
		return Pendiente;
	} else if (string_equals_ignore_case(estado_pedido_str, STR_ESTADO_CONFIRMADO)) {
		return Confirmado;
	} else if (string_equals_ignore_case(estado_pedido_str, STR_ESTADO_TERMINADO)) {
		return Terminado;
	} else {
		log_error(logger, "Estado de pedido no valido verificar Filesystem %s", estado_pedido_str);
		exit_failure();
		return Terminado; // Solo para evitar el Warning
	}
}


e_estado_pedido pedido_get_e_estado_pedido_from_string(char *estado_pedido_str) {
	if (string_equals_ignore_case(estado_pedido_str, STR_ESTADO_PENDIENTE)) {
		return PENDIENTE;
	} else if (string_equals_ignore_case(estado_pedido_str, STR_ESTADO_CONFIRMADO)) {
		return CONFIRMADO;
	} else {
		log_error(logger, "Estado de pedido no valido verificar Filesystem %s", estado_pedido_str);
		exit_failure();
		return Terminado; // Solo para evitar el Warning
	}
}

void afip_create_caches() {
	precios_init();
	//
	struct dirent *files = NULL;
	struct dirent *file_pedido = NULL;
	DIR *dir = opendir(afip_context.restaurantes_dir);
	while ((files = readdir(dir)) != NULL) {
		if (files->d_type == DT_DIR && !string_starts_with(files->d_name, ".")) {
			log_debug(logger, "INIT: Agregando restaurante %s", files->d_name);
			restaurantes_crear_item_restaurante(files->d_name);
			// Carga de precios del restaurante
			char *stream = afip_obtener_restaurante(files->d_name);
			t_dictionary *restaurante_file = stream_to_dictionary(stream);
			char **array_platos = dictionary_get_string_array_value(restaurante_file, "PLATOS");
			char **array_precio_platos = dictionary_get_string_array_value(restaurante_file, "PRECIO_PLATOS");
			int i = 0;
			while (array_platos[i] != NULL) {
				precios_set_precio(files->d_name, array_platos[i], atoi(array_precio_platos[i]));
				i++;
			}
			free_string_array(array_platos);
			free_string_array(array_precio_platos);
			dictionary_destroy_and_destroy_elements(restaurante_file, free);
			free(stream);
			// Carga de pedidos sin terminar
			char *resto_path = string_from_format("%s/%s", afip_context.restaurantes_dir, files->d_name);
			DIR *resto_dir = opendir(resto_path);
			while ((file_pedido = readdir(resto_dir)) != NULL) {
				if (file_pedido->d_type == DT_REG && string_starts_with(file_pedido->d_name, "pedido")) {
					char *id_pedido = pedido_get_id_from_filename(file_pedido->d_name);
					t_obtener_pedido_res *pedido_res = afip_recuperar_pedido(files->d_name, id_pedido);
					if (pedido_res != NULL) {
						char *clave = string_from_format("%s_pedido%s", files->d_name, id_pedido);
						t_pedido_item *value = pedido_value_create();
						switch (pedido_res->estado) {
						case PENDIENTE:
							value->estado = Pendiente;
							break;
						case CONFIRMADO:
							value->estado = Confirmado;
							break;
						}
						dictionary_put(pedidos_dict->dict, clave, value);
						obtener_pedido_res_destroy(pedido_res);
						free(clave);
					}
					free(id_pedido);
				}
			}
			closedir(resto_dir);
		}
	}

	closedir(dir);
	log_debug(logger, "Afip caches creados OK");
}

void afip_block_save() {
	rewind(afip_blocks.block_file);
	size_t blocks_writes = fwrite(afip_blocks.blocks->bitarray, sizeof(char), afip_context.block_count,
			afip_blocks.block_file);
	if (blocks_writes < 0) {
		log_error(logger, "Error al escribir el bitmap file");
		exit_failure();
	}
	fflush(afip_blocks.block_file);
}

void afip_leer_bloque(char **result, int *bloqueId, int size) {
	FILE *fptr = NULL;
	char *buffer = NULL;
	char *block_file = string_from_format("%s/%d%s", afip_context.blocks_dir, *bloqueId, AFIP_EXTENSION);

	if ((fptr = fopen(block_file, "rb")) != NULL) {
		int read_size = (size > afip_context.block_size) ? afip_context.block_size - 4 : size;
		buffer = calloc(read_size + 1, sizeof(char));
		fread(buffer, read_size, 1, fptr);
		string_append(result, buffer);
		free(buffer);
		if (read_size != size) {
			fread(bloqueId, sizeof(uint32_t), 1, fptr);
		} else {
			*bloqueId = -1;
		}
		fclose(fptr);
	} else {
		log_error(logger, "Ocurrio un error leyendo el archivo de bloque %s", block_file);
		*bloqueId = -1;
	}
}

uint32_t afip_calcular_cantidad_bloques(uint32_t stream_length) {
	uint32_t count = 0;
	do {
		count++;
		if (stream_length <= afip_context.block_size) {
			// Es el ultimo bloque
			stream_length = 0;
		} else if (stream_length > afip_context.block_size) {
			// Le descuento el tamaño de bloque menos los 4 bytes
			stream_length -= (afip_context.block_size - 4);
		}
	} while (stream_length > 0);

	return count;
}

_Bool is_file_accesible(char *path) {
	return (access(path, R_OK | W_OK) == 0);
}

_Bool create_empty_file(char *path) {
	_Bool result = false;
	int file_fd = open(path, O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	result = (file_fd != -1);
	close(file_fd);
	return result;
}

int circle_inc(int value, int max) {
	return (++value == max) ? 0 : value;
}

t_list* afip_block_array_insert(int block_count) {
	t_list *resultado = list_create();

	size_t max = bitarray_get_max_bit(afip_blocks.blocks);
	if (block_count > max) {
		log_error(logger, "La cantidad solicitada es mayor a la total");
		return NULL;
	}

	log_debug(logger, "Se solicitan %d bloques", block_count);
	int reserved = 0;
	pthread_mutex_lock(&(afip_blocks.mutex));

	do {
		if (!bitarray_test_bit(afip_blocks.blocks, afip_blocks.first_free)) {
			log_info(logger, "Asigno el bloque %d", afip_blocks.first_free);
			bitarray_set_bit(afip_blocks.blocks, afip_blocks.first_free);
			list_add(resultado, int_clone(afip_blocks.first_free));
			reserved++;
		}
		afip_blocks.first_free = circle_inc(afip_blocks.first_free, afip_blocks.blocks->size);
	} while (reserved < block_count);

	while (bitarray_test_bit(afip_blocks.blocks, afip_blocks.first_free)) {
		afip_blocks.first_free = circle_inc(afip_blocks.first_free, afip_blocks.blocks->size);
	}
	// Guardo los datos en el archivo
	afip_block_save();
	pthread_mutex_unlock(&afip_blocks.mutex);

	for (int i = 0; i < resultado->elements_count; i++) {
		uint32_t *value = list_get(resultado, i);
		log_info(logger, "Numero de bloque reservado %d", *value);
	}

	return resultado;
}

void afip_block_array_update(t_list *bloques_actual, uint32_t block_count) {
	size_t max = bitarray_get_max_bit(afip_blocks.blocks);
	if (block_count > max) {
		log_error(logger, "La cantidad solicitada es mayor a la total");
		return;
	}

	if (bloques_actual->elements_count == block_count) {
		log_debug(logger, "No se modifica la cantidad de bloques, retornando");
		return;
	}

	log_debug(logger, "Se va a cambiar de %d a %d bloques", bloques_actual->elements_count, block_count);
	pthread_mutex_lock(&afip_blocks.mutex);
	if (bloques_actual->elements_count > block_count) {
		// Se achico el tamaño de bloques utilizados
		for (int i = block_count; i >= bloques_actual->elements_count; i--) {
			int *bloque = list_get(bloques_actual, i);
			log_info(logger, "Libero el bloque %d", *bloque);
			bitarray_clean_bit(afip_blocks.blocks, *bloque);
			list_remove(bloques_actual, i);
		}
	} else {
		// Se incremento el tamaño de bloques utilizados
		int reserved = bloques_actual->elements_count;
		do {
			if (!bitarray_test_bit(afip_blocks.blocks, afip_blocks.first_free)) {
				log_info(logger, "Asigno el bloque %d", afip_blocks.first_free);
				bitarray_set_bit(afip_blocks.blocks, afip_blocks.first_free);
				uint32_t *nuevo = int_clone(afip_blocks.first_free);
				list_add(bloques_actual, nuevo);
				reserved++;
			}
			afip_blocks.first_free = circle_inc(afip_blocks.first_free, afip_blocks.blocks->size);
		} while (reserved < block_count);

		while (bitarray_test_bit(afip_blocks.blocks, afip_blocks.first_free)) {
			afip_blocks.first_free = circle_inc(afip_blocks.first_free, afip_blocks.blocks->size);
		}
	}
	// Guardo los datos en el archivo
	afip_block_save();
	pthread_mutex_unlock(&afip_blocks.mutex);
}

_Bool afip_guardar_bloques(t_list *list_bloques, char *stream) {
	FILE *file = NULL;
	size_t write = 0;

	for (int i = 0; i < list_bloques->elements_count; i++) {
		uint32_t *bloque = list_get(list_bloques, i);
		char *filename = string_from_format("%s/%d%s", afip_context.blocks_dir, *bloque, AFIP_EXTENSION);
		if ((file = fopen(filename, "wb")) != NULL) {
			char *buffer = NULL;
			if (i == (list_bloques->elements_count - 1)) {
				// Si es el ultimo bloque
				if (i == 0) {
					buffer = string_substring_from(stream, 0);
				} else {
					buffer = string_substring_from(stream, (afip_context.block_size * i) - (4 * i));
				}
				write = fwrite(buffer, sizeof(char), strlen(buffer), file);
				log_debug(logger, "Archivo: escribi %d bytes", write);
				fflush(file);
				fclose(file);
				free(buffer);
			} else {
				// Si NO es el ultimo bloque
				if (i == 0) {
					buffer = string_substring(stream, 0, afip_context.block_size - 4);
				} else {
					buffer = string_substring(stream, (afip_context.block_size * i) - (4 * i),
							afip_context.block_size - 4);
				}
				uint32_t *next = list_get(list_bloques, i + 1);
				write = fwrite(buffer, sizeof(char), afip_context.block_size - 4, file);
				write += fwrite(next, sizeof(uint32_t), 1, file);
				log_debug(logger, "Archivo: escribi %d", (write - 1) + sizeof(uint32_t));
				fflush(file);
				fclose(file);
				free(buffer);
			}
		} else {
			log_debug(logger, "Error abriendo el archivo %s ", filename);
			exit_failure();
		}

	}
	return true;
}

t_sync_dictionary* create_sync_dictionary() {
	t_sync_dictionary *result = NULL;
	result = calloc(1, sizeof(t_sync_dictionary));
	pthread_mutex_init(&result->mutex, NULL);
	result->dict = dictionary_create();

	return result;
}

void sync_dictionary_lock(t_sync_dictionary *dict) {
	pthread_mutex_lock(&dict->mutex);
}

void sync_dictionary_unlock(t_sync_dictionary *dict) {
	pthread_mutex_unlock(&dict->mutex);
}

void afip_guardar_inicio(bool create, char *filename, int bloque_inicial, int tamanio_total) {
	if (create) {
		create_empty_file(filename);
	}
	t_config *archivo_inicial = config_create(filename);
	if (archivo_inicial == NULL) {
		log_error(logger, "No se puede abrir el archivo %s", filename);
		exit_failure();
	}

	config_set_int_value(archivo_inicial, KEY_SIZE, tamanio_total);
	config_set_int_value(archivo_inicial, KEY_INITIAL_BLOCK, bloque_inicial);
	config_save(archivo_inicial);
	config_destroy(archivo_inicial);
	log_debug(logger, "Se guardo el archivo %s correctamente", filename);
}

void afip_get_inicio(char *filename, int *bloque_inicial, int *tamanio_total) {
	t_config *archivo_inicial = config_create(filename);
	if (archivo_inicial == NULL) {
		log_error(logger, "No se puede abrir el archivo %s", filename);
		exit_failure();
	}
	*tamanio_total = config_get_int_value(archivo_inicial, KEY_SIZE);
	*bloque_inicial = config_get_int_value(archivo_inicial, KEY_INITIAL_BLOCK);
	config_destroy(archivo_inicial);
	log_debug(logger, "Datos del archivo %s - size: %d bloque inicial: %d", filename, *tamanio_total, *bloque_inicial);
}

char* afip_leer_archivo(char *afip_path) {
	int size, bloqueId = 0;
	char *result = string_new();

	//obtengo los datos del archivo inicial
	afip_get_inicio(afip_path, &bloqueId, &size);

	//leo el primer bloque
	while (bloqueId != -1) {
		int bloque_actual = bloqueId;
		log_debug(logger, "Leyendo %d", bloque_actual);
		afip_leer_bloque(&result, &bloqueId, size);
		log_debug(logger, "Bloque leido %d: [%s]", bloque_actual, result);
		if (bloqueId != -1) {
			size -= (afip_context.block_size - 4);
		}
		//si hay mas bloques continuo leyendo el siguiente bloque
	}

	return result;
}

t_archivo_bloques* afip_leer_archivo_con_bloques(char *afip_path) {
	int size, bloqueId = 0;
	t_list *bloques = list_create();
	char *stream = string_new();
	t_archivo_bloques *resultado = NULL;

	//obtengo los datos del archivo inicial
	afip_get_inicio(afip_path, &bloqueId, &size);

	while (bloqueId != -1) {
		uint32_t *bloque = NULL;
		bloque = int_clone(bloqueId);
		list_add(bloques, bloque);
		afip_leer_bloque(&stream, &bloqueId, size);
		log_debug(logger, "Bloque leido %d: [%s]", *bloque, stream);
		if (bloqueId != -1) {
			size -= (afip_context.block_size - 4);
		}
		//si hay mas bloques continuo leyendo el siguiente bloque
	}

	resultado = calloc(sizeof(t_archivo_bloques), 1);
	resultado->stream = stream;
	resultado->bloques = bloques;

	return resultado;
}

t_pedido_item* pedido_value_create() {
	t_pedido_item *pedido = calloc(sizeof(t_pedido_item), 1);
	pedido->rwlock = malloc(sizeof(pthread_rwlock_t));
	pthread_rwlock_init(pedido->rwlock, NULL);
	pedido->estado = Pendiente;

	return pedido;
}

void pedido_value_destroy(t_pedido_item *pedido) {
	if (pedido->rwlock != NULL) {
		pthread_rwlock_destroy(pedido->rwlock);
	}
	if (pedido != NULL) {
		free(pedido);
	}
}

t_dictionary* stream_to_dictionary(char *data) {
	t_dictionary *dictionary = dictionary_create();
	char **lines = string_split(data, "\n");

	void add_cofiguration(char *line) {
		if (!string_starts_with(line, "#")) {
			char **keyAndValue = string_n_split(line, 2, "=");
			dictionary_put(dictionary, keyAndValue[0], keyAndValue[1]);
			free(keyAndValue[0]);
			free(keyAndValue);
		}
	}
	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);
	free(lines);

	return dictionary;
}

