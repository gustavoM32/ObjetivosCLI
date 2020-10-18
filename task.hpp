#ifndef TASK_HPP
#define TASK_HPP
/*************************************/
#include "objectives.hpp"
#include "period.hpp"
#include "todo.hpp"
#include "util.hpp"

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

void taskMenu(Task *);
/*************************************/
#endif
