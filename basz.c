//----------------------------------------------------------------------
// Operating Systems Lab 
// --project: Simple command interpreter
// --autor:   Rafał Romanowski
// --Bialystok University of Technology 2013
//----------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int CountChar(char *s, char c)
{
	int i;
	for (i=0; s[i]; s[i]== c ? i++ : s++);
	return i;
}

int parseCommand(char** programName, char*** args, int* spacesInLine)	
{
	char *line = NULL;
	size_t len = 0;

	ssize_t	read = getline(&line, &len, stdin);

	*spacesInLine = CountChar(line, ' ');

	*programName = strtok(line, " \n");
	*args = (char **)malloc(sizeof(char *) * (*spacesInLine + 1));
	
	(*args)[0] = (char *)malloc(sizeof(char) * strlen(*programName));
	strcpy((*args)[0], *programName);

	
	int i;
	for(i = 1; i < *spacesInLine + 1; i++)
	{
		char* tmp = strtok(NULL, " \n");
	 	(*args)[i] = (char *)malloc(sizeof(char) * strlen(tmp));
	 	strcpy((*args)[i], tmp);	
	} 

	(*args)[2] = NULL;

	return read;
}

int main(int argc, char const *argv[])
{
	for(;;)
	{
		char *programName;
		char** arguments;
		int spacesInLine;

		printf("basz > ");

		if(parseCommand(&programName, &arguments, &spacesInLine) == -1)
		{
			int i;
			for(i = 0; i < spacesInLine; i++)
			{
				free(arguments[i]);
			}
			free(arguments);
			return 0;
		}
		//-----------------------------------------------------------------
		
		if(!fork())
		{
			execvp(programName, arguments);
			printf("\nChild fail \n");
		}
		else
		{
			wait();
			printf("\nEnd \n");
		}

		//-----------------------------------------------------------------

		int i;
		for(i = 0; i < spacesInLine; i++)
		{
			free(arguments[i]);
		}
		free(arguments);
		printf("\nFree \n");	
	}
	return 0;
}