#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void ParsePipeline(char** astr, char*** parsedCmds, int pipeSize);

void MiddlePipes(char** cmds, int** fd, int n);

void Pipeline(char** cmds, int** fd, unsigned int n);
