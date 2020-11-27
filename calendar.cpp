#include <iostream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "calendar.hpp"
#include "objectives.hpp"
#include "io.hpp"
#include "util.hpp"
#include "help.hpp"
#include "todo.hpp"

using namespace std;

char *wDayName[] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};
char *wDayShort[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"};

/*
    updateCalendarSchedules()
    Fills calendar with to-do schedules.
*/
void updateCalendarSchedules(Calendar *calendar, Todo *todo) {
    for (auto it = todo->schedules.begin(); it != todo->schedules.end(); it++) {
        calendar->schedules.push_back(*it);
    }
}

/*
    updateCalendarTodo()
    Fills calendar with sub-to-dos of todo.
*/
void updateCalendarTodo(Calendar *calendar, Todo *todo) {
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        Todo *subtodo = *it;
        if (subtodo->status == TODO_PRIORITY) {
            calendar->todos.push_back(subtodo);
        }
        updateCalendarSchedules(calendar, subtodo);
        updateCalendarTodo(calendar, subtodo);
    }
}

/*
    updateCalendarTask()
    Fills calendar with to-dos of task.
*/
void updateCalendarTask(Calendar *calendar, Task *task) {
    for (auto it = task->todos.begin(); it != task->todos.end(); it++) {
        Todo *todo = *it;
        if (todo->status == TODO_PRIORITY) {
            calendar->todos.push_back(todo);
        }
        updateCalendarSchedules(calendar, todo);
        updateCalendarTodo(calendar, todo);
    }

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        updateCalendarTask(calendar, *it);
    }
}

/*
    schedComp
    Schedule compare function.
*/
bool schedComp(const Schedule *sched1, const Schedule *sched2) {
    if (sched1->date < sched2->date) return true;
    if (sched1->date > sched2->date) return false;
    if (sched1->timeSet) return false;
    if (sched2->timeSet) return true;
    return false;
}

/*
    sortCalendar()
    Order schedules by date.
*/
void sortCalendar() {
    calendar->schedules.sort(schedComp);
}

void getTodoFullName(Todo *todo, string &name) {
    if (todo->type == ROOT) {
        name += todo->parent.task->uniquePath;
    } else {
        getTodoFullName(todo->parent.todo, name);
    }

    name += " > ";
    name += todo->name;
}

/*
    printScheduled()
    Prints scheduled to-dos.
*/
void printScheduled() {
    int i = 1;
    time_t curDayStart = getDayStart(getCurrentTime());
    int totalSpent = 0, totalEstimated = 0;
    bool printLate = true;
    bool printCommon = true;
    bool printTotalLast = true;

    sortCalendar();

    printf(" ___________________  Scheduled  ___________________\n");

    if (calendar->schedules.size() == 0) {
        printf("\n    No scheduled to-dos.\n");
    } else {
        Schedule *sched = calendar->schedules.back();
        struct tm date, schedDate;
        localtime_r(&(sched->date), &date);
        time_t dayStart = getDayStart(sched->date) + SECS_IN_A_DAY;

        auto it = calendar->schedules.rbegin();
        do {
            sched = *it;
            localtime_r(&(sched->date), &schedDate);

            if (sched->date == 0) {
                if (printCommon) {
                    printCommon = false;
                    printf("\n ________________  Common  _________________\n\n");
                }
            } else if (sched->date < curDayStart) {
                if (printLate) {
                    printLate = false;
                    printf("\n ________________  Late  _________________\n\n");
                }
            } else {
                while (sched->date < dayStart) {
                    dayStart -= SECS_IN_A_DAY;
                    localtime_r(&dayStart, &date);
                    if (totalEstimated != 0) {
                        printf("        Total %2.0fh\n", totalEstimated / 60.0);
                        totalSpent = totalEstimated = 0;
                    }
                    printf("\n _____________  %s (%02d/%02d)  _____________\n\n", wDayShort[date.tm_wday], date.tm_mday, date.tm_mon + 1);
                }
            }

            printf("    %2d: ", i);

            if (sched->timeSet) {
                printf("%02d:%02d ", schedDate.tm_hour, schedDate.tm_min);
            } else if (sched->date != 0) {
                printf("--:-- ");
            }

            string todoName;
            getTodoFullName(sched->todo, todoName);

            if (sched->date != 0) {
                if (sched->timeEstimate == 0) printf(" 0h ");
                else if (sched->timeEstimate / 60.0 < 1.0) printf("<1h ");
                else printf("%2.0fh ", sched->timeEstimate / 60.0);
            }
            
            cout << todoName << endl;

            totalSpent += sched->timeSpent;
            totalEstimated += sched->timeEstimate;
            i++;
            it++;
            if (printTotalLast && (it == calendar->schedules.rend() || (*it)->date < curDayStart)) {
                printTotalLast = false;
                printf("        Total %2.0fh\n", totalEstimated / 60.0);
            }
        } while (it != calendar->schedules.rend());
    }
    printf(" ___________________________________________________\n\n");
}

