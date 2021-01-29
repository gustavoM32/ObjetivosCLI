#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "calendar.hpp"
#include "calendarUtil.hpp"
#include "objectives.hpp"
#include "io.hpp"
#include "util.hpp"
#include "help.hpp"
#include "todo.hpp"

using namespace std;

/*
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
    Edits schedule attributes.
*/
void editSchedule() {
    int id, hour, min, day, mon, year;
    
    id = calendar->schedules.size() - atoi(getToken(2));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;

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
    Edits schedule attributes.
*/
void delaySchedule() {
    int id = calendar->schedules.size() - atoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;

    int delay = atoi(getToken(2));
    sched->date += SECS_IN_A_DAY * delay;
    printf("Delayed scheduled for %d day(s).\n\n", delay);
    sched->timeSet = 0;
    sched->date = changeTime(sched->date, 0, 0, 0);
}

/*
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
    Delete schedule and set its to-do to complete.
*/
void completeTodo() {
    int id = calendar->schedules.size() - atoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;

    Todo *todo = sched->todo;

    printf("Todo \"%s > %s\" completed.\n\n", todo->task->code.c_str(), todo->name.c_str());

    for (auto it = todo->schedules.begin(); it != todo->schedules.end(); it++) {
        delete *it;
    }

    todo->schedules.clear();

    todo->status = TODO_COMPLETED;

    updateCalendar();
}

/*
    Touch a to-do, creating a period of zero length.
*/
void touchTodo() {
    Period *period;
    int id;
    Todo *todo;
    string todoName;
    Schedule *sched;

    id = calendar->schedules.size() - atoi(getToken(1));
    sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;
    
    todo = sched->todo;

    period = new Period;
    period->todo = todo;
    todo->periods.push_back(period);

    period->start = period->end = getCurrentTime();

    printf("To-do \"%s > %s\" atualizado.\n\n", period->todo->task->code.c_str(), period->todo->name.c_str());
}


/*
    Starts a period of todo.
*/
void startPeriod() {
    Period *period;
    int id;
    Todo *todo;
    string todoName;

    if (calendar->periodSched != nullptr) {
        printf("Há um período em execução.\n\n");
        return;
    }

    id = calendar->schedules.size() - atoi(getToken(1));
    calendar->periodSched = ithSchedule(calendar->schedules, id);

    if (calendar->periodSched == nullptr) return;
    
    todo = calendar->periodSched->todo;

    period = new Period;
    period->todo = todo;
    todo->periods.push_back(period);

    period->start = period->end = getCurrentTime();

    printf("Período \"%s > %s\" iniciado. Digite 'stop' para parar.\n\n", period->todo->task->code.c_str(), period->todo->name.c_str());
}

/*
    Stops running period.
*/
void stopPeriod() {
    Period* period;
    Todo *todo;
    Schedule *sched;
    long int taskDur = 0;
    string formatedDur;

    if (calendar->periodSched == nullptr) {
        printf("Não há período em execução.\n\n");
        return;
    }

    sched = calendar->periodSched;
    todo = sched->todo;
    period = todo->periods.back();

    period->end = getCurrentTime();
    taskDur = period->end - period->start;

    formatedDur = formatDur(taskDur);
    printf("Período \"%s > %s\" encerrado. Duração: %s.\n\n", period->todo->task->code.c_str(), period->todo->name.c_str(), formatedDur.c_str());
    calendar->periodSched = nullptr;
}

/*
    Cancels running period.
*/
void cancelPeriod() {
    Period* period;
    Todo *todo;
    long int taskDur = 0;
    string formatedDur;

    if (calendar->periodSched == nullptr) {
        printf("Não há período em execução.\n\n");
        return;
    }

    todo = calendar->periodSched->todo;
    period = todo->periods.back();

    taskDur = getCurrentTime() - period->start;
    todo->periods.pop_back();
    delete period;

    formatedDur = formatDur(taskDur);
    printf("Period \"%s > %s\" cancelado. Duração: %s.\n\n", period->todo->task->code.c_str(), period->todo->name.c_str(), formatedDur.c_str());
    calendar->periodSched = nullptr;
}

/*
    Prints and saves total time in running period.
*/
void showTaskPeriodTime() {
    Period* period;
    Todo *todo;
    long int taskDur = 0;
    string formatedDur;

    if (calendar->periodSched == nullptr) {
        printf("Não há período em execução.\n\n");
        return;
    }

    todo = calendar->periodSched->todo;
    period = todo->periods.back();

    period->end = getCurrentTime();

    taskDur = period->end - period->start;
    formatedDur = formatDur(taskDur);
    printf("Tempo gasto no período \"%s > %s\": %s.\n\n", period->todo->task->code.c_str(), period->todo->name.c_str(), formatedDur.c_str());
}

/*
    Prints warning of running period.
*/
void periodWarning() {
    Period* period;
    Todo *todo;
    long int taskDur = 0;
    string formatedDur;
    list<Period *> periodList;
    getPeriodsFromTask(periodList, rootTask);

    if (calendar->periodSched == nullptr) {
        Period *last = nullptr;
        for (auto it = periodList.begin(); it != periodList.end(); it++) {
            if (last == nullptr || last->end < (*it)->end) last = *it;
        }
        if (last != nullptr) {
            taskDur = getCurrentTime() - last->end;
            formatedDur = formatDur(taskDur);
            printf("Tempo desde o último período \"%s > %s\": %s\n\n", last->todo->task->code.c_str(), last->todo->name.c_str(), formatedDur.c_str());
        }
    } else {
        todo = calendar->periodSched->todo;
        period = todo->periods.back();

        period->end = getCurrentTime();
        taskDur = period->end - period->start;
        formatedDur = formatDur(taskDur);
        printf("Período \"%s > %s\" em execução! Duração: %s.\n\n", period->todo->task->code.c_str(), period->todo->name.c_str(), formatedDur.c_str());
    }
}

/*
    Changes status of prioritized to-do.
*/
void changePrioritizedStatus() {
    int status;
    int id = atoi(getToken(1)) - 1;

    Todo *todo = ithTodo(calendar->todos, id);

    if (todo == nullptr) return;

    if (strcmp(getToken(2), "remove") == 0) {
        status = TODO_PENDING;
    } else if (strcmp(getToken(2), "complete") == 0) {
        status = TODO_COMPLETED;
    } else {
        printf("Argumento inválido.\n\n");
        return;
    }

    todo->status = status;

    updateCalendar();

    printf("Estado do to-do \"%s\" foi alterado para \"%s\".\n\n", todo->name.c_str(), getTodoStatusName(status).c_str());
}


/*
    Prints scheduled to-dos.
*/
void printScheduled() {
    int i = 1;
    time_t curDayStart = getDayStart(getCurrentTime());
    int totalEstimated = 0;
    bool printLate = true;
    bool printCommon = true;
    bool printTotalLast = true;

    sortCalendar();

    printTitle("Calendário", MAIN_LEVEL);

    if (calendar->schedules.size() == 0) {
        cout << "\n    Não há agendamentos.\n";
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
                    cout << endl;
                    printTitle("Periódicos", SECONDARY_LEVEL);
                }
            } else if (sched->date < curDayStart) {
                if (printLate) {
                    printLate = false;
                    cout << endl;
                    printTitle("Atrasados", SECONDARY_LEVEL);
                }
            } else {
                while (sched->date < dayStart) {
                    dayStart -= SECS_IN_A_DAY;
                    localtime_r(&dayStart, &date);
                    if (totalEstimated != 0) {
                        cout << getColor(BRIGHT_BLUE) << "        Total " << setprecision(0) << fixed << setw(2) << totalEstimated / 60.0 << "h\n" << getColor(BRIGHT_WHITE);
                        totalEstimated = 0;
                    }
                    stringstream dateString;
                    dateString << wDayShort[date.tm_wday] << " (" << date.tm_mday << "/" << date.tm_mon + 1 << ")";
                    cout << endl;
                    printTitle(dateString.str(), SECONDARY_LEVEL);
                }
            }

            cout << "    " << getColor(BRIGHT_BLUE) << setw(2) << i << ": " << getColor(BRIGHT_WHITE);

            cout << getColor(CYAN);
            if (sched->timeSet) {
                cout << setfill('0') << setw(2) << schedDate.tm_hour << ":" << schedDate.tm_min << setfill(' ') << " ";
            } else if (sched->date != 0) {
                cout << "--:-- ";
            }

            cout << getColor(BRIGHT_BLUE);
            if (sched->date != 0) {
                if (sched->timeEstimate == 0) cout << " 0h ";
                else if (sched->timeEstimate / 60.0 < 1.0) cout << "<1h ";
                else cout << setprecision(0) << fixed << setw(2) << sched->timeEstimate / 60.0 << "h ";

            }

            cout << getColor(BRIGHT_WHITE);

            string todoName = sched->todo->task->code;
            todoName += " > ";
            todoName += sched->todo->name;
            
            if (sched->date == 0) {
                if (!sched->todo->periods.empty()) {
                    time_t timeSince = getCurrentTime() - sched->todo->periods.back()->end;
                    cout << getColor(CYAN) << setw(9) << formatDur(timeSince) << getColor(BRIGHT_WHITE) << " ";
                }
            }
            cout << todoName;

            cout << endl;

            totalEstimated += sched->timeEstimate;
            i++;
            it++;
            if (printTotalLast && (it == calendar->schedules.rend() || (*it)->date < curDayStart)) {
                printTotalLast = false;
                cout << getColor(BRIGHT_BLUE) << "        Total " << setprecision(0) << fixed << setw(2) << totalEstimated / 60.0 << "h\n" << getColor(BRIGHT_WHITE);
            }
        } while (it != calendar->schedules.rend());
    }
    cout << endl;
    printLine(MAIN_LEVEL);
}

