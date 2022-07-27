#pragma once

extern std::string wDayName[];
extern std::string wDayShort[];

// Calendar *createCalendar();

void updateCalendar();

void sortCalendar();

// void updateCalendarSchedules(Calendar *calendar, Todo *todo);

// void updateCalendarTodo(Calendar *calendar, Todo *todo);

// void updateCalendarTask(Calendar *calendar, Task *task);

void printWeekSummary(bool showAll);
