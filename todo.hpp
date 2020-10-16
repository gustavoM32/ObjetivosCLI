#ifndef TODO_HPP
#define TODO_HPP
/*************************************/
#include <time.h>
#include "objectives.hpp"

Todo* createTodo(char *name, int type);

void freeTodo(Todo *todo);

void addTodo(Task* task);

void createSchedule(Todo *todo, int timeSet, time_t date, int estimate);

void removeTodo(Task* task);

void listTodos(Task* task, int showCompleted);

void todosMenu(Task* task);
/*************************************/
#endif
