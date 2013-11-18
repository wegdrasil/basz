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
//#include <sys/wait.h>
#include <fcntl.h>

#include "Basz.h"
#include "CommandHistory.h"
#include "Pipeline.h"

#define FIFO_NAME "readhistoryFIFO"

CommandHistory cmdHistory;
void sigusr1_handler(int sig)
{
	PrintHistory(&cmdHistory);
}

int main(int argc, char const *argv[])
{ 
	cmdHistory.fileName = "history";
	cmdHistory.capacity = 5;
	cmdHistory.size = 0;
	cmdHistory.head = NULL;
	cmdHistory.tail = NULL;
	//----------------------------------------
	struct sigaction sa;
	sa.sa_handler = sigusr1_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }
	//----------------------------------------
	
	ReadHistory(&cmdHistory);
	//PrintHistory(&cmdHistory);
	//kill(getpid(), SIGUSR1);
	

	for(;;)
	{
		int ret;
        while((waitpid(-1, &ret, WNOHANG)) > 0)
        	printf("dealing with zombie\n");

		char *programName;
		char** arguments;
		int spacesInLine;
		char *cmdLine = NULL;
		size_t len = 0;

		
		printf("basz$ ");

		ssize_t read = getline(&cmdLine, &len, stdin);
		
		if((read == -1))
		{
			//SendHistoryToFIFO(&cmdHistory); //compile and open ReadHistroyFifo.c
			printf("History saving \n");
			WriteHistory(&cmdHistory);
			DestroyHistoryList(&cmdHistory);
			printf("History saved \n");
			printf("Bye basz\n");

			return;
		}
			

		AddCmdToHistory(&cmdHistory, &cmdLine);
	
		//PrintHistory(&cmdHistory);
		kill(getpid(), SIGUSR1);

		cmdLine = strtok(cmdLine, "\n");

		int pipelineSize = 0;
		char** parsedCommands;
		int b_pipeline = 0;
		int** arrayOfFileDesc;
		if((pipelineSize = CountChar(cmdLine, '|')) > 0)
		{
			printf("%d\n", pipelineSize);
			b_pipeline = 1;
			printf("pipeline\n");

			arrayOfFileDesc = (int**)malloc(sizeof(int*) * pipelineSize);
			int h;
			for(h = 0; h < pipelineSize; h++)
				arrayOfFileDesc[h] = (int*)malloc(sizeof(int) * 2);

			pipelineSize++;

			ParsePipeline(&cmdLine, &parsedCommands, pipelineSize);
			int k;
			for(k = 0; k < pipelineSize; k++)
				printf ("%s\n", parsedCommands[k]);
			
		}
		

		int b_background = 0;
		char* truncCmdLine = NULL;

		char* pch;
		int b_stdoutNew = 0;

		char* newcmdline = NULL;
		char* fileDiversion = NULL;
		int fileDesc[2];
		
		if((pch = strstr(cmdLine, " >> ")) != NULL)
		{
			printf("cmdLine: %s\n", cmdLine);
		    printf("pch: %s\n", pch);

		    int lengthCmd = strlen(cmdLine) - strlen(pch);

		    printf("l cmdLine: %d\n", strlen(cmdLine));
		    printf("l pch: %d\n", strlen(pch));
		    

		    newcmdline = (char *)malloc(sizeof(char) * lengthCmd);
		    //strncpy(newcmdline, cmdLine, lengthCmd);
		    snprintf(newcmdline, lengthCmd+1, cmdLine);
		    printf("new command: %s\n", newcmdline);

			b_stdoutNew = 1;
			pch = strtok(pch, " >");
			printf("file: %s\n", pch);
			fileDiversion = pch;
			printf("file: %s\n", fileDiversion);
			pipe(fileDesc); 
			
			if(newcmdline[strlen(newcmdline)-1] == '&')
			{
				b_background = 1;

				truncCmdLine = (char *)malloc(sizeof(char) * (strlen(newcmdline)-2));
				
				int i;
				for(i = 0; i < strlen(newcmdline)-2; i++ )
				{	
					truncCmdLine[i] = cmdLine[i];
					printf("%c\n", truncCmdLine[i]);
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
				}
				
			}	
		}
		//-----------------------------------------------------------------
		pid_t pid = fork();
		if(pid < 0){perror("fork fail"); exit(-1);}
		if(pid == 0)
		{
			if(b_background)
			{
				ParseCmd(truncCmdLine, &arguments);
			}
			
			if(b_pipeline)
			{
				Pipeline(parsedCommands, arrayOfFileDesc,  pipelineSize-1);
			}
				
			if(b_stdoutNew)
			{
             
                ParseCmd(newcmdline, &arguments);
            
				fileDesc[1]=open(fileDiversion,O_WRONLY|O_CREAT|O_TRUNC, 0666);
                dup2(fileDesc[1],1);
                close(fileDesc[1]);
                
            	execvp(arguments[0], arguments);
			}
			ParseCmd(cmdLine, &arguments);

			execvp(arguments[0], arguments);
			printf("\nChild fail \n");
		}
		else if (pid > 0)
		{	
			int status;
			if(b_background)
			{
				//dont wait in background!
			}
			else
			{
				waitpid(pid, &status, 0);
			}
		}

		//-----------------------------------------------------------------
	
	}
	return 0;
}