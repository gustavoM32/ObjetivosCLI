#ifndef TODO_H
#define TODO_H
/*************************************/
#include <time.h>
#include "objectives.h"

Todo* createTodo(char *name, int type);

void freeTodo(Todo *todo);

void addTodo(Task* task);

void addTodoEsp(Task* task);

void removeTodo(Task* task);

void changeTodoDate(Task *task);

void startPeriod(Task* task);

void savePeriodTime(Task* task);

void stopPeriod(Task* task);

void cancelPeriod(Task* task);

void showTaskPeriodTime(Task* task);

void listTodos(Task* task);

void todosMenu(Task* task);
/*************************************/
#endif
