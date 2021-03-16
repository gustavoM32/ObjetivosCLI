#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include "calendarUtil.hpp"
#include "objectives.hpp"
#include "taskUtil.hpp"
#include "util.hpp"

using namespace std;

string wDayName[] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};
string wDayShort[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"};

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
void printWeekSummary(bool showAll) {
    long int curWeek[2];
    long int charInterval = 900;
    long int curTime = getCurrentTime();
    long int objStart = getTime(25, 8, 2019, 0, 0, 0);
    long int weekProgress = (curTime - objStart) % SECS_IN_A_WEEK;

    list<Period *> periodList;
    getPeriodsFromTask(periodList, rootTask);
    sortPeriods(periodList);

    // bug check
    Period *maxp = periodList.front();
    for (auto it = periodList.begin(); it != periodList.end(); it++) {
        Period *p = *it;
        if (maxp->end - maxp->start < p->end - p->start) maxp = p;
    }

    long int maxDur = maxp->end - maxp->start;
    if (maxDur > 36000) {
        cout << "Período com duração anormal!\n";
        cout << maxp->todo->task->code << ": " << formatDur(maxDur) << "\n";
        return;
    }

    auto pit = periodList.begin();

    curWeek[0] = curTime - weekProgress;
    curWeek[1] = curWeek[0] + SECS_IN_A_WEEK;

    long int weekStart;

    if (showAll) weekStart = objStart;
    else weekStart = curWeek[0] - SECS_IN_A_WEEK;

    while (weekStart < curWeek[1]) {
        long int weekTime = 0;
        int weekN = 1 + (weekStart - objStart) / SECS_IN_A_WEEK;

        long int otherProd = 0;
        map<Task *, long int> coloredProd;

        printTitle(to_string(weekN) + "ª semana de objetivos", SECONDARY_LEVEL);
        
        cout << colorString("    Horas    ", BRIGHT_BLUE);
        for (int h = 0; h < 24; h++) {
            cout << colorString("|", BRIGHT_BLUE) << setfill('0') << getColor(BRIGHT_CYAN) << setw(2) << h << getColor(BRIGHT_WHITE) << " ";
        }
        cout << "\n";

        for (int d = 0; d < 7; d++) {
            long int dayStart = weekStart + d * SECS_IN_A_DAY;
            long int dayTime = 0;

            cout << "  " << colorString(wDayShort[d] + " " + formatDate(dayStart), BRIGHT_CYAN) << colorString(" [", BRIGHT_BLUE);

            for (int t = dayStart; t < dayStart + SECS_IN_A_DAY; t += charInterval) {
                long int prodTime = 0;
                map<Task *, long int> taskProd;
                if (t > curTime) cout << " ";
                else {
                    while (pit != periodList.end() && (*pit)->end <= t) pit++;
                    if (pit == periodList.end() || (*pit)->start >= t + charInterval) {
                        cout << colorString("-", BRIGHT_BLACK);
                    } else {
                        while (pit != periodList.end() && (*pit)->start < t + charInterval) {
                            long int inter = periodIntersect((*pit)->start, (*pit)->end, t, t + charInterval);
                            prodTime += inter;
                            Task *colorRoot = getColorRoot((*pit)->todo->task);
                            if (taskProd.find(colorRoot) == taskProd.end()) {
                                taskProd[colorRoot] = inter;
                            } else {
                                taskProd[colorRoot] += inter;
                            }
                            pit++;
                        }
                        Task *maxTime = taskProd.begin()->first;
                        for (auto it = taskProd.begin(); it != taskProd.end(); it++) {
                            Task *colorRoot = getColorRoot(it->first);
                            if (colorRoot == nullptr) otherProd += it->second;
                            else if (coloredProd.find(colorRoot) == coloredProd.end()) {
                                coloredProd[colorRoot] = it->second;
                            } else {
                                coloredProd[colorRoot] += it->second;
                            }
                            if (it->second > taskProd[maxTime]) maxTime = it->first;
                        }
                        pit--;
                        dayTime += prodTime;

                        int color = getTaskColor(maxTime);
                        double percentCharInterval = (double) prodTime / charInterval > 0.5;

                        if (percentCharInterval >= 0.5) cout << colorString("▓", (Color) color);
                        else if (percentCharInterval > 0) cout << colorString("▒", (Color) color);
                        else cout << colorString("░", (Color) color);
                    }
                }
            }

            weekTime += dayTime;
            cout << colorString("] ", BRIGHT_BLUE) << colorString(formatDur(dayTime), BRIGHT_CYAN) << "\n";
        }

        cout << "\n  " << colorString("Total " + formatDur(weekTime), BRIGHT_BLUE) << "    ";
        for (auto it = coloredProd.begin(); it != coloredProd.end(); it++) {
            cout << colorString(it->first->code + " " + formatDur(it->second), (Color) it->first->color) << "    ";
        }
        cout << "Outros " << formatDur(otherProd) << "\n\n";
        
        weekStart += SECS_IN_A_WEEK;
    }
}