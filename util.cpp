#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "util.hpp"
#include "objectives.hpp"

/**
    mallocSafe()
    This function does the same thing as malloc but exits the program if it
    can't allocate memory.
*/
void* mallocSafe(int nBytes) {
    void* ptr = malloc(nBytes);
    if (ptr == NULL) {
        printf("Out of memory! Exiting...\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

/**
    fopenSafe()
    This function does the same thing as fopen but exits the program if is
    unable to open the file.
*/
FILE* fopenSafe(char* fileName, char* mode) {
    FILE* file;
	file = fopen(fileName, mode);
    if (file == NULL) {
	    printf("Error opening file.\n\n");
	    exit(EXIT_FAILURE);
	}
    return file;
}

/**
    getCurrentTime()
    This function returns current time in seconds since the start of 1970.
*/
long int getCurrentTime() {
    time_t curTime;
    time(&curTime);
    return curTime;
}

/**
    getDayStart()
    Returns the start of the day of supplied date in local time.
*/
time_t getDayStart(time_t time) {
    struct tm *stm = localtime(&time);
    stm->tm_sec = 0;
    stm->tm_min = 0;
    stm->tm_hour = 0;
    return mktime(stm);
}

/**
    formatDur()
    This function returns in 'timeString' the 'totalTime' formated as 'h:mm:ss'.
*/
void formatDur(long int totalTime, char timeString[]) {
    int hours, minutes, seconds;
    seconds = totalTime % 60;
    totalTime /= 60;
    minutes = totalTime % 60;
    totalTime /= 60;
    hours = totalTime;
    sprintf(timeString, "%d:%02d:%02d", hours, minutes, seconds);
}

/**
    formatTime()
    This function returns in 'timeString' the 'time' formated as
    'dd/mm/yy hh:mm:ss'.
*/
void formatTime(long int time, char timeString[]) {
    struct tm *structTime;
    structTime = localtime(&time);
    sprintf(timeString, "%02d/%02d/%02d %02d:%02d:%02d",
        structTime->tm_mday, structTime->tm_mon + 1, structTime->tm_year - 100,
        structTime->tm_hour, structTime->tm_min, structTime->tm_sec);
}

/**
    formatDate()
    This function returns in 'dateString' the 'time' formated as
    'dd/mm'.
*/
void formatDate(long int time, char dateString[]) {
    struct tm *structTime;
    structTime = localtime(&time);
    sprintf(dateString, "%02d/%02d",
        structTime->tm_mday, structTime->tm_mon + 1);
}

/*
    getTime()
    Returns time in seconds of date 'day'/'month'/'year' and time
    'hour':'min':'sec'.
*/
long int getTime(int day, int month, int year, int hour, int min, int sec) {
    struct tm *structTime;
    time_t curTime = time(NULL);
    structTime = localtime(&curTime);
    structTime->tm_mday = day;
    structTime->tm_mon = month - 1;
    structTime->tm_year = year - 1900;
    structTime->tm_hour = hour;
    structTime->tm_min = min;
    structTime->tm_sec = sec;
    return mktime(structTime);
}

/*
    changeTime()
    Returns new time from 'oldTime' with changed 'hour', 'min' and 'sec'.
*/
long int changeTime(long int oldTime, int hour, int min, int sec) {
    struct tm *structTime;
    structTime = localtime(&oldTime);
    structTime->tm_hour = hour;
    structTime->tm_min = min;
    structTime->tm_sec = sec;
    return mktime(structTime);
}

/*
    changeDate()
    Returns new time from 'oldTime' with changed 'day', 'month' and 'year'.
*/
long int changeDate(long int oldTime, int day, int month, int year) {
    struct tm *structTime;
    structTime = localtime(&oldTime);
    if (year >= 1900) structTime->tm_year = year - 1900;
    else structTime->tm_year += year;
    structTime->tm_mon = month - 1;
    structTime->tm_mday = day;
    return mktime(structTime);
}

/*
    formatTaskName()
    Takes input and removes starting spaces and ends it after a newline or end
    of string character is found
*/
void formatTaskName(char* input) {
    int iId;
    int oId;
    for (iId = 0; input[iId] == ' '; iId++);
    for (oId = 0; input[iId] != '\n' && input[iId] != '\0'; oId++) {
        input[oId] = input[iId];
        iId++;
    }
    input[oId] = '\0';
}

/*
    readFileString()
    Returns in 'name' the next string in file 'input', skipping spaces and
    newline chars until the start of string and ends it when a newline char is
    found or 'size' is reached.
*/
void readFileString(char* name, int size, FILE* input) {
    char readChar;
    int i = 0;
    readChar = getc(input);
    while (readChar == '\n' || readChar == ' ') {
        readChar = getc(input);
    }
    while (readChar != '\n' || i == size - 1) {
        name[i] = readChar;
        i++;
        readChar = getc(input);
    }
    name[i] = '\0';
}

/*
    toLowercase()
    Converts string 's' to lowercase.
*/
char *toLowercase(char *s) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        if ('A' <= s[i] && s[i] <= 'Z') {
            s[i] += 'a' - 'A';
        }
    }
    return s;
}

/*
    toUppercase()
    Converts string 's' to uppercase.
*/
char *toUppercase(char *s) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        if ('a' <= s[i] && s[i] <= 'z') {
            s[i] += 'A' - 'a';
        }
    }
    return s;
}

