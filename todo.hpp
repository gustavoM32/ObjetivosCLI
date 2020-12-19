#ifndef TODO_HPP
#define TODO_HPP
/*************************************/
#include <string>
#include <time.h>
#include "objectives.hpp"

Todo* createTodo(std::string name, Task *task, Todo *parent = nullptr);

void freeTodo(Todo *todo);

std::string getTodoStatusName(int status);

void addTodo(Task* task);

void createSchedule(Todo *todo, int timeSet, time_t date, int estimate);

void removeTodo(Task* task);

void listTodos(Task* task, int showHidden);

void todosMenu(Task* task);
/*************************************/
#endif
