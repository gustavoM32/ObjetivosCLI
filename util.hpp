#ifndef UTIL_HPP
#define UTIL_HPP
/*************************************/
#include <string>
#include <list>
#include <stdio.h>
#include "objectives.hpp"

enum Color {
	BLACK = 30,
	RED = 31,
	GREEN = 32,
	YELLOW = 33,
	BLUE = 34,
	MAGENTA = 35,
	CYAN = 36,
	WHITE = 37,
	BRIGHT_BLACK = 90,
	BRIGHT_RED = 91,
	BRIGHT_GREEN = 92,
	BRIGHT_YELLOW = 93,
	BRIGHT_BLUE = 94,
	BRIGHT_MAGENTA = 95,
	BRIGHT_CYAN = 96,
	BRIGHT_WHITE = 97
};

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

std::string escapeNewLines(std::string s);

std::string unescapeNewLines(std::string s);

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

time_t getTodoTime(Todo *todo, int option = TIME_OPTION_NONE);

time_t getTodoTotalTime(Todo *todo, int option = TIME_OPTION_NONE);

void getIdPath(std::string path, std::list<size_t> &ids);

std::string getTodoFullName(Todo *todo);

std::string getTodoPath(Todo *todo);

bool todoCompleted(Todo *todo);

std::string colorString(std::string text, Color color);

std::string getColor(Color color);

void printTitle(std::string text, std::string symbol, bool extraSpace = true);

void printLine(std::string symbol, bool extraSpace = true);
/*************************************/
#endif
