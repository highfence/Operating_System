

/* define */
#define _CRT_SECURE_NO_WARNINGS
#define MAX_LINE_NUM 256
#define MAX_CMD_NUM 128


/* static */
const char prompt[] = "MyOwnShell> ";
const char delimiter[] = " \t\n";


/* Structral Function */
void CmdProcess(char*);
int CmdParsing(char*, char**);
int ActFunction(char**, int);


/* inner command */
void cmdLs(char**, int);
void cmdLl(char**, int);


/* include */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>


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
    
    if (cmdLine[0] == '\n')
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
            printf("[ls] : show files in this direction \n");
            printf("[ls -a] : show files and directions in this direction \n");
            printf("[ls -l] : show files in this direction with long listing \n");
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
        printf("Wrong command. if you want to know, type [ls --help] \n");
        return;
    }
    
    if (NULL != dir) {
        while (dir_entry = readdir(dir)) {
            if (!strcmp(cmdStr[1], "-a")){
                printf("%s\n", dir_entry->d_name);
            }
            else {
                if ((dir_entry->d_name)[0] != '.'){
                    printf("%s\n", dir_entry->d_name);
                }
            }
        }
        closedir(dir);
    }
    else {
        printf("Can't open the direction \n");
    }
    
    return;
}


/* function command ll */
void cmdLl(char** cmdStr, int cmdCount)
{
    DIR *dir = NULL;
    struct dirent *dir_entry;
    struct stat stat_file;
    
    if (cmdCount == 0 || cmdStr[0] == NULL){
        printf("Func cmdLl entry error! cmdCount is zero. \n");
    }
    
    else if (!strcmp(cmdStr[1], "--help")){
        printf("[ll] : show files in this direction with long listing \n");
    }
    
    else if (!strcmp(cmdStr[0], "ll")){
        dir = opendir(".");
        if (dir != NULL){
            while (dir_entry = readdir(dir))
            {
                if (stat(dir_entry->d_name, &stat_file) == -1){
                    printf("Func stat error!");
                    return;
                }
                
                /* If dir_entry is DIR, print "d", if not, print "-" */
                if (S_ISDIR(stat_file.st_mode)){ printf("d "); }
                else { printf("- "); }
                
                /* Permission of User print part */
                if (S_IRUSR) { printf("r"); }
                else { printf("-");}
                
                if (S_IWUSR) { printf("w"); }
                else { printf("-");}
                
                if (S_IXUSR) { printf("x "); }
                else { printf("- ");}
                
                /* Permission of Group print part */
                if (S_IRGRP) { printf("r"); }
                else { printf("-");}
                
                if (S_IWGRP) { printf("w"); }
                else { printf("-");}
                
                if (S_IXGRP) { printf("x "); }
                else { printf("- ");}
                
                /* Permission of Other print part */
                if (S_IROTH) { printf("r"); }
                else { printf("-");}
                
                if (S_IWOTH) { printf("w"); }
                else { printf("-");}
                
                if (S_IXOTH) { printf("x"); }
                else { printf("-");}
                
                
            }
        }
    }
    
    else {
        printf("Wrong command. if you want to know, type [ll --help] \n");
        return;
    }
    
    
    
}

















