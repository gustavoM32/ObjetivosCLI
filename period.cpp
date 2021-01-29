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
    char *ext;
    char *type;
    long int old;
    long int *timestamp;
    int a, b, c;
    string oldFormatedTime;
    string newFormatedTime;
    ext = toLowercase(getToken(1));
    type = toLowercase(getToken(2));
    int id = atoi(getToken(3)) - 1;

    Period *period = ithPeriod(periods, id);
    if (period == nullptr) return;

    if (strcmp(ext, "start") == 0) {
        timestamp = &(period->start);
    } else if (strcmp(ext, "end") == 0) {
        timestamp = &(period->end);
    } else {
        printf("You can edit period 'start' or 'end'\n\n");
        return;
    }

    old = *timestamp;
    if (strcmp(type, "time") == 0) {
        sscanf(getToken(4), "%d:%d:%d", &a, &b, &c);
        *timestamp = changeTime(*timestamp, a, b, c);
    } else if (strcmp(type, "date") == 0) {
        sscanf(getToken(4), "%d/%d/%d", &a, &b, &c);
        *timestamp = changeDate(*timestamp, a, b, 2000 + c);
    } else {
        printf("You can edit period 'time' or 'date'\n\n");
        return;
    }
    newFormatedTime = formatTime(*timestamp);
    oldFormatedTime = formatTime(old);
    printf("Period %s %s changed from %s to %s\n\n", ext, type, oldFormatedTime.c_str(), newFormatedTime.c_str());
}

/*
    Ask user for a period Id of task pointed by 'task and remove if it exists.
*/
void removePeriod(Task *task, list<Period *> &periods) {
    int id = atoi(getToken(1)) - 1;

    Period *period = ithPeriod(periods, id);
    if (period == nullptr) return;

    period->todo->periods.remove(period);
    delete period;

    printf("Period removed.\n\n");
}

/*
    Lists task periods.
*/
void listPeriods(Task *task, list<Period *> &periods) {
    int i = 1;
    string formatedStartTime;
    string formatedEndTime;
    string formatedDur;
    string periodName;

    periods.clear();
    getPeriodsFromTodo(periods, task->rootTodo);
    periods.sort(periodComp);

    if (periods.size() == 0) {
        printf("    Não há períodos.\n");
    }
    for (auto it = periods.begin(); it != periods.end(); it++) {
        Period *period = *it;
        formatedStartTime = formatTime(period->start);
        formatedEndTime = formatTime(period->end);
        formatedDur = formatDur(period->end - period->start);
        periodName = getTodoPath(period->todo);
        cout << "    " << getColor(CYAN) << setw(3) << i << "." << getColor(BRIGHT_WHITE);
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
    char *commandName;
    list<Period *> periods;
    printTitle("Períodos - " + task->code, MAIN_LEVEL);
    listPeriods(task, periods);
        
    while (true) {
        periodWarning();
        commandName = getCommandName();
        printTitle("Períodos - " + task->code, MAIN_LEVEL);

        if (strcmp(commandName, "pds") == 0) {
            if (validArgs(0)) listPeriods(task, periods);
        } else if (strcmp(commandName, "edit") == 0) {
            if (validArgs(4)) {
                editPeriod(task, periods);
                listPeriods(task, periods);
                saveAll();
            }
        } else if (strcmp(commandName, "rem") == 0) {
            if (validArgs(1)) {
                removePeriod(task, periods);
                listPeriods(task, periods);
                saveAll();
            }
        } else if (strcmp(commandName, "cd") == 0) {
            if (validArgs(1)) {
                if (strcmp("..", getToken(1)) == 0) {
                    curMenu = TASK_MENU;
                    return;
                } else {
                    printf("Digite 'cd ..' para voltar\n\n");
                }
            }
        } else if (generalCommands(commandName)) return;
    }
}
