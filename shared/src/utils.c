#include "utils.h"

int split_size(char **split) {
	int i = 0;
	while (split[i] != NULL) {
		i++;
	}
	return i;
}

int string_count_char(char *str, char ch) {
	int count = 0;
	for (int i = 0; str[i] != '\0'; ++i) {
		if (ch == str[i])
			++count;
	}
	return count;
}

void free_string_array(char **array) {
	int i = 0;
	while (array[i] != NULL) {
		free(array[i++]);
	}
	free(array);
}

_Bool is_comma_separated_number(char *str) {

	if (!string_starts_with(str, "["))
		return false;

	if (!string_ends_with(str, "]"))
		return false;

	for (int i = 1; i < strlen(str) - 1; i++) {
		if (!isdigit(str[i]) && str[i] != ',' && str[i] != ' ')
			return false;
	}
	return true;
}

_Bool is_comma_separated_string(char *str) {

	if (!string_starts_with(str, "["))
		return false;

	if (!string_ends_with(str, "]"))
		return false;

	for (int i = 1; i < strlen(str) - 1; i++) {
		if (!isalpha(str[i]) && str[i] != ',' && str[i] != ' ')
			return false;
	}
	return true;
}

_Bool string_is_alpha(char *str) {
	for (int i = 0; i < strlen(str); i++) {
		if (!isalpha(str[i]))
			return false;
	}
	return true;
}

_Bool string_is_number(char *str) {
	for (int i = 0; i < strlen(str); i++) {
		if (!isdigit(str[i]))
			return false;
	}
	return true;
}

t_intarray* create_intarray(int size) {
	t_intarray *result = NULL;
	result = calloc(sizeof(t_intarray), 1);
	result->data = calloc(sizeof(int), size);
	result->size = size;
	return result;
}

t_intarray* string_split_int(char *string, char *separator) {
	t_intarray *result = NULL;
	char **data = string_split(string, separator);
	int size = split_size(data);
	result = calloc(sizeof(t_intarray), 1);
	result->data = calloc(sizeof(uint32_t), size);
	result->size = size;
	for (int i = 0; i < size; i++) {
		result->data[i] = atoi(data[i]);
	}
	free_string_array(data);

	return result;
}

void free_intarray(t_intarray *intarray) {
	if (intarray->data != NULL)
		free(intarray->data);
	free(intarray);
}

void exit_failure() {
	perror("Error: ");
	exit(EXIT_FAILURE);
}

void config_set_int_value(t_config *config, char *key, int value) {
	char *str = string_itoa(value);
	config_set_value(config, key, str);
	free(str);
}

char* string_ncopy_from(int size, char *src) {
	char *target = NULL;
	target = calloc(size + 1, sizeof(char));
	strncpy(target, src, size);
	return target;
}

int dictionary_get_int_value(t_dictionary *dictionary, char *key) {
	char *value = dictionary_get(dictionary, key);
	return atoi(value);
}

char** dictionary_get_string_array_value(t_dictionary *dictionary, char *key) {
	char *value_in_dictionary = dictionary_get(dictionary, key);
	return string_get_string_as_array(value_in_dictionary);
}

void dictionary_get_position_value(t_dictionary *dictionary, char *key, uint32_t *x, uint32_t *y) {
	char *value_in_dictionary = dictionary_get(dictionary, key);
	char **string_array = string_get_string_as_array(value_in_dictionary);
	*x = atoi(string_array[0]);
	*y = atoi(string_array[1]);
	free_string_array(string_array);
}

int indexof(char *str, char c) {
	int i;
	for (i = 0; i < strlen(str); i++) {
		if (str[i] == c)
			return i;
	}

	return -1;
}

t_list* list_from_string_array(char *arr_str) {
	t_list *list = list_create();

	char *str_csv = string_substring(arr_str, 1, strlen(arr_str) - 2);
	char **arr = string_split(str_csv, ",");

	for (char **el = arr; *el; el++) {
		list_add(list, *el);
	}
	return list;
}

float distancia(float x1, float y1, float x2, float y2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

uint32_t* int_clone(int value) {
	uint32_t *result = calloc(sizeof(uint32_t), 1);
	*result = value;
	return result;
}

