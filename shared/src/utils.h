#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/process.h>
#include <commons/collections/list.h>

typedef struct {
	uint32_t size;
	uint32_t *data;
} t_intarray;

t_intarray* create_intarray(int size);
t_intarray* string_split_int(char *string, char *separator);
void free_intarray(t_intarray *intarray);

extern t_log *logger;

int split_size(char **split);

int string_count_char(char *str, char ch);

void free_string_array(char **array);

_Bool is_comma_separated_string(char *str);

_Bool is_comma_separated_number(char *str);

_Bool string_is_alpha(char *str);

_Bool string_is_number(char *str);

void config_set_int_value(t_config *config, char *key, int value);

void exit_failure();

char* string_ncopy_from(int size, char *src);

int dictionary_get_int_value(t_dictionary *dictionary, char *key);

char** dictionary_get_string_array_value(t_dictionary *dictionary, char* key);

void dictionary_get_position_value(t_dictionary* dictionary, char* key, uint32_t* x, uint32_t* y);

int indexof(char *str, char c);

t_list* list_from_string_array(char* arr_str);

float distancia(float x1, float y1, float x2, float y2);

uint32_t* int_clone(int value);

#endif /* UTILS_H_ */
