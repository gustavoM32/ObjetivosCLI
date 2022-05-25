#include "calendar.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "calendarUtil.hpp"
#include "help.hpp"
#include "io.hpp"
#include "objectives.hpp"
#include "todo.hpp"
#include "util.hpp"

using namespace std;

const map<string, int> WEEKDAYS({
    {"dom", 0},
    {"sun", 0},
    {"seg", 1},
    {"mon", 1},
    {"ter", 2},
    {"tue", 2},
    {"qua", 3},
    {"wed", 3},
    {"qui", 4},
    {"thu", 4},
    {"sex", 5},
    {"fri", 5},
    {"sab", 6},
    {"sat", 6}
});

/*
    Creates a schedule for a todo.
*/
// void scheduleTodoCalendar() {
//     int id, day, mon, hour, min;
//     Todo *todo;
//     time_t date;
//     struct tm *structTime;
//     int args;
//     int estimate;
    
//     id = stoi(getToken(1)) - 1;
//     todo = ithTodo(calendar->habits, id);

//     if (todo == nullptr) return;

//     args = getNComms();

//     if (args != 2) {
//         sscanf(getToken(2), "%d/%d", &day, &mon);

//         date = getCurrentTime();
//         structTime = localtime(&date);

//         if (structTime->tm_mon > mon) structTime->tm_year++;
//         structTime->tm_sec = 0;
//         structTime->tm_mon = mon - 1;
//         structTime->tm_mday = day;
//         if (args == 5) {
//             sscanf(getToken(3), "%d:%d", &hour, &min);
//         } else if (args == 4) {
//             hour = min = 0;
//         }
//         structTime->tm_hour = hour;
//         structTime->tm_min = min;
//         estimate = 60 * stof(getToken(args - 1));
//         date = mktime(structTime);
//         printf("To-do date scheduled to %02d/%02d/%04d.\n\n", day, mon, 1900 + structTime->tm_year);
//     } else {
//         estimate = 0;
//         date = 0;
//         printf("Todo added to calendar.\n\n");
//     }

//     createSchedule(todo, args == 5, date, estimate);

//     updateCalendar();
// }

void editScheduleTime() {
    int id, hour, min;
    
    id = calendar->schedules.size() - stoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;

    if (getNComms() == 2) {
        sched->timeSet = 0;
        sched->date = changeTime(sched->date, 0, 0, 0);
        cout << "Hora removida.\n\n";
    } else {
        if (sscanf(getToken(2).c_str(), "%d:%d", &hour, &min) == 1) {
            min = 0;
        }
        sched->timeSet = 1;
        sched->date = changeTime(sched->date, hour, min, 0);
        printf("Changed scheduled time to %02d:%02d.\n\n", hour, min);
    }

}

void editScheduleDate() {
    int id, day, mon, year;
    
    id = calendar->schedules.size() - stoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;

    if (getNComms() == 2) {
        sched->timeSet = 0;
        sched->date = 0;
        cout << "Data removida.\n\n";
    } else {
        if (sscanf(getToken(2).c_str(), "%d/%d/%d", &day, &mon, &year) == 2) {
            sched->date = changeDate(sched->date, day, mon, getYear(getCurrentTime()));
            printf("Changed scheduled date to %02d/%02d.\n\n", day, mon);
        } else {
            sched->date = changeDate(sched->date, day, mon, year);
            printf("Changed scheduled date to %02d/%02d/%04d.\n\n", day, mon, year);
        }
        sched->timeSet = 0;
        sched->date = changeTime(sched->date, 0, 0, 0);
    }
}

void editScheduleEstimate() {
    int id;
    
    id = calendar->schedules.size() - stoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;

    sched->timeEstimate = 60 * stof(getToken(2));
    printf("Changed time estimate to %.1f\n\n", stof(getToken(2)));
}

void postponeSchedule() {
    // post id delay_in_days [keep]
    // post id next_weekday [keep]
    int id = calendar->schedules.size() - stoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;
    
    // treat prioritized schedules as yesterday
    if (sched->date == 0) sched->date = changeTime(getCurrentTime() - SECS_IN_A_DAY, 0, 0, 0);

    string delay_string = getToken(2);
    int delay;

    try {
        delay = stoi(delay_string);
    } catch (const std::invalid_argument& ia) {
        if (WEEKDAYS.find(delay_string) != WEEKDAYS.end()) {
            int cur_weekday = getWeekday(sched->date);
            int delay_weekday = WEEKDAYS.at(delay_string);
            delay = (6 + delay_weekday - cur_weekday) % 7 + 1;
        } else {
            printf("Invalid value for schedule delay.\n\n");
            return;
        }
    }

    sched->date += SECS_IN_A_DAY * delay;
    
    if (getNComms() == 3 || getToken(3) != "keep") {
        sched->timeSet = 0;
        sched->date = changeTime(sched->date, 0, 0, 0);
    }
    printf("Postponed schedule for %d day(s).\n\n", delay);
}

