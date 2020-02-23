#ifndef CALENDAR_H
#define CALENDAR_H

#include "objectives.h"

Calendar *createCalendar();

void fillCalendar(Calendar *, Task *);

void sortCalendar(Todo **, int, int);

void printCalendar(Calendar *, Task *);

void printNoDateTodos(Calendar *calendar, Task *root);

void sortPeriods(Period **items, int s, int e);

void getPeriods(Period *periods[], int *n, Task *task);

void printWeekSummary(Task *root);

#endif
