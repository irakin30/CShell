#ifndef SHELL_H
#define SHELL_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>

char **parse_args(char *line);
void run(char **args);
void child_run(char **args); //working
int special_cases(char **arg);
char *strip_spaces(char *str);
void redirect_out(char *fileName, char **args);
void redirect_out_append(char *fileName, char **args);
void redirect_in(char *fileName, char **args);
void redirect_inout(char *fileIn, char **args, char *fileOut);
void redirect_pipe(char **src_args, char **dest_args);
#endif
