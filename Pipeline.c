#include "Pipeline.h"

void ParsePipeline(char** astr, char*** parsedCmds, int pipeSize)
{
	//amount of commands in the pipeline
	
 	char** cmds = (char **)malloc(sizeof(char *) * (pipeSize));
 	char* pch = NULL;
  	pch = strtok(*astr, "|");

 	cmds[0] = (char *)malloc(sizeof(char) * strlen(pch));
 	strcpy(cmds[0], pch);  	

	int i = 1;	
	while (pch != NULL && i < pipeSize)
	{
		pch = strtok (NULL, "|");
		cmds[i] = (char *)malloc(sizeof(char) * strlen(pch));
		strcpy(cmds[i], pch);  	
		i++;
	}
	
	char** tmpCmd = NULL;
	tmpCmd = (char**)malloc(sizeof(char*)*(pipeSize));

    //im trying to make this _____ps_____-l_____ =>  ps -l

	for(i = 0; i < pipeSize; i++)
	{
		tmpCmd[i] = (char *)malloc(sizeof(char) * (strlen(cmds[i]) - CountChar(cmds[i], ' ')));
		pch = NULL;
		pch = strtok(cmds[i], " ");
		strcpy(tmpCmd[i], pch);
		
		while(pch != NULL)
		{
		 	pch = strtok(NULL, " ");
		 	if(pch == NULL)
		 		break;
		 	strcat(tmpCmd[i], " ");
		 	strcat(tmpCmd[i], pch);
	    }
		
	}	

// 	//	free(tmp);
    *parsedCmds = tmpCmd;
}

void MiddlePipes(char** cmds, int** fd, int n)
{
	char** localCmds = cmds;

	if(n == 0)
	{
		pipe(fd[n]);
		int pid = fork();
		if(pid < 0){perror("fork fail"); exit(-1);}
		else if(pid == 0)
		{
			//(1)
			dup2(fd[n][1], 1);	close(fd[n][0]); close(fd[n][1]);
					
			char** arguments;
			ParseCmd(localCmds[n], &arguments);
			execvp(arguments[0], arguments);
		}
		else
		{
			dup2(fd[n][0], 0);	close(fd[n][0]);	close(fd[n][1]);

			dup2(fd[n+1][1], 1); close(fd[n+1][0]);	close(fd[n+1][1]);
			
			int status;
			waitpid(pid, &status, 0);
			
			char** arguments;
			ParseCmd(localCmds[n+1], &arguments);
			execvp(arguments[0], arguments);
		}
	}	
	//(2)
	else
	{
		n--;
		pipe(fd[n]);
		int pid = fork();
		if(pid < 0){perror("fork fail"); exit(-1);}
		else if(pid == 0)
		{
			MiddlePipes(localCmds,fd, n);
		}
		else
		{
			// out/ in
			dup2(fd[n][0], 0);	close(fd[n][0]); close(fd[n][1]);

			dup2(fd[n+1][1], 1); close(fd[n+1][0]);	close(fd[n+1][1]);
			
			int status;
			waitpid(pid, &status, 0);

			char** arguments;
			ParseCmd(localCmds[n+1], &arguments);
			execvp(arguments[0], arguments); 
		}
	}	

}

void Pipeline(char** cmds, int** fd, unsigned int n)
{
	char** localCmds = cmds;
	if(n == 1)
	{
		n--;

		pipe(fd[n]);
		int pid = fork();
		if(pid < 0){perror("fork fail"); exit(-1);}
		else if(pid == 0)
		{
			dup2(fd[n][1], 1);	close(fd[n][0]); close(fd[n][1]);
		
			char** arguments;
			ParseCmd(localCmds[n], &arguments);
			execvp(arguments[0], arguments);
		}
		else  //last 
		{
			//(3)
	    	dup2(fd[n][0], 0); 	close(fd[n][0]);  	close(fd[n][1]);

	    	int status;
			waitpid(pid, &status, 0);

			char** arguments;
			ParseCmd(localCmds[n+1], &arguments);
			execvp(arguments[0], arguments);
		}
	}
	else
	{
		n--;
		pipe(fd[n]);
		int pid = fork();
		if(pid < 0){perror("fork fail"); exit(-1);}
		else if(pid == 0)
		{
			MiddlePipes(localCmds, fd, n);
		}
		else  //last 
		{
			//(3)
	    	dup2(fd[n][0], 0); 	close(fd[n][0]);   	close(fd[n][1]);

	    	int status;
			waitpid(pid, &status, 0);

			char** arguments;
			ParseCmd(localCmds[n+1], &arguments);
			execvp(arguments[0], arguments);
		}
	}
}