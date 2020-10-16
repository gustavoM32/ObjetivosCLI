#ifndef CALENDAR_HPP
#define CALENDAR_HPP

#include "objectives.hpp"

void printNoDateTodos(Calendar *calendar, Task *root);

void printWeekSummary(Task *root);

void periodWarning();

Calendar *createCalendar();

void calendarMenu();

#endif
