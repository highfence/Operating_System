

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
void cmdLs(char**, int);
void cmdLl(char**, int);

/* include */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>


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

	if (cmdCount == 0 || !ActFunction(cmdStr, cmdCount))
	{
		return;
	}


	return;
}


int CmdParsing(char* cmdLine, char** cmdStr)
{
	int count = 1;

	if (cmdLine[0] == NULL || cmdLine[0] == '\n')
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
	if (cmdCount == 0 || cmdStr[0] == NULL)
	{
		printf("ActFunction Input Error!\n");
		return 0;
	}

	if (!strcmp(cmdStr[0], "exit") || !strcmp(cmdStr[0], "quit"))
	{
		exit(0);
	}

	else if (!strcmp(cmdStr[0], "ls")) {
		cmdLs(cmdStr, cmdCount);
	}
	return 0;

}


/* function command ls */
void cmdLs(char** cmdStr, int cmdCount)
{
	DIR *dir_info;
	struct dirent *dir_entry;

	if (!cmdCount) {
		printf("Func cmdLs entry error! cmdCount is zero. \n");
	}

	else if (cmdCount == 1) {
		dir_info = opendir(".");
	}

	else if (cmdCount == 2) {
		if (!strcmp(cmdStr[1], "--help")) {
			printf("[ls] : show files in this direction \n");
			printf("[ls -l] : show files in this dirction with long listing \n");
			return;
		}
		else if (!strcmp(cmdStr[1], "-l")) {
			cmdLl(cmdStr, cmdCount);
		}
		else {
			dir_info = opendir(cmdStr[1]);
		}
	}

	else if (cmdCount >= 3) {
		printf("Wrong command. if you want to know, type [ls --help] \n");
		return;
	}

	if (dir_info != NULL) {
		while (dir_entry = readdir(dir_info)) {
			printf("%sn", dir_entry->d_name);
		}
		closedir(dir_info);
	}

	return;
}




















