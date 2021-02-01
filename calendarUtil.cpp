#include <string>
#include "calendarUtil.hpp"
#include "objectives.hpp"
#include "util.hpp"

char *wDayName[] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};
char *wDayShort[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"};

using namespace std;

/*
    Creates the calendar.
*/
Calendar *createCalendar() {
    Calendar *calendar = new Calendar;
    calendar->periodSched = nullptr;
    return calendar;
}

/*
    Order schedules by date.
*/
void sortCalendar() {
    calendar->schedules.sort(schedComp);
}

/*
    Fills calendar with to-do schedules.
*/
void updateCalendarSchedules(Calendar *calendar, Todo *todo) {
    for (auto it = todo->schedules.begin(); it != todo->schedules.end(); it++) {
        calendar->schedules.push_back(*it);
    }
}

/*
    Fills calendar with sub-to-dos of todo.
*/
void updateCalendarTodo(Calendar *calendar, Todo *todo) {
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        Todo *subtodo = *it;
        if (subtodo->status == TODO_HABIT) {
            calendar->habits.push_back(subtodo);
        }
        updateCalendarSchedules(calendar, subtodo);
        updateCalendarTodo(calendar, subtodo);
    }
}

/*
    Fills calendar with to-dos of task.
*/
void updateCalendarTask(Calendar *calendar, Task *task) {
    updateCalendarTodo(calendar, task->rootTodo);

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        updateCalendarTask(calendar, *it);
    }
}

/*
    Updates the calendar.
*/
void updateCalendar() {
    calendar->habits.clear();
    calendar->schedules.clear();

    updateCalendarTask(calendar, rootTask);
}

/*
    Prints all to-dos with no specified dates.
*/
void printWeekSummary() {
    int i;
    long int curWeek[2];
    long int curTime;
    long int weekProgress;
    long int charInterval = 900;
    long int objStart;
    long int weekTime = 0;
    long int dayTime = 0;
    string formatText;
    list<Period *> periodList;
    getPeriodsFromTask(periodList, rootTask);
    sortPeriods(periodList);
    curTime = getCurrentTime();
    objStart = getTime(25, 8, 2019, 0, 0, 0);
    weekProgress = (curTime - objStart) % SECS_IN_A_WEEK;

    curWeek[0] = curTime - weekProgress;
    curWeek[1] = curWeek[0] + SECS_IN_A_WEEK;

    auto pit = periodList.begin();
    for (i = 0; objStart + i < curWeek[1]; i += charInterval) {
        long int charStart = objStart + i;
        long int charEnd = charStart + charInterval;
        long int prodTime = 0;
    int h;
        if (i == SECS_IN_A_WEEK) {
            printf("\n");
        }
        if (i % SECS_IN_A_WEEK == 0) {
            weekTime = 0;
            printf("\n                         %ldth objective week\n\n", 1 + (charEnd - objStart) / SECS_IN_A_WEEK);
      printf("    Horas    ");
      for (h = 0; h < 24; h++) {
        printf("|%02d ", h);
      }
      printf("\n");
        }
        if (i % 86400 == 0) {
            formatText = formatDate(charStart);
            dayTime = 0;
            printf("  %s %s [", wDayShort[(i / 86400) % 7], formatText.c_str());
        }
        if (charStart > curTime) printf(" ");
        else {
            while (pit != periodList.end() && (*pit)->end <= charStart) pit++;
            if (pit == periodList.end() || (*pit)->start >= charEnd) {
                printf(".");
            } else {
                while (pit != periodList.end() && (*pit)->start < charEnd) {
                    prodTime += min((*pit)->end, charEnd) - max((*pit)->start, charStart);
                    pit++;
                }
                pit--;
                dayTime += prodTime;
                if ((double) prodTime / charInterval > 0.5) printf("#");
                else printf(".");
            }
        }
        if ((i + charInterval) % 86400 == 0) {
            weekTime += dayTime;
            formatText = formatDur(dayTime);
            printf("] %s\n", formatText.c_str());
        }
        
        if ((i + charInterval) % SECS_IN_A_WEEK == 0) {
            formatText = formatDur(weekTime);
            printf("\n  Total time: %s\n", formatText.c_str());
        }
    }
    printf("\n");
}