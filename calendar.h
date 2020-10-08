#ifndef CALENDAR_H
#define CALENDAR_H

#include "objectives.h"

void printNoDateTodos(Calendar *calendar, Task *root);

void printWeekSummary(Task *root);

void periodWarning();

Calendar *createCalendar();

void calendarMenu();

#endif
