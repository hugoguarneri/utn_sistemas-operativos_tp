#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>


typedef struct {
	char* cmd;
    t_list* args;
} t_command;


t_command* command_create(char* line);

void command_destroy(t_command* command);

#endif /* COMMAND_H_ */

