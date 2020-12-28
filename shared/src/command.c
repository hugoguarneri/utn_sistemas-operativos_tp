#include "command.h"

t_command* command_create(char* line){
	char* line_aux = strdup(line);
	t_command* command = malloc(sizeof(t_command));

	string_append(&line_aux, " ");
	char** array =  string_split(line_aux, " ");

	command->cmd = malloc(strlen(array[0]));
	command->cmd = array[0];

	t_list* args = list_create();
	for (array++; *array != NULL; array++){
		list_add(args, *array);
	}
	command->args = args;
	free(line_aux);
	return command;
}

void command_destroy(t_command* command){
	list_destroy_and_destroy_elements(command->args, (void*) free);
	free(command->cmd);
	free(command);

}


