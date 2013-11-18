#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

typedef struct CmdNode
{
	struct CmdNode* next;
	//struct CmdNode* prev;
	char* cmdLine;
}CmdNode;
//--------------------------------------------------
typedef struct CommandHistory
{
	char* fileName;
	unsigned int capacity;
	int size;
	int fileDesc;
	FILE* pFile;
	CmdNode* head;
	CmdNode* tail;
}CommandHistory;
//--------------------------------------------------

void AddCmdToHistory(CommandHistory* cmdHist, char** cmdLine);

void PrintHistory(CommandHistory* cmdHist);

void WriteHistory(CommandHistory* cmdHist);

void ReadHistory(CommandHistory* cmdHist);

void DestroyHistoryList(CommandHistory* cmdHist);

void SendHistoryToFIFO(CommandHistory* cmdHist);