/*
    Edits schedule attributes.
*/
void delaySchedule() {
    int id = calendar->schedules.size() - stoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;

    int delay = stoi(getToken(2));
    if (sched->date == 0) sched->date = changeTime(getCurrentTime() - SECS_IN_A_DAY, 0, 0, 0);
    sched->date += 3600 * delay;
    sched->timeSet = 1;
    printf("Delayed schedule for %d hour(s).\n\n", delay);
}

/*
    Delete schedule.
*/
void removeSchedule() {
    int id = calendar->schedules.size() - stoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;
    
    if (sched == calendar->periodSched) {
        cout << "Não é possível remover agendamento enquanto seu período está rodando\n\n";
        return;
    }

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
    int id = calendar->schedules.size() - stoi(getToken(1));
    Schedule *sched = ithSchedule(calendar->schedules, id);

    if (sched == nullptr) return;

    if (sched == calendar->periodSched) {
        cout << "Não é possível concluir uma tarefa enquanto seu período está rodando\n\n";
        // TODO: REFACTOR: make function to stop a period, then call it here
        return;
    }

    Todo *todo = sched->todo;

    if (!changeTodoStatus(todo, TODO_COMPLETED)) return;

    cout << "To-do \"" << todo->task << " > " << todo->name << "\" completed.\n\n";

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

    id = stoi(getToken(1)) - 1;
    todo = ithTodo(calendar->habits, id);

    if (todo == nullptr) return;
    
    period = new Period;
    period->todo = todo;
    todo->periods.push_back(period);

    period->start = period->end = getCurrentTime();

    printf("Hábito \"%s > %s\" cumprido!\n\n", period->todo->task->code.c_str(), period->todo->name.c_str());
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
        Schedule *sched = calendar->periodSched;
        todo = sched->todo;
        period = todo->periods.back();

        period->end = getCurrentTime();
        time_t taskDur = period->end - period->start;

        string formatedDur = formatDur(taskDur);
        printf("Período \"%s > %s\" encerrado. Duração: %s.\n\n", period->todo->task->code.c_str(), period->todo->name.c_str(), formatedDur.c_str());
    }

    id = calendar->schedules.size() - stoi(getToken(1));
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
    TodoStatus status;
    int id = stoi(getToken(1)) - 1;

    Todo *todo = ithTodo(calendar->habits, id);

    if (todo == nullptr) return;

    if (getToken(2) == "remove") {
        status = TODO_PENDING;
    } else if (getToken(2) == "complete") {
        status = TODO_COMPLETED;
    } else {
        printf("Argumento inválido.\n\n");
        return;
    }

    todo->status = status;

    updateCalendar();

    printf("Estado do to-do \"%s\" foi alterado para \"%s\".\n\n", todo->name.c_str(), getTodoStatusName(status).c_str());
}

void printHabits(bool printAll) {
    int i = 0;
    int printed = 0;
    for (auto it = calendar->habits.begin(); it != calendar->habits.end(); it++) {
        Todo *todo = *it;
        i++;
        if (!printAll && habitToday(todo)) continue;
        cout << "    " << getColor("BRIGHT_BLUE") << setw(2) << i << ". " << getColor("BRIGHT_WHITE");
        cout << getTodoFullName(todo) << colorString(" (" + to_string(countHabitRecord(todo)) + "%)\n", "BRIGHT_CYAN");
        printed++;
    }

    cout << "    ";
    if (printed == 0) {
        if (printAll) cout << "Não há hábitos\n";
        else cout << "Nenhum hábito pendente!\n";
    }

    cout << "\n";
}