/*
    printPrioritized()
    Prints prioritized to-dos.
*/
void printPrioritized() {
    printf(" __________________  Prioritized  __________________\n\n");

    printf("         Spent   Sched   Todo name\n");
    int i = 1;
    for (auto it = calendar->todos.begin(); it != calendar->todos.end(); it++) {
        Todo *todo = *it;
        string todoName;
        getTodoFullName(todo, todoName);
        printf("    %2d: %5.1fh  %5ld     %s\n", i + 1, todo->timeSpent / 60.0, todo->schedules.size(), todoName.c_str());
        i++;
    }

    printf(" ___________________________________________________\n\n");
}

/*
    printNoDateTodos()
    Prints all to-dos with no specified dates.

*/
// void printNoDateTodos(Calendar *calendar, Task *root) {
//     int item;
//     Todo *curTodo;
//     setUPath(root, root);
//     calendar->nItems = 0;
//     updateCalendar(calendar, root);
//     if (calendar->nItems == 0) {
//         printf("No to-dos without date.\n\n");
//         return;
//     }
//     printf("  ********************  Sem data  ********************\n\n");
//     for (item = 0; item < calendar->nItems; item++) {
//         if ((curTodo = calendar->items[item])->planned != 0) continue;
//         printf("      * %s (%s)\n\n", curTodo->name, curTodo->parent->uniquePath);
//     }
// }

/*
    periodComp
    Period compare function.
*/
bool periodComp(const Period *p1, const Period *p2) {
    if (p1->start < p2->start) return true;
    if (p1->start > p2->start) return false;
    return false;
}

/*
    sortPeriods()
    Order periods by time.
*/
void sortPeriods(list<Period *> &periods) {
    periods.sort(periodComp);
}

/*
    getPeriods()
    Inserts all TASK periods into PERIODS and the vector size into N.
*/
void getPeriods(list<Period *> &periods, Task *task) {
    for (auto it = task->periods.begin(); it != task->periods.end(); it++) {
        periods.push_back(*it);
    }
    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        getPeriods(periods, *it);
    }
}

/*
    printWeekSummary()
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
    getPeriods(periodList, rootTask);
    sortPeriods(periodList);
    curTime = getCurrentTime();
    objStart = getTime(25, 8, 2019, 0, 0, 0);
    weekProgress = (curTime - objStart) % SECS_IN_A_WEEK;

    curWeek[0] = curTime - weekProgress;
    curWeek[1] = curWeek[0] + SECS_IN_A_WEEK;

    auto pit = periodList.begin();
    for (i = 0; objStart + i < curWeek[1]; i += charInterval) {
        int charStart = objStart + i;
        int charEnd = charStart + charInterval;
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

/*
    updateCalendar()
    Updates the calendar.
*/
void updateCalendar() {
    calendar->todos.clear();
    calendar->schedules.clear();

    updateCalendarTask(calendar, rootTask);
    setUPath(rootTask, rootTask);
}

/*
    createCalendar()
    Creates the calendar.
*/
Calendar *createCalendar() {
    Calendar* calendar = new Calendar;
    calendar->periodSched = nullptr;
    return calendar;
}