/*
    Prints prioritized to-dos.
*/
void printPrioritized() {
    printf(" __________________  Prioritized  __________________\n\n");

    printf("         Spent   Sched   Todo name\n");
    int i = 1;
    for (auto it = calendar->todos.begin(); it != calendar->todos.end(); it++) {
        Todo *todo = *it;
        string todoName;
        todoName = getTodoFullName(todo);
        long int timeSpent = getTodoTime(todo);
        printf("    %2d: %5.1fh  %5ld     %s\n", i + 1, timeSpent / 3600.0, todo->schedules.size(), todoName.c_str());
        i++;
    }

    printf(" ___________________________________________________\n\n");
}

/*
    Enters calendar menu.
*/
void calendarMenu() {
    char *commandName;
    updateCalendar();
    printScheduled();
    while (true) {
        // printf(" _________________________  Calendar  _________________________\n\n");
        periodWarning();
        commandName = getCommandName();
        if (strcmp(commandName, "sched") == 0) {
            if (getNComms() == 2 || getNComms() == 4 || getNComms() == 5) {
                scheduleTodoCalendar();
                printScheduled();
            } else {
                printf("Número inválido de argumentos.\n\n");
            }
        } else if (strcmp(commandName, "edit") == 0) {
            if (getNComms() == 3 || getNComms() == 4) {
                editSchedule();
                printScheduled();
            } else {
                printf("Número inválido de argumentos.\n\n");
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
        } else if (strcmp(commandName, "touch") == 0) {
            if (validArgs(1)) {
                touchTodo();
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
                changePrioritizedStatus();
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
                    printf("Digite 'cd ..' para voltar\n\n");
                }
            }
        } else if (generalCommands(commandName)) return;
    }
}
