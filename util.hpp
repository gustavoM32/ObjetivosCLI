#ifndef UTIL_HPP
#define UTIL_HPP
/*************************************/
#include <string>
#include <list>
#include <stdio.h>
#include "objectives.hpp"

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

int countTodosTodo(Todo *todo);

int countTodosTask(Task *task);

int isInList(std::string word, std::list<std::string> l);

Task *ithTask(std::list<Task *> &tasks, size_t i);

Todo *ithTodo(std::list<Todo *> &todos, size_t i);

Period *ithPeriod(std::list<Period *> &periods, size_t i);

Schedule *ithSchedule(std::list<Schedule *> &schedules, size_t i);

Note *ithNote(std::list<Note *> &notes, size_t i);

bool periodComp(const Period *p1, const Period *p2);

bool schedComp(const Schedule *sched1, const Schedule *sched2);

void sortPeriods(std::list<Period *> &periods);

void getPeriodsFromTodo(std::list<Period *> &periods, Todo *todo);

void getPeriodsFromTask(std::list<Period *> &periods, Task *task);

time_t getTodoTime(Todo *todo);

time_t getTodoTotalTime(Todo *todo);

void getIdPath(std::string path, std::list<size_t> &ids);

std::string getTodoFullName(Todo *todo);

std::string getTodoPath(Todo *todo);

bool todoCompleted(Todo *todo);
/*************************************/
#endif
