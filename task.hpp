#ifndef TASK_HPP
#define TASK_HPP
/*************************************/
#include <string>
#include "objectives.hpp"
#include "period.hpp"
#include "todo.hpp"
#include "util.hpp"

Task *findTaskByName(Task *, std::string &);

Task *findTaskByCode(Task *, std::string &);

Task *getSubtask(Task *, std::string);

Task *createTask(std::string, std::string);

void freeTask(Task *task);

void addSubtask(Task *);

void deleteTask(Task *);

void removeSubtask(Task *);

void listSubtasks(Task *);

void showTaskTotalTime(Task *);

void renameTask(Task *);

void changeCode(Task *);

bool allCompleted(Task *);

void setSubtaskStatus(Task *);

void subtasksMenu(Task *);

void taskMenu(Task *);
/*************************************/
#endif
