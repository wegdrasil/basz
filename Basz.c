#include "Basz.h"
#include "CommandHistory.h"

int CountChar(char *s, char c)
{
	int i;
	for (i=0; s[i]; s[i]== c ? i++ : s++);
	return i;
}

void ParseCmd(char* line, char*** args)	
{
	int spacesInLine = CountChar(line, ' ');
	int size = spacesInLine ? (spacesInLine + 2) : (spacesInLine + 1);
	char* pch = strtok(line, " \n");
	*args = (char **)malloc(sizeof(char *) * (size));
	
	(*args)[0] = (char *)malloc(sizeof(char) * strlen(pch));
	strcpy((*args)[0], pch);
	
	int i;
	for(i = 1; i < size -1; i++)
	{
		char* tmp = strtok(NULL, " \n");
	 	(*args)[i] = (char *)malloc(sizeof(char) * strlen(tmp));
	 	strcpy((*args)[i], tmp);	
	} 

	(*args)[size] = NULL;
}