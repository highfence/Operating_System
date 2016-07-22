

// define
#define _CRT_SECURE_NO_WARNINGS
#define MAX_LINE_NUM 256
#define MAX_CMD_NUM 128

// static
const char prompt[] = "MyOwnShell> ";
const char delimiter[] = " \t\n";

// function
void CmdProcess(char*);
int CmdParsing(char*, char**);


#include <stdio.h>
#include <string.h>




int main(void)
{
	char cmdLine[MAX_LINE_NUM];

	while (1)
	{
		printf("%s", prompt);

		if (fgets(cmdLine, MAX_LINE_NUM, stdin) == NULL)
		{
			return 1;
		}

		fflush(stdout);

		CmdProcess(cmdLine);

	}

	return 0;
}



void CmdProcess(char* cmdLine)
{
	int cmdCount;
	char* cmdStr[MAX_CMD_NUM];
	int idx = 0;

	cmdCount = CmdParsing(cmdLine, cmdStr);

	// only for test
	printf("Number of Command : %d \n", cmdCount);

	for (idx = 0; idx < cmdCount; idx++)
	{
		printf("Command #%d : %s\n", idx + 1, cmdStr[idx]);
	}


	return;
}


int CmdParsing(char* cmdLine, char** cmdStr)
{
	int count = 1;

	cmdStr[0] = strtok(cmdLine, delimiter);

	while (cmdStr[count] = strtok(NULL, delimiter))
	{
		++count;
	}

	return count;
}