/*
    startPeriod()
    Starts a period of todo.
*/
void startPeriod() {
    Period *period;
    int id;
    Task *task;
    Todo *todo;
    string todoName;

    if (calendar->periodSched != nullptr) {
        printf("There is already a period running.\n\n");
        return;
    }

    id = calendar->schedules.size() - atoi(getToken(1));
    calendar->periodSched = ithSchedule(calendar->schedules, id);

    if (calendar->periodSched == nullptr) return;
    
    todo = calendar->periodSched->todo;
    getTodoFullName(todo, todoName);

    while (todo->type != ROOT) todo = todo->parent.todo;
    task = todo->parent.task;

    period = new Period;
    task->periods.push_back(period);

    period->start = period->end = getCurrentTime();
    period->name = todoName;

    printf("Period \"%s\" started. Type 'stop' when done.\n\n", period->name.c_str());
}

/*
    stopPeriod()
    Stops running period.
// */
void stopPeriod() {
    Period* period;
    Task *task;
    Todo *todo;
    Schedule *sched;
    long int taskDur = 0;
    string formatedDur;

    if (calendar->periodSched == nullptr) {
        printf("There is no period running.\n\n");
        return;
    }

    sched = calendar->periodSched;
    todo = sched->todo;
    while (todo->type != ROOT) todo = todo->parent.todo;
    task = todo->parent.task;
    period = task->periods.back();

    period->end = getCurrentTime();
    taskDur = period->end - period->start;
    sched->timeSpent += taskDur / 60;
    sched->todo->timeSpent += taskDur / 60;

    formatedDur = formatDur(taskDur);
    printf("Period \"%s\" stopped. Duration: %s.\n\n", period->name.c_str(), formatedDur.c_str());
    calendar->periodSched = nullptr;
}

/*
    cancelPeriod()
    Cancels running period.
*/
void cancelPeriod() {
    Period* period;
    Task *task;
    Todo *todo;
    long int taskDur = 0;
    string formatedDur;

    if (calendar->periodSched == nullptr) {
        printf("There is no period running.\n\n");
        return;
    }

    todo = calendar->periodSched->todo;
    while (todo->type != ROOT) todo = todo->parent.todo;
    task = todo->parent.task;
    period = task->periods.back();

    taskDur = getCurrentTime() - period->start;
    task->periods.pop_back();
    delete period;

    formatedDur = formatDur(taskDur);
    printf("Period \"%s\" canceled. Duration: %s.\n\n", period->name.c_str(), formatedDur.c_str());
    calendar->periodSched = nullptr;
}

/*
    showTaskPeriodTime()
    Prints and saves total time in running period.
*/
void showTaskPeriodTime() {
    Period* period;
    Task *task;
    Todo *todo;
    long int taskDur = 0;
    string formatedDur;

    if (calendar->periodSched == nullptr) {
        printf("There is no period running.\n\n");
        return;
    }

    todo = calendar->periodSched->todo;
    while (todo->type != ROOT) todo = todo->parent.todo;
    task = todo->parent.task;
    period = task->periods.back();

    period->end = getCurrentTime();

    taskDur = period->end - period->start;
    formatedDur = formatDur(taskDur);
    printf("Time spent in period \"%s\". Duration: %s.\n\n", period->name.c_str(), formatedDur.c_str());
}

/*
    periodWarning()
    Prints warning of running period.
*/
void periodWarning() {
    Period* period;
    Task *task;
    Todo *todo;
    long int taskDur = 0;
    string formatedDur;
    Period *last = nullptr;
    list<Period *> periodList;
    getPeriods(periodList, rootTask);

    if (calendar->periodSched == NULL) {
        for (auto it = periodList.begin(); it != periodList.end(); it++) {
            if (last == nullptr || last->end < (*it)->end) last = *it;
        }
        if (last != nullptr) {
            taskDur = getCurrentTime() - last->end;
            formatedDur = formatDur(taskDur);
            printf("Time since last period: %s\n\n", formatedDur.c_str());
        }
    } else {
        todo = calendar->periodSched->todo;
        while (todo->type != ROOT) todo = todo->parent.todo;
        task = todo->parent.task;
        period = task->periods.back();

        period->end = getCurrentTime();
        taskDur = period->end - period->start;
        formatedDur = formatDur(taskDur);
        printf("Period \"%s\" running! Duration: %s.\n\n", period->name.c_str(), formatedDur.c_str());
    }
}

