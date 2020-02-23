#ifndef TASK_H
#define TASK_H
/*************************************/
#include "objectives.h"
#include "period.h"
#include "todo.h"
#include "util.h"

int findTaskByName(Task *, char *);

int findTaskByCode(Task *, char *);

Task *getSubtask(Task *, char *);

Task *createTask(char *, char *);

void freeTask(Task *task);

void addSubtask(Task *);

void deleteTask(Task *);

void removeSubtask(Task *);

void listSubtasks(Task *);

long int getTaskTotalTime(Task *);

void showTaskTotalTime(Task *);

void renameTask(Task *);

void changeCode(Task *);

bool allCompleted(Task *);

void setSubtaskStatus(Task *);

void subtasksMenu(Task *);

Task *taskMenu(Task *);
/*************************************/
#endif
