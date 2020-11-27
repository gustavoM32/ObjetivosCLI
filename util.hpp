#ifndef UTIL_HPP
#define UTIL_HPP
/*************************************/
#include <string>
#include <list>
#include <stdio.h>
#include "objectives.hpp"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

void* mallocSafe(int nBytes);

FILE* fopenSafe(char* fileName, char* mode);

long int getCurrentTime();

std::string formatDur(long int totalTime);

std::string formatTime(long int time);

std::string formatDate(long int time);

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

int countTasks(Task *task, std::string &code);

void setUPath(Task *root, Task *task);

int countTodosTask(Task *task);

int isInList(std::string word, std::list<std::string> l);

Task *ithTask(std::list<Task *> &tasks, size_t i);

Todo *ithTodo(std::list<Todo *> &todos, size_t i);

Period *ithPeriod(std::list<Period *> &periods, size_t i);

Schedule *ithSchedule(std::list<Schedule *> &schedules, size_t i);

Note *ithNote(std::list<Note *> &notes, size_t i);
/*************************************/
#endif
