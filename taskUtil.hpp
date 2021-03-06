#pragma once

#include <list>
#include "objectives.hpp"

// Task *findTaskByName(Task* parent, string &name);

// Task *findTaskByCode(Task* parent, string &code);

Task* getSubtask(Task* task);

// Task* createTask(string name, string code);

// void freeTask(Task *task);

void listStatusTasks(std::list<Task *> tasks, std::string statusName);

time_t getTaskTotalTime(Task* task);

// void showTaskTotalTime(Task* task);

void showTaskWeekTime(Task* task);

// bool allCompleted(Task* task);

Task *searchTask();

void printTaskTree();

bool codeUsed(Task *task, std::string &code);

Task *getColorRoot(Task *task);

std::string getTaskColor(Task *task);

std::string getMotivation(Task *task);

void printRecentHistory(Task* task);

void removeTrailingNewLines(std::string &text);