/*
    Prints scheduled to-dos.
*/
void printScheduled() {
    int i = 1;
    time_t curDayStart = getDayStart(getCurrentTime());
    int totalEstimated = 0;
    int timeSetEstimated = 0;
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
                    cout << "\n";
                    printTitle("Priorizados", SECONDARY_LEVEL);
                }
            } else if (sched->date < curDayStart) {
                if (printLate) {
                    printLate = false;
                    cout << "\n";
                    printTitle("Atrasados", SECONDARY_LEVEL);
                }
            } else {
                while (sched->date < dayStart) {
                    dayStart -= SECS_IN_A_DAY;
                    localtime_r(&dayStart, &date);
                    if (totalEstimated != 0) {
                        cout << getColor("BRIGHT_BLUE") << "    Total " << setprecision(0) << fixed << getColor("CYAN") << setw(2) << timeSetEstimated / 60.0 << "h";
                        cout << " " << getColor("BRIGHT_CYAN") << setw(2) << totalEstimated / 60.0 << "h\n" << getColor("BRIGHT_WHITE");
                        totalEstimated = 0;
                        timeSetEstimated = 0;
                    }
                    stringstream dateString;
                    dateString << wDayShort[date.tm_wday] << " (" << setfill('0') << setw(2) << date.tm_mday << "/" << setw(2) << date.tm_mon + 1 << setfill(' ') << ")";
                    cout << "\n";
                    printTitle(dateString.str(), SECONDARY_LEVEL);
                }
            }

            cout << "    " << getColor("BRIGHT_BLUE") << setw(2) << i << ". " << getColor("BRIGHT_WHITE");

            cout << getColor("CYAN");
            if (sched->timeSet) {
                cout << setfill('0') << setw(2) << schedDate.tm_hour << ":"  << setw(2) << schedDate.tm_min << setfill(' ') << " ";
            } else if (sched->date != 0) {
                cout << "--:-- ";
            } else {
                cout << "..... ";
            }

            cout << getColor("BRIGHT_CYAN");
            if (sched->timeEstimate == 0) cout << " 0h ";
            else if (sched->timeEstimate / 60.0 < 1.0) cout << "<1h ";
            else cout << setprecision(0) << fixed << setw(2) << sched->timeEstimate / 60.0 << "h ";

            cout << getColor("BRIGHT_WHITE");

            cout << getTodoFullName(sched->todo) << "\n";

            totalEstimated += sched->timeEstimate;
            if (sched->timeSet) timeSetEstimated += sched->timeEstimate;
            i++;
            it++;
            if (printTotalLast && (it == calendar->schedules.rend() || (*it)->date < curDayStart)) {
                printTotalLast = false;
                cout << getColor("BRIGHT_BLUE") << "    Total " << setprecision(0) << fixed << getColor("CYAN") << setw(2) << timeSetEstimated / 60.0 << "h";
                cout << " " << getColor("BRIGHT_CYAN") << setw(2) << totalEstimated / 60.0 << "h\n" << getColor("BRIGHT_WHITE");
            }
        } while (it != calendar->schedules.rend());
    }

    cout << "\n";

    printTitle("Hábitos", SECONDARY_LEVEL);
    printHabits(false);

    printLine(MAIN_LEVEL);
}

/*
    Enters calendar menu.
*/
void calendarMenu() {
    string commandName;
    updateCalendar();
    printScheduled();
    while (true) {
        // printf(" _________________________  Calendar  _________________________\n\n");
        periodWarning();
        commandName = getCommandName();
        /*if (commandName == "sched") {
            if (getNComms() == 2 || getNComms() == 4 || getNComms() == 5) {
                scheduleTodoCalendar();
                printScheduled();
            } else {
                printf("Número inválido de argumentos.\n\n");
            }
        } else */if (commandName == "time") {
            if (getNComms() == 2 || getNComms() == 3) {
                editScheduleTime();
                printScheduled();
                saveAll();
            } else {
                printf("Número inválido de argumentos.\n");
            }
        } else if (commandName == "date") {
            if (getNComms() == 2 || getNComms() == 3) {
                editScheduleDate();
                printScheduled();
                saveAll();
            } else {
                printf("Número inválido de argumentos.\n");
            }
        } else if (commandName == "est") {
            if (validArgs(2)) {
                editScheduleEstimate();
                printScheduled();
                saveAll();
            }
        } else if (commandName == "post") {
            if (getNComms() == 3 || getNComms() == 4) {
                postponeSchedule();
                printScheduled();
                saveAll();
            } else {
                printf("Número inválido de argumentos.\n");
            }
        } else if (commandName == "delay") {
            if (validArgs(2)) {
                delaySchedule();
                printScheduled();
                saveAll();
            }
        } else if (commandName == "clear") {
            if (validArgs(1)) {
                removeSchedule();
                printScheduled();
                saveAll();
            }
        } else if (commandName == "complete") {
            if (validArgs(1)) {
                completeTodo();
                printScheduled();
                saveAll();
            }
        } else if (commandName == "start") {
            if (validArgs(1)) {
                startPeriod();
                saveAll();
            }
        } else if (commandName == "habit") {
            if (validArgs(1)) {
                touchTodo();
                saveAll();
                printScheduled();
            }
        } else if (commandName == "habits") {
            if (validArgs(0)) {
                printTitle("Hábitos", MAIN_LEVEL);
                printHabits(true);
                printLine(MAIN_LEVEL);
            }
        } else if (commandName == "stop") {
            if (validArgs(0)) {
                stopPeriod();
                saveAll();
            }
        } else if (commandName == "cancel") {
            if (validArgs(0)) {
                cancelPeriod();
                saveAll();
            }
        } else if (commandName == "status") {
            if (validArgs(0)) {
                showTaskPeriodTime();
                saveAll();
            }
        } else if (commandName == "cal") {
            if (validArgs(0)) {
                printScheduled();
            }
        } else if (commandName == "cd") {
            if (validArgs(1)) {
                if (getToken(1) == "..") {
                    curMenu = lastMenu;
                    return;
                } else {
                    printf("Digite 'cd ..' para voltar\n\n");
                }
            }
        } else if (generalCommands(commandName)) return;
    }
}
