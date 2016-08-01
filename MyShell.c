

/* define */
#define _CRT_SECURE_NO_WARNINGS
#define MAX_LINE_NUM 256
#define MAX_CMD_NUM 128

/* static */
const char prompt[] = "MyOwnShell> ";
const char delimiter[] = " \t\n";

/* function */
void CmdProcess(char*);
int CmdParsing(char*, char**);
int ActFunction(char**, int);

/* include */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




int main(void)
{
	char cmdLine[MAX_LINE_NUM];

	while (1)
	{
		printf("%s", prompt);

		if (fgets(cmdLine, MAX_LINE_NUM, stdin) == NULL)
		{
			return 0;
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

	if (!ActFunction(cmdStr, cmdCount))
	{
		return;
	}


	return;
}


int CmdParsing(char* cmdLine, char** cmdStr)
{
	int count = 1;

	if (cmdStr[0] == NULL)
	{
		return 0;
	}

	cmdStr[0] = strtok(cmdLine, delimiter);

	while (cmdStr[count] = strtok(NULL, delimiter))
	{
		++count;
	}

	return count;
}

int ActFunction(char** cmdStr, int cmdCount)
{
	if (cmdCount == 0 || cmdStr == NULL)
	{
		printf("ActFunction Input Error!");
		return 0;
	}

	if (!strcmp(cmdStr[0], "exit") || !strcmp(cmdStr[0], "quit"))
	{
		exit(0);
	}

	return 0;
		
}























