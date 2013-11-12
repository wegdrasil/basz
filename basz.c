//----------------------------------------------------------------------
// Operating Systems Lab 
// --project: Simple command interpreter
// --autor:   Rafał Romanowski
// --Bialystok University of Technology 2013
//----------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int CountChar(char *s, char c)
{
	int i;
	for (i=0; s[i]; s[i]== c ? i++ : s++);
	return i;
}

void ParseCmd(char** line, char** programName, char*** args, int* spacesInLine)	
{

	*spacesInLine = CountChar(*line, ' ');

	*programName = strtok(*line, " \n");
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
}

int main(int argc, char const *argv[])
{
	//to the infinity and beyond!!!
	for(;;)
	{
		int ret;
        while((waitpid(-1, &ret, WNOHANG)) > 0)
        	printf("dealing with zombie\n");

		char *programName;
		char** arguments;
		int spacesInLine;

		printf("basz > ");

		char *cmdLine = NULL;
		size_t len = 0;

		ssize_t read = getline(&cmdLine, &len, stdin);
		
		cmdLine = strtok(cmdLine, "\n");

		if(read == -1)
		{
			printf("Bye basz\n");
			return;
		}

		int b_background = 0;
		char* truncCmdLine = NULL;

		char* pch;
		int b_stdoutNew = 0;
		char* newcmdline = NULL;

		int fileDesc[2];

		if((pch = strstr(cmdLine, " >> ")) != NULL)
		{
			int lengthCmd = strlen(cmdLine) - strlen(pch);
			
			newcmdline = (char *)malloc(sizeof(char) * (lengthCmd));
			strncpy(newcmdline, cmdLine, lengthCmd);

			printf("new command: %s\n", newcmdline);

			b_stdoutNew = 1;
			pch = strtok(pch, " >");
			printf("file: %s\n", pch);

			pipe(fileDesc); // errors?
			
			if(newcmdline[strlen(newcmdline)-1] == '&')
			{
				b_background = 1;

				truncCmdLine = (char *)malloc(sizeof(char) * (strlen(newcmdline)-2));
				
				int i;
				for(i = 0; i < strlen(newcmdline)-2; i++ )
				{	
					truncCmdLine[i] = cmdLine[i];
					//printf("%c\n", truncCmdLine[i]);
				}
				
			}	
		}
		else
		{
			if(cmdLine[strlen(cmdLine)-1] == '&')
			{
				b_background = 1;

				truncCmdLine = (char *)malloc(sizeof(char) * (strlen(cmdLine)-2));
				
				int i;
				for(i = 0; i < strlen(cmdLine)-2; i++ )
				{	
					truncCmdLine[i] = cmdLine[i];
					//printf("%c\n", truncCmdLine[i]);
				}
				
			}	
		}
				
		//-----------------------------------------------------------------
		pid_t pid = fork();
		if(pid == 0)
		{
			if(b_background)
				ParseCmd(&truncCmdLine, &programName, &arguments, &spacesInLine);
			else
				ParseCmd(&cmdLine, &programName, &arguments, &spacesInLine);

			if(b_stdoutNew)
			{
				fileDesc[1]=open(pch,O_WRONLY|O_CREAT|O_TRUNC, 0666);
                dup2(fileDesc[1],1);

                close(fileDesc[1]);
                execvp(arguments[0], arguments);
			}
			else
			{
				execvp(arguments[0], arguments);
				printf("\nChild fail \n");
			}
		}
		else if (pid > 0)
		{	
			int status;
			if(b_background)
			{
				//dont wait in backgroud!
			}
			else
			{
				waitpid(pid, &status, 0);
			}
			printf("End \n");
		}

		//-----------------------------------------------------------------
		// free(truncCmdLine);
		// int i;
		// for(i = 0; i < spacesInLine; i++)
		// {
		// 	free(arguments[i]);
		// }
		// free(arguments);
		//printf("Free \n");	
	
	}
	return 0;
}