/*
    scheduleTodoCalendar()
    Creates a schedule for a todo.
*/
void scheduleTodoCalendar() {
    int id, day, mon, hour, min;
    Todo *todo;
    time_t date;
    struct tm *structTime;
    int args;
    int estimate;
    
    id = atoi(getToken(1)) - 1;
    todo = ithTodo(calendar->todos, id);

    if (todo == nullptr) return;

    args = getNComms();

    if (args != 2) {
        sscanf(getToken(2), "%d/%d", &day, &mon);

        date = getCurrentTime();
        structTime = localtime(&date);

        if (structTime->tm_mon > mon) structTime->tm_year++;
        structTime->tm_sec = 0;
        structTime->tm_mon = mon - 1;
        structTime->tm_mday = day;
        if (args == 5) {
            sscanf(getToken(3), "%d:%d", &hour, &min);
        } else if (args == 4) {
            hour = min = 0;
        }
        structTime->tm_hour = hour;
        structTime->tm_min = min;
        estimate = 60 * atof(getToken(args - 1));
        date = mktime(structTime);
        printf("To-do date scheduled to %02d/%02d/%04d.\n\n", day, mon, 1900 + structTime->tm_year);
    } else {
        estimate = 0;
        date = 0;
        printf("Todo added to calendar.\n\n");
    }

    createSchedule(todo, args == 5, date, estimate);

    updateCalendar();
}

/*
    removeSchedule()
    Delete schedule.
*/
void removeSchedule() {
    int id = calendar->schedules.size() - atoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;

    Todo *todo = sched->todo;

    printf("Schedule '%s' removed.\n\n", todo->name.c_str());

    todo->schedules.remove(sched);
    delete sched;

    updateCalendar();
}

/*
    completeTodo()
    Delete schedule and set its to-do to complete.
*/
void completeTodo() {
    int id = calendar->schedules.size() - atoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched != nullptr) return;

    Todo *todo = sched->todo;

    printf("Todo '%s' completed.\n\n", todo->name.c_str());

    for (auto it = todo->schedules.begin(); it != todo->schedules.end(); it++) {
        delete *it;
    }

    todo->status = TODO_COMPLETED;

    updateCalendar();
}

/*
    editSchedule()
    Edits schedule attributes.
*/
void editSchedule() {
    int id, hour, min, day, mon, year;
    
    id = calendar->schedules.size() - atoi(getToken(2));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched != nullptr) return;

    if (getNComms() == 4) {
        if (strcmp(getToken(1), "time") == 0) {
            sscanf(getToken(3), "%d:%d", &hour, &min);
            sched->timeSet = 1;
            sched->date = changeTime(sched->date, hour, min, 0);
            printf("Changed scheduled time to %02d:%02d.\n\n", hour, min);
        } else if (strcmp(getToken(1), "date") == 0) {
            if (sscanf(getToken(3), "%d/%d/%d", &day, &mon, &year) == 2) {
                sched->date = changeDate(sched->date, day, mon, 0);
                printf("Changed scheduled date to %02d/%02d.\n\n", day, mon);
            } else {
                sched->date = changeDate(sched->date, day, mon, year);
                printf("Changed scheduled date to %02d/%02d/%04d.\n\n", day, mon, year);
            }
            sched->timeSet = 0;
            sched->date = changeTime(sched->date, 0, 0, 0);
        } else if (strcmp(getToken(1), "estimate") == 0) {
            sched->timeEstimate = 60 * atof(getToken(3));
            printf("Changed time estimate to %.1f\n\n", atof(getToken(3)));
        } else {
            printf("Invalid edit option.\n\n");
        }
    } else {
        if (strcmp(getToken(1), "notime") == 0) {
            sched->timeSet = 0;
            sched->date = changeTime(sched->date, 0, 0, 0);
            printf("Removed time estimate.\n\n");
        } else {
            printf("Invalid edit option.\n\n");
        }
    }
}