/*
    copyFile()
    Copy file with name orig to dest.
*/
void copyFile(char *orig, char *dest) {
    FILE *input;
    FILE *output;
    char c;
    input = fopen(orig, "r");
    output = fopen(dest, "w");
    if (input == NULL || output == NULL) {
        printf("Erro na cópia de \"%s\" para \"%s\"!\n", orig, dest);
        return;
    }
    while ((c = getc(input)) != EOF) {
        fprintf(output, "%c", c);
    }
    fclose(input);
    fclose(output);
}

/*
    notAvailable()
    Warns user that the command doesn't exist in current scope.
*/
void notAvailable(char* userCommand) {
    printf("Command \"%s\" does not exist or is not available.\n\n", userCommand);
}

/*
    countTasks()
    DOCUMENTAR.
*/
int countTasks(Task *task, char *code) {
    int i;
    int count = 0;
    if (strcmp(task->code, code) == 0) count++;
    for (i = 0; i < task->nSubtasks; i++) {
        count += countTasks(task->subtasks[i], code);
    }
    return count;
}

/*
    setUPath()
    DOCUMENTAR.
*/
void setUPath(Task *root, Task *task) {
    int i;
    char uniquePath[PATH_LEN + 3 + CODE_LEN];
    if (countTasks(root, task->code) == 1 || task->parent == NULL) {
        strcpy(task->uniquePath, task->code);
    } else {
        sprintf(uniquePath, "%s > %s", task->parent->uniquePath, task->code);
        strcpy(task->uniquePath, uniquePath);
    }
    for (i = 0; i < task->nSubtasks; i++) {
        setUPath(root, task->subtasks[i]);
    }
}

/*
    countTodosTodo()
    Count the number of to-dos in a todo.
*/
int countTodosTodo(Todo *todo) {
    int res = 0;
    int i;
    for (i = 0; i < todo->nSubtodos; i++) {
        if (todo->subtodos[i]->status != TODO_COMPLETED)
            res += 1 + countTodosTodo(todo->subtodos[i]);
    }
    return res;
}

/*
    countTodosTask()
    Count the number of to-dos in a task.
*/
int countTodosTask(Task *task) {
    int res = 0;
    int i;
    for (i = 0; i < task->nTodos; i++) {
        if (task->todos[i]->status != TODO_COMPLETED)
            res += 1 + countTodosTodo(task->todos[i]);
    }
    for (i = 0; i < task->nSubtasks; i++) {
        res += countTodosTask(task->subtasks[i]);
    }
    return res;
}

/*
    isInList()
    Returns 1 if a word is in an array of words
*/

int isInList(char word[], int size, char array[][COMMAND_LEN]) {
    int i;
    for (i = 0; i < size; i++) {
        if (strcmp(word, array[i]) == 0) return true;
    }
    return false;
}