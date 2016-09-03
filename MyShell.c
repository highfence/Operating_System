// Lastest modify : 16.09.03

/* define */
#define _CRT_SECURE_NO_WARNINGS
#define MAX_LINE_NUM 256
#define MAX_CMD_NUM 128
#define BUFFER_SIZE 1024


/* static */
const char prompt[] = "MyOwnShell> ";
const char delimiter[] = " \t\n";

/* enum */
static const char * Month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


/* Structral Function */
void CmdProcess(char*);
int CmdParsing(char*, char**, int*, int*);
int ActFunction(char**, int, int*, int*);
void cmdRedir(char**, int*, int*, int*);


/* inner command */
int innerCmd(char**, int);
void cmdLs(char**, int);
void cmdLl(char**, int);
void cmdCp(char**, int);
void cmdRm(char**, int);
void cmdMv(char**, int);
void cmdCd(char**, int);
void cmdPwd(char**, int);
void cmdMkdir(char**, int);
void cmdRmdir(char**, int);

/* Outer command */
int outerCmd(char**, int);

/* Pipe */
void cmdPipe(char**, int, int*);

/* include */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>


int main(void)
{
	char cmdLine[MAX_LINE_NUM];

	while (1)
	{
		printf("%s", prompt);
		fflush(stdout);

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
	
	/* If command has a pipe of redir, Idx has cmdStr idx value.
	 * If not, they has Zero. */
	int pipeIdx = 0;
	int redirIdx = 0;
	
	cmdCount = CmdParsing(cmdLine, cmdStr, &pipeIdx, &redirIdx);
	
	if (cmdCount == 0){
		return;
	}
	
	if (ActFunction(cmdStr, cmdCount, &pipeIdx, &redirIdx) == 0 && cmdCount != 0) {
		printf("I can't find your command! type [--help]. \n");
	}

	return;
}


int CmdParsing(char* cmdLine, char** cmdStr, int* pipeIdx, int* redirIdx)
{
	int count = 1;
	int idx;

	if (cmdLine[0] == '\n')
	{
		return 0;
	}

	cmdStr[0] = strtok(cmdLine, delimiter);

	while (((cmdStr[count]) = (strtok(NULL, delimiter))) != 0)
	{
		++count;
	}
	
	for (idx = 0; cmdStr[idx] != NULL; ++idx) {
		if (!strcmp(cmdStr[idx], "|")) { *pipeIdx = idx; }
		else if (!strcmp(cmdStr[idx], ">")) { *redirIdx = idx; }
	}
	
	return count;
}


int ActFunction(char** cmdStr, int cmdCount, int* pipeIdx, int* redirIdx)
{
	int fd = -1; // For redirection.

	if (*pipeIdx != 0){
		cmdPipe(cmdStr, cmdCount, pipeIdx);
	}
		
	if (*redirIdx != 0){
		cmdRedir(cmdStr, &cmdCount, redirIdx, &fd);
	}

	/* Checking commands with innerCommand */
	if (innerCmd(cmdStr, cmdCount)){
		return 1;
	}
	
	/* Outer command operation */
	if (outerCmd(cmdStr, cmdCount)) {
		return 1;
	}
	
	/* Redirection Stdout recovery */
	if (redirIdx != 0){
		
		if (dup2(fd, 1) == -1){
			printf("Stdout recovery failed. \n");
		}
		return 1;
	}

	return 0;

}


/* function command ls */
void cmdLs(char** cmdStr, int cmdCount)
{
	DIR *dir = NULL;
	struct dirent *dir_entry;
	
	if (!cmdCount) {
		printf("Func cmdLs entry error! cmdCount is zero. \n");
	}

	else if (cmdCount == 1) {
		dir = opendir(".");
	}

	else if (cmdCount == 2) {
		if (!strcmp(cmdStr[1], "--help")) {
			printf("[ls] : show files in this directory \n");
			printf("[ls -a] : show files and directory in this directory \n");
			printf("[ls -l] : show files in this directory with long listing \n");
			return;
		}
		else if (!strcmp(cmdStr[1], "-l")) {
			cmdLl(cmdStr, cmdCount);
			return;
		}
		else {
			dir = opendir(cmdStr[1]);
		}
	}

	else if (cmdCount >= 3) {
		printf("Wrong command. if you want to know how to use, type [ls --help] \n");
		return;
	}

	if (NULL != dir) {
		while ((dir_entry = readdir(dir)) != 0) {
			printf("%s\n", dir_entry->d_name);			
		}
		closedir(dir);
	}
	else {
		printf("Can't open the directory \n");
	}

	return;
}


/* function command ll */
void cmdLl(char** cmdStr, int cmdCount)
{
	DIR *dir;
	int size;
	struct dirent *dir_entry;
	struct stat stat_file;
	struct passwd *usr;
	struct group *grp;
	struct tm *time;
	
	
	if (cmdCount == 0 || cmdStr[0] == NULL){
		printf("Func cmdLl entry error! cmdCount is zero. \n");
	}
	
	else if (cmdCount == 2 && !strcmp(cmdStr[1], "--help")){
		printf("[ll] : show files in this directory with long listing \n");
	}
	
	else if ((cmdCount == 1 && !strcmp(cmdStr[0], "ll")) || (cmdCount == 2 && !strcmp(cmdStr[0], "ls") && !strcmp(cmdStr[1], "-l"))){
		dir = opendir(".");
		
		if (dir != NULL){
			while ((dir_entry = readdir(dir)) != 0)
			{
			
				if (stat(dir_entry->d_name, &stat_file) == -1){
					printf("Func stat error!");
					return;
				}
				
				
				/* If dir_entry is DIR, print "d", if not, print "-" */
				if (S_ISDIR(stat_file.st_mode)){ printf("d"); }
				else { printf("-"); }
				
				/* Permission of User print part */
				printf((stat_file.st_mode & S_IRUSR) ? "r" : "-");
				printf((stat_file.st_mode & S_IWUSR) ? "w" : "-");
				printf((stat_file.st_mode & S_IXUSR) ? "x" : "-");
				
				
				/* Permission of Group print part */
				printf((stat_file.st_mode & S_IRGRP) ? "r" : "-");
				printf((stat_file.st_mode & S_IWGRP) ? "w" : "-");
				printf((stat_file.st_mode & S_IXGRP) ? "x" : "-");
				
				
				/* Permission of Other print part */
				printf((stat_file.st_mode & S_IROTH) ? "r" : "-");
				printf((stat_file.st_mode & S_IWOTH) ? "w" : "-");
				printf((stat_file.st_mode & S_IXOTH) ? "x" : "-");
				
				
				/* Print hard link */
				printf("%-3d", stat_file.st_nlink);
				
				/* Print user & group name */
				usr = getpwuid(stat_file.st_uid);
				printf("%-7s", usr->pw_name);
				grp = getgrgid(stat_file.st_gid);
				printf("%-7s", grp->gr_name);
				
				/* Print byte of file */
				if (!S_ISDIR(stat_file.st_mode)){
					size = stat_file.st_size;
					printf("%-10d", size);
				}
				else { printf("          "); }
				
				/* Print create time of file */
				time = localtime(&stat_file.st_ctime);
				printf("  %-s ", Month[time->tm_mon]);
				printf("%-2d %-2d:%-2d", time->tm_mday, time->tm_hour, time->tm_min);
				
				/* Print file name */
				printf("     %-20s", dir_entry->d_name);
				printf("\n");
			}
		}
		closedir(dir);
	}
	
	else {
		printf("Wrong command. if you want to know, type [ll --help] \n");
		return;
	}
	
	return;
	
}


void cmdCp(char** cmdStr, int cmdCount)
{

	int targetFile;
	int copyPoint;
	int readSize, writeSize;
	char buffer[BUFFER_SIZE];
	
	/* Exception handling */
	if (cmdCount == 0 || cmdStr[0] == NULL){
		printf("Func cmdCp entry error! cmdCount is zero. \n");
		return;
	}
	
	
	/* When user type "--help" or wrong command */
	if (cmdCount == 2 && !strcmp(cmdStr[0], "cp") && !strcmp(cmdStr[1], "--help")){
		printf("[cp file_name copy_name] : select file in this directory (file_name) and copy it (copy_name) \n");
		return;
	}
	
	else if (cmdCount != 3) {
		printf("Wrong command. if you want to know how to use, type [cp --help] \n");
		return;
	}
	
	
	/* Open targetFile only can read, and copyPoint make new file which can read and write */
	targetFile = open(cmdStr[1], O_RDONLY);
	copyPoint = open(cmdStr[2], O_RDWR|O_CREAT|O_EXCL, 0664);
	
	/* File open failure handling */
	if (targetFile == -1 || copyPoint == -1){
		printf("Func cmdCp error! can't open the file. \n");
		return;
	}
	
	
	/* Read the targetFile and copy it with exception handling */
	readSize = read(targetFile, buffer, BUFFER_SIZE);
	if (readSize == -1) { printf("Func cmdCp error! can't read the file. \n"); }
	
	writeSize = write(copyPoint, buffer, readSize);
	if (writeSize == -1) { printf("Func cmdCp error! can't write the file. \n"); }
		
	
	/* Take times for case when file's size is over than BUFFER_SIZE */
	while (readSize == BUFFER_SIZE) {
		readSize = read(targetFile, buffer, BUFFER_SIZE);
		writeSize = write(copyPoint, buffer, readSize);
	}
	
	
	close(targetFile);
	close(copyPoint);
	
	return;
}

void cmdRm(char** cmdStr, int cmdCount)
{
	/* Exception handling */
	if (cmdCount <= 0 || cmdStr[0] == NULL) {
		printf("Func cmdRm entry error! cmdCount is zero. \n");
		return;
	}
	
	else if (cmdCount < 2 || cmdCount > 3) {
		printf("Wrong command. if you want to know how to use, type [rm --help] \n");
		return;
	}
	
	else if (cmdCount == 2 && !strcmp(cmdStr[1], "--help")){
		printf("[rm file_name] : remove file (file_name) in this directory. \n ");
		return;
	}
	
	/* Delete link of file */
	unlink(cmdStr[1]);
	return;
	
}


void cmdMv(char** cmdStr, int cmdCount)
{
	int root;

	/* Exception handling */
	if (cmdCount <= 0 || cmdStr[0] == NULL){
		printf("Func cmdMv entry error! cmdStr[0] is NULL. \n");
	}
	
	else if (cmdCount < 3 || cmdCount > 4){
		printf("Wrong command. if you want to know how to use, type [mv --help] \n");
		return;
	}
	
	else if (cmdCount == 2 && !strcmp(cmdStr[1], "--help")){
		printf("[mv file_name target_directory/file_name] : move file to target directory. \n");
		printf("[mv file_name change_name] : change file name \n");
	}
	
	root = link(cmdStr[1], cmdStr[2]);
	if (root == -1){
		printf("Func cmdMv link error! \n");
		return;
	}
	
	root = unlink(cmdStr[1]);
	if (-1 == root){
		printf("Func cmdMv unlink error! \n");
		return;
	}

	
	return;
}

void cmdCd(char** cmdStr, int cmdCount){
	
	/* Exception handling */
	if (cmdCount != 2){
		printf("Wrond command. If you want to know how to use [cd], type [cd --help]. \n");
	}
	
	if ((cmdCount == 2) && (!strcmp(cmdStr[0], "cd") && (!strcmp(cmdStr[1], "--help")))){
		printf("[cd directory] : Move present directory to input directory. \n");
	}
	
	if (chdir(cmdStr[1]) == -1){
		printf("Command cmdCd error! can't change directory. plz try again. \n");
		return;
	}
	
	return;
}

void cmdPwd(char** cmdStr, int cmdCount){
	
	char buffer[BUFFER_SIZE];
	
	/* Exception handling */
	if (cmdCount == 0 || cmdStr[0] == NULL){
		printf("Command cmdPwd entry error! wrong data entry. \n");
	}
	
	if (cmdCount > 2){
		printf("Wrong command. If you want to know how to use command [pwd], type [pwd --help]. \n");
	}
	
	if ((cmdCount == 2) && (!strcmp(cmdStr[1], "--help"))){
		printf("[pwd] : Check directory which you working now. \n");
	}
	
	if (getcwd(buffer, BUFFER_SIZE) == NULL){
		printf("Command cmdPwd error! getcwd can't operate. \n");
	}
	
	/* Print present working directory. */
	else {
		printf("%s\n", buffer);
	}
	
}

void cmdMkdir(char** cmdStr, int cmdCount){
	
	/* Exception handling */
	if (cmdCount == 0 || cmdStr[0] == NULL){
		printf("Command cmdMkdir entry error! wrong data entry. \n");
	}
	
	if (cmdCount != 2){
		printf("Wrong command. If you know how to use command [mkdir], type [mkdir --help]. \n");
	}
	
	if ((cmdCount == 2) && (!strcmp(cmdStr[1], "--help"))){
		printf("[mkdir directory] : Make new directory which name you typed. \n");
	}
	
	if (mkdir(cmdStr[1],S_IRUSR|S_IWUSR|S_IXUSR) == -1){
		printf("Command mkdir failed! Check the rest of directory. Can't make same directory on one directory. \n");
		return;
	}
	
	/* Make new directory */
	return;
	
}


void cmdRmdir(char** cmdStr, int cmdCount){

	/* Exception handling */
	if (cmdCount == 0 || cmdStr[0] == NULL){
		printf("command cmdRmdir entry error! wrong data entry. \n");
	}
	
	if (cmdCount > 2) {
		printf("Wrong command. If you know how to use command [rmdir], type [rmdir --help]. \n");
	}
	
	if ((cmdCount == 0) && (!strcmp(cmdStr[1], "--help"))){
		printf("[rmdir directory] : Remove directory which you typed. Only operate when directory hasn't files of other directory. \n");
	}
	
	if (rmdir(cmdStr[1]) == -1){
		printf("Remove failed! Check 3 condition. \n");
		printf("1. Check the directory name perfect. \n");
		printf("2. Check is the directory empty. \n");
		printf("3. Check the directory is using now. \n");
		return;
	}
	
	else {
		return;
	}
	

}


void cmdRedir(char** cmdStr, int* cmdCount, int* redirIdx, int* outFd){
	
	int idx = *redirIdx;
	int fd;
	
	if (cmdStr[idx]) {
		
		if (!cmdStr[idx + 1]){
			printf("Wrong redirection command. \n");
			printf("[command > object] : command redirection to object. \n");
			return;
		}
		
		else {
			if ((fd = open(cmdStr[idx + 1], O_RDWR | O_CREAT | S_IROTH, 0644)) == -1){
				printf("Command cmdRedir error! Can't open fd. \n");
				return;
			}
		}
		
		/* Saving stdout for recovery */
		if ((*outFd = dup(1)) == -1) {
			printf("Saving stdout fd error! \n");
			return;
		}
		
		dup2(fd, STDOUT_FILENO);
		close(fd);
		
		cmdStr[idx] = NULL;
		cmdStr[idx+1] = NULL;
		
		for (idx = idx; cmdStr[idx] != NULL; idx++){
			cmdStr[idx] = cmdStr[idx + 2];
		}
		
		cmdStr[idx] = NULL;
		*cmdCount -= 2;
	}
	
	return;
	
}

int innerCmd(char** cmdStr, int cmdCount){
	
	
	if (!strcmp(cmdStr[0], "exit") || !strcmp(cmdStr[0], "quit"))
	{
		exit(0);
	}
	
	if (!strcmp(cmdStr[0], "--help"))
	{
		printf("[ls] : show files in this directory \n");
		printf("[ls -a] : show files and directory in this directory \n");
		printf("[ls -l] : show files in this directory with long listing \n");
		printf("[ll] : show files in this directory with long listing \n");
		printf("[mv file_name target_directory/file_name] : move file to target directory. \n");
		printf("[mv file_name change_name] : change file name \n");
		printf("[rm file_name] : remove file (file_name) in this directory. \n ");
	}

	else if (!strcmp(cmdStr[0], "ls")) {
		cmdLs(cmdStr, cmdCount);
	}
	
	else if (!strcmp(cmdStr[0], "ll")) {
		cmdLl(cmdStr, cmdCount);
	}
	
	else if (!strcmp(cmdStr[0], "cp")) {
		cmdCp(cmdStr, cmdCount);
	}
	
	else if (!strcmp(cmdStr[0], "rm")) {
		cmdRm(cmdStr, cmdCount);
	}
	
	else if (!strcmp(cmdStr[0], "mv")) {
		cmdMv(cmdStr, cmdCount);
	}
	
	else if (!strcmp(cmdStr[0], "cd")) {
		cmdCd(cmdStr, cmdCount);
	}
	
	else if (!strcmp(cmdStr[0], "pwd")) {
		cmdPwd(cmdStr, cmdCount);
	}
	
	else if (!strcmp(cmdStr[0], "mkdir")){
		cmdMkdir(cmdStr, cmdCount);
	}
	
	else {
		return 0;
	}
	
	return 1;
}


int outerCmd(char** cmdStr, int cmdCount) {
	
	pid_t pid;
	char pathName[MAX_LINE_NUM] = { 0 };
	int bgSig = 0;
	pid = fork();

	if (!strcmp(cmdStr[cmdCount - 1], "&")){
		// background process exist.
		bgSig = 1;
		cmdStr[cmdCount - 1] = NULL;
	
	
		switch(pid) {
			case -1 :
				fprintf(stderr, "Command outerCmd error! Fork operation failed. \n");
				return 0;
		
			case 0 :
				/* Child process operation */
				cmdStr[cmdCount] = NULL;
				strcpy(pathName, cmdStr[0]);
			
			
				if (execvp(pathName, cmdStr) == -1) {
					printf("%s : Command not found. \n", cmdStr[0]);
				}
				exit(0);
				break;

			default :
				/* Parent process operation */
				if (bgSig == 0){
					wait(NULL);
				}
			
				else {
				/* Background process */
					cmdStr[cmdCount - 1] = NULL;
					printf("[bg] %d : %s\n", pid, cmdStr[cmdCount - 2]);
					wait(NULL);
					printf("PID %d is terminated. \n", pid);
				}
		}
		return 1;
	}
	
	else {
		return 0;
	}
	
}


void cmdPipe(char** cmdStr, int cmdCount, int* pipeIdx){
	
	char* writeMsg[BUFFER_SIZE];
	char* readMsg[BUFFER_SIZE];
	pid_t pid;
	int fd[2];
	int idx, readIdx;
	
	
	
	for (idx = 0; idx < *pipeIdx; idx++){
		writeMsg[idx] = cmdStr[idx];
	}
	
	++idx;
	writeMsg[idx] = NULL;
	
	for (readIdx = 0; cmdStr[idx] != NULL; idx++){
		readMsg[readIdx] = cmdStr[idx];
		++readIdx;
	}
			
	if (pipe(fd) == -1){
		printf("Command cmdPipe error! pipe failed. \n");
		return;
	}
	
	pid = fork();
	
	
	switch (pid){
		
		case -1 :
			printf("Command cmdPipe error! fork failed. \n");
			break;
			
		case 0 :
			// Parent process, close read discripter.
			close(fd[0]);
			dup2(fd[1], STDOUT_FILENO);
			close(fd[1]);
			execvp(writeMsg[0], writeMsg);
			break;
			
		default :
			// Child process.
			close(fd[1]);
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			execvp(readMsg[0], readMsg);
			exit(0);
			
	}

	return;
}