/*
    delaySchedule()
    Edits schedule attributes.
*/
void delaySchedule() {
    int id = calendar->schedules.size() - atoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched != nullptr) return;

    int delay = atoi(getToken(2));
    sched->date += SECS_IN_A_DAY * delay;
    printf("Delayed scheduled for %d day(s).\n\n", delay);
    sched->timeSet = 0;
    sched->date = changeTime(sched->date, 0, 0, 0);
}

/*
    changePrioritizeStatus()
    Changes status of prioritized to-do.
*/
void changePrioritizeStatus() {
    char *statusName;
    int status;
    int id = atoi(getToken(1)) - 1;

    Todo *todo = ithTodo(calendar->todos, id);

    if (todo != nullptr) return;

    if (strcmp(getToken(2), "remove") == 0) {
        status = TODO_PENDING;
        statusName = "pending";
    } else if (strcmp(getToken(2), "complete") == 0) {
        status = TODO_COMPLETED;
        statusName = "completed";
    } else {
        printf("Invalid argument.\n\n");
    }

    todo->status = status;

    updateCalendar();

    printf("\"%s\" status is set to \"%s\".\n\n", todo->name.c_str(), statusName);
}

/*
    calendarMenu()
    Enters calendar menu.
*/
void calendarMenu() {
    char *commandName;
    updateCalendar();
    printScheduled();
    while (true) {
        printf(" _________________________  Calendar  _________________________\n\n");
        periodWarning();
        commandName = getCommandName();
        if (strcmp(commandName, "sched") == 0) {
            if (getNComms() == 2 || getNComms() == 4 || getNComms() == 5) {
                scheduleTodoCalendar();
                printScheduled();
            } else {
                printf("Invalid number of arguments.\n\n");
            }
        } else if (strcmp(commandName, "edit") == 0) {
            if (getNComms() == 3 || getNComms() == 4) {
                editSchedule();
                printScheduled();
            } else {
                printf("Invalid number of arguments.\n\n");
            }
        } else if (strcmp(commandName, "delay") == 0) {
            if (validArgs(2)) {
                delaySchedule();
                printScheduled();
            }
        } else if (strcmp(commandName, "rem") == 0) {
            if (validArgs(1)) {
                removeSchedule();
                printScheduled();
            }
        } else if (strcmp(commandName, "complete") == 0) {
            if (validArgs(1)) {
                completeTodo();
                printScheduled();
            }
        } else if (strcmp(commandName, "start") == 0) {
            if (validArgs(1)) {
                startPeriod();
                saveAll();
            }
        } else if (strcmp(commandName, "stop") == 0) {
            if (validArgs(0)) {
                stopPeriod();
                saveAll();
            }
        } else if (strcmp(commandName, "cancel") == 0) {
            if (validArgs(0)) {
                cancelPeriod();
                saveAll();
            }
        } else if (strcmp(commandName, "time") == 0) {
            if (validArgs(0)) {
                showTaskPeriodTime();
                saveAll();
            }
        } else if (strcmp(commandName, "todo") == 0) {
            if (validArgs(2)) {
                changePrioritizeStatus();
                printPrioritized();
                saveAll();
            }
        } else if (strcmp(commandName, "pri") == 0) {
            if (validArgs(0)) {
                printPrioritized();
            }
        } else if (strcmp(commandName, "cal") == 0) {
            if (validArgs(0)) {
                printScheduled();
            }
        } else if (strcmp(commandName, "cd") == 0) {
            if (validArgs(1)) {
                if (strcmp("..", getToken(1)) == 0) {
                    curMenu = lastMenu;
                    return;
                } else {
                    printf("Type 'cd ..' to go back\n\n");
                }
            }
        } else if (generalCommands(commandName)) return;
    }
}
