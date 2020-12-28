#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "sindicato.h"

#define DIR_METADATA "Metadata"
#define FILE_METADATA "Metadata/Metadata.AFIP"
#define BITMAP_BIN "Bitmap.bin"
#define DIR_BLOCKS "Blocks"
#define DIR_RECETAS "Files/Recetas"
#define DIR_RESTAURANTE "Files/Restaurantes"

#define AFIP_EXTENSION ".AFIP"

#define METADATA_BLOCK_SIZE_KEY "BLOCK_SIZE"
#define METADATA_BLOCKS_KEY "BLOCKS"
#define METADATA_MAGIC_NUMBER_KEY "MAGIC_NUMBER"

#define KEY_SIZE "SIZE"
#define KEY_INITIAL_BLOCK "INITIAL_BLOCK"


typedef struct {
	//
	char *mount_path;
	// Metadata
	char *metadata_path;
	// Bitmap
	char *bitmap_path;
	// Files/Recetas
	char *blocks_dir;
	// Files/Recetas
	char *recetas_dir;
	// Files/Restaurantes
	char *restaurantes_dir;
	uint32_t block_size;
	uint32_t block_count;
} t_afip_context;

typedef struct {
	pthread_mutex_t mutex;
	off_t first_free;
	FILE *block_file;
	t_bitarray *blocks;
} t_afip_block_array;

typedef struct {
	pthread_mutex_t mutex;
	t_dictionary *dict;
} t_sync_dictionary;

typedef struct {
	estado_pedido estado;
	pthread_rwlock_t *rwlock;
} t_pedido_item;

typedef struct {
	char *stream;
	t_list *bloques;
} t_archivo_bloques;


typedef struct file_pointer t_afip_file_pointer;

t_afip_context afip_context;
t_afip_block_array afip_blocks;

t_sync_dictionary *recetas_dict;

t_sync_dictionary *restaurantes_dict;

t_sync_dictionary *pedidos_dict;

void afip_check_directories_permissions(char *mount_point);
void afip_initialize(char *basedir);
void afip_create_caches();
uint32_t afip_calcular_cantidad_bloques(uint32_t stream_length);
_Bool afip_guardar_bloques(t_list *list_bloques, char *stream);
_Bool is_file_accesible(char *path);
t_list* afip_block_array_insert(int block_count);
void afip_block_array_update(t_list *bloques_actual, uint32_t block_count);
t_sync_dictionary *create_sync_dictionary();
void sync_dictionary_lock(t_sync_dictionary *);
void sync_dictionary_unlock(t_sync_dictionary *);
_Bool create_empty_file(char *path);
void afip_leer_bloque(char** result, int *bloqueId, int size);
void afip_guardar_inicio(bool create, char *filename, int bloque_inicial, int tamanio_total);
void afip_get_inicio(char *filename, int *bloque_inicial, int *tamanio_total);
char* afip_leer_archivo(char *afip_path);
t_archivo_bloques* afip_leer_archivo_con_bloques(char *afip_path);
t_pedido_item* pedido_value_create();
void pedido_value_destroy(t_pedido_item* pedido);
t_dictionary* stream_to_dictionary(char *data);
char* pedido_get_id_from_filename(char *filename);
estado_pedido pedido_get_estado_from_string(char *estado_pedido_str);
e_estado_pedido pedido_get_e_estado_pedido_from_string(char *estado_pedido_str);

#endif
