#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include "period.hpp"
#include "util.hpp"
#include "io.hpp"
#include "help.hpp"
#include "calendar.hpp"

using namespace std;

/*
    Edit period from task pointed by 'task' according to user input.
*/
void editPeriod(Task* task, list<Period *> &periods) {
    string ext;
    string type;
    long int old;
    long int *timestamp;
    int a, b, c;
    string oldFormatedTime;
    string newFormatedTime;
    ext = toLowercase(getToken(1));
    type = toLowercase(getToken(2));
    int id = stoi(getToken(3)) - 1;

    Period *period = ithPeriod(periods, id);
    if (period == nullptr) return;

    if (ext == "start") {
        timestamp = &(period->start);
    } else if (ext == "end") {
        timestamp = &(period->end);
    } else {
        printf("You can edit period 'start' or 'end'\n\n");
        return;
    }

    old = *timestamp;
    if (type == "time") {
        sscanf(getToken(4).c_str(), "%d:%d:%d", &a, &b, &c);
        *timestamp = changeTime(*timestamp, a, b, c);
    } else if (type == "date") {
        sscanf(getToken(4).c_str(), "%d/%d/%d", &a, &b, &c);
        *timestamp = changeDate(*timestamp, a, b, 2000 + c);
    } else {
        printf("You can edit period 'time' or 'date'\n\n");
        return;
    }
    newFormatedTime = formatTime(*timestamp);
    oldFormatedTime = formatTime(old);
    printf("Period %s %s changed from %s to %s\n\n", ext.c_str(), type.c_str(), oldFormatedTime.c_str(), newFormatedTime.c_str());
}

/*
    Ask user for a period Id of task pointed by 'task and remove if it exists.
*/
void removePeriod(Task *task, list<Period *> &periods) {
    int id = stoi(getToken(1)) - 1;

    Period *period = ithPeriod(periods, id);
    if (period == nullptr) return;

    period->todo->periods.remove(period);
    delete period;

    printf("Period removed.\n\n");
}

/*
    Ask user for a period an amount of minutes and remove them from the last period.
*/
void reducePeriod(Task *task, list<Period *> &periods) {
    long int minutes = stol(getToken(1));

    Period *period = periods.back();

    if (periods.empty()) {
        cout << "Não há períodos\n\n";
        return;
    }

    if (period->end - period->start < 60 * minutes) {
        cout << "Período possui menos minutos restantes\n\n";
        return;
    }

    period->end -= 60 * minutes;

    cout << minutes << " minutos removidos do último período\n\n";
}

/*
    Lists task periods.
*/
void listPeriods(Task *task, list<Period *> &periods, bool showAll) {
    int i = 1;
    string formatedStartTime;
    string formatedEndTime;
    string formatedDur;
    string periodName;

    periods.clear();
    getPeriodsFromTodo(periods, task->rootTodo);
    periods.sort(periodComp);

    if (!showAll) {
        while (periods.size() > 10) {
            periods.pop_front();
        }
    }

    if (periods.size() == 0) {
        printf("    Não há períodos.\n");
    }
    for (auto it = periods.begin(); it != periods.end(); it++) {
        Period *period = *it;
        formatedStartTime = formatTime(period->start);
        formatedEndTime = formatTime(period->end);
        formatedDur = formatDur(period->end - period->start);
        periodName = getTodoPath(period->todo);
        cout << "    " << getColor("CYAN") << setw(3) << i << "." << getColor("BRIGHT_WHITE");
        cout << " [" << formatedStartTime << " ... " << formatedEndTime << "]   " << formatedDur << "   \"" << periodName << "\"\n";
        i++;
    }
    cout << "\n";
    printLine(MAIN_LEVEL);
}

/*
    Enters periods menu.
*/
void periodsMenu(Task *task) {
    string commandName;
    list<Period *> periods;
    printTitle("Períodos - " + task->code, MAIN_LEVEL);
    listPeriods(task, periods, false);
        
    while (true) {
        periodWarning();
        commandName = getCommandName();
        printTitle("Períodos - " + task->code, MAIN_LEVEL);

        if (commandName == "pds") {
            if (getNComms() == 1) {
                listPeriods(task, periods, false);
            } else if (getNComms() == 2) {
                if (getToken(1) == "all") {
                    listPeriods(task, periods, true);
                } else {
                    printf("Argumento inválido.\n\n");
                }
            } else {
                printf("Número inválido de argumentos.\n");
            }
        } else if (commandName == "edit") {
            if (validArgs(4)) {
                editPeriod(task, periods);
                listPeriods(task, periods, false);
                saveAll();
            }
        } else if (commandName == "rem") {
            if (validArgs(1)) {
                removePeriod(task, periods);
                listPeriods(task, periods, false);
                saveAll();
            }
        } else if (commandName == "reduce") {
            if (validArgs(1)) {
                reducePeriod(task, periods);
                listPeriods(task, periods, false);
                saveAll();
            }
        
        } else if (commandName == "cd") {
            if (validArgs(1)) {
                if (getToken(1) == "..") {
                    curMenu = TASK_MENU;
                    return;
                } else {
                    printf("Digite 'cd ..' para voltar\n\n");
                }
            }
        } else if (generalCommands(commandName)) return;
    }
}
