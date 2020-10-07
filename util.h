#ifndef UTIL_H
#define UTIL_H
/*************************************/
#include <stdio.h>
#include "objectives.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef int bool;

enum Bool {false, true};

void* mallocSafe(int nBytes);

FILE* fopenSafe(char* fileName, char* mode);

long int getCurrentTime();

void formatDur(long int totalTime, char timeString[]);

void formatTime(long int time, char timeString[]);

void formatDate(long int time, char dateString[]);

long int getTime(int day, int month, int year, int hour, int min, int sec);

time_t getDayStart(time_t time);

long int changeTime(long int oldTime, int hour, int min, int sec);

long int changeDate(long int oldTime, int day, int month, int year);

void formatTaskName(char* input);

void readFileString(char* name, int size, FILE* input);

char *toLowercase(char *);

char *toUppercase(char *);

void copyFile(char *orig, char *dest);

void notAvailable(char* userCommand);

int countTasks(Task *task, char *code);

void setUPath(Task *root, Task *task);
/*************************************/
#endif
