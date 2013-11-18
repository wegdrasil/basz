#include "CommandHistory.h"
#define FIFO_NAME "readhistoryFIFO"
void AddCmdToHistory(CommandHistory* cmdHist, char** cmdLine)
{
	if(cmdHist->size < cmdHist->capacity)
 	{
 		CmdNode* tmp = NULL;
 		tmp = (CmdNode*)malloc(sizeof(CmdNode));
 		
 		if(cmdHist->head == NULL)
 			cmdHist->tail = tmp; 

 		tmp->next = cmdHist->head;
 		tmp->cmdLine = (char *)malloc(sizeof(char) * strlen(*cmdLine));
 		strcpy(tmp->cmdLine, *cmdLine);
 		(cmdHist->size)++;

 		cmdHist->head = tmp;
 	}
 	else
 	{
 		//remove
 		CmdNode* tmprm = NULL;
		tmprm = cmdHist->head;
		CmdNode* obl = NULL; //	one before last
	
 	 	for( ;tmprm->next != cmdHist->tail; tmprm = tmprm->next);
 	 		obl = tmprm;
 	 	//printf("obl %s\n", obl->cmdLine);

 	 	free(cmdHist->tail);
 	 	cmdHist->tail = obl;
 	 	obl->next = NULL;

 		//add
 		CmdNode* tmpadd = NULL;
 		tmpadd = (CmdNode*)malloc(sizeof(CmdNode));
 		
 		tmpadd->next = cmdHist->head;
 		tmpadd->cmdLine = (char *)malloc(sizeof(char) * strlen(*cmdLine));
 		strcpy(tmpadd->cmdLine, *cmdLine);
 		
 		cmdHist->head = tmpadd;
 	}	
}

void PrintHistory(CommandHistory* cmdHist)
{
	int count = 0;
	CmdNode* tmp = NULL;
	tmp = cmdHist->head;
	char* msgEmpty = "History is empty\n";
    	
	if(tmp == NULL)
	{
    	write(0, msgEmpty, strlen(msgEmpty));
		return;
	}
	while(tmp != NULL)
	{
	 	count++;
	 	write(0, tmp->cmdLine, strlen(tmp->cmdLine));
	 	tmp = tmp->next;
	}
	//printf("Length: %d\n", count);
}

void WriteHistory(CommandHistory* cmdHist)
{
	CmdNode* tmp = NULL;
	tmp = cmdHist->head;

	cmdHist->pFile = fopen(cmdHist->fileName, "w");

	while(tmp != NULL)
	{
		printf("%s", tmp->cmdLine);
	
		//write(cmdHist->fileDesc, (char*)tmp->cmdLine, sizeof(tmp->cmdLine));
		fprintf(cmdHist->pFile, tmp->cmdLine);
		tmp = tmp->next;
	}
	fclose(cmdHist->pFile);
}

void ReadHistory(CommandHistory* cmdHist)
{
    char *cmdLine = NULL;
	size_t len = 0;
	cmdHist->pFile = fopen(cmdHist->fileName, "r");
	if (cmdHist->pFile == NULL)
        exit(EXIT_FAILURE);

	while(getline(&cmdLine, &len, cmdHist->pFile) != -1)
	{
		printf("%s", cmdLine);
		AddCmdToHistory(cmdHist, &cmdLine);
	}
	//PrintHistory(cmdHist->head);
	fclose(cmdHist->pFile);
}

void DestroyHistoryList(CommandHistory* cmdHist)
{
 	CmdNode* tmp_rm = NULL;
	int count = 0;
	int i;
	CmdNode* obl = NULL; //	one before last
	for(i = 0; i < cmdHist->size; i++)
	{
		tmp_rm = cmdHist->head;
		
		//printf("Find obl\n");
		if(tmp_rm == cmdHist->tail)
		{
			//printf("I will delete %s... ", tmp_rm->cmdLine);
 	
			free(cmdHist->head);
			count++;
			//printf("I've done it\n");

			break;
		}

 	  	for( ;tmp_rm->next != cmdHist->tail; tmp_rm = tmp_rm->next);
 	  		obl = tmp_rm;
 		//printf("I will delete %s... ", obl->next->cmdLine);
 	
 	  	free(cmdHist->tail);
 	  	cmdHist->tail = obl;
 	  	obl->next = NULL;
 	  	count++;

 		//printf("I've done it\n");
 	}
 	printf("deleted %d elements\n", count);
 	cmdHist->size = 0;
 	cmdHist->head = NULL;
	cmdHist->tail = NULL;
}

void SendHistoryToFIFO(CommandHistory* cmdHist)
{
	CmdNode* tmp = NULL;
	tmp = cmdHist->head;
	char* msgEmpty = "History is empty\n";
	
	mkfifo(FIFO_NAME, 0666);
	char s[300];
	int num, fd;

	printf("waiting for readers...\n");
	fd = open(FIFO_NAME, O_WRONLY);
	
	printf("got a reader--type some stuff\n");
    	
	if(tmp == NULL)
	{
		write(fd, msgEmpty, strlen(msgEmpty));
    	return;
	}
	while(tmp != NULL)
	{
		write(fd, tmp->cmdLine, strlen(tmp->cmdLine));
	 	tmp = tmp->next;
	}

	close(fd);
}