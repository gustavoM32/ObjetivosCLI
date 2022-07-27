#pragma once

#include <string>
#include <time.h>
#include "objectives.hpp"

Todo* createTodo(std::string name, Task *task, Todo *parent = nullptr);

void freeTodo(Todo *todo);

std::string getTodoStatusName(TodoStatus status);

bool habitToday(Todo *todo);

int countHabitRecord(Todo *todo);

void addTodo(Task* task);

bool changeTodoStatus(Todo *todo, int status);

void createSchedule(Todo *todo, int timeSet, time_t date, int estimate);

void removeTodo(Task* task);

void listTodos(Task* task, bool showHidden);

void todosMenu(Task* task);
