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
    editPeriod()
    Edit period from task pointed by 'task' according to user input.
*/
void editPeriod(Task* task) {
    int periodId;
    char *ext;
    char *type;
    long int old;
    long int *period;
    int a, b, c;
    string oldFormatedTime;
    string newFormatedTime;
    sscanf(getToken(1), "%d", &periodId);
    ext = toLowercase(getToken(2));
    type = toLowercase(getToken(3));

    periodId--;
    if (periodId < 0 || periodId >= task->nPeriods) {
        printf("Invalid period id.\n\n");
        return;
    }
    if (strcmp(ext, "start") == 0) {
        period = &((task->periods[periodId]).start);
    } else if (strcmp(ext, "end") == 0) {
        period = &((task->periods[periodId]).end);
    } else {
        printf("You can edit period 'start' or 'end'\n\n");
        return;
    }
    old = *period;
    if (strcmp(type, "time") == 0) {
        sscanf(getToken(4), "%d:%d:%d", &a, &b, &c);
        *period = changeTime(*period, a, b, c);
    } else if (strcmp(type, "date") == 0) {
        sscanf(getToken(4), "%d/%d/%d", &a, &b, &c);
        *period = changeDate(*period, a, b, 2000 + c);
    } else {
        printf("You can edit period 'time' or 'date'\n\n");
        return;
    }
    newFormatedTime = formatTime(*period);
    oldFormatedTime = formatTime(old);
    printf("Period %s %s changed from %s to %s\n\n", ext, type, oldFormatedTime.c_str(), newFormatedTime.c_str());
}

/*
    addPeriod()
    Add period in task pointed by 'task' according to user input.
*/
void addPeriod(Task* task) {
    int day, month, year, hour, min, sec;
    sscanf(getToken(1), "%d/%d/%d", &day, &month, &year);
    sscanf(getToken(2), "%d:%d:%d", &hour, &min, &sec);
    task->periods[task->nPeriods].start = getTime(day, month, year + 2000, hour, min, sec);
    sscanf(getToken(3), "%d/%d/%d", &day, &month, &year);
    sscanf(getToken(4), "%d:%d:%d", &hour, &min, &sec);
    task->periods[task->nPeriods].end = getTime(day, month, year + 2000, hour, min, sec);
    (task->nPeriods)++;
    printf("Period added.\n\n");
}

/*
    removePeriod()
    Ask user for a period Id of task pointed by 'task and remove if it exists.
*/
void removePeriod(Task *task) {
    int periodId;
    int i;
    sscanf(getToken(1), "%d", &periodId);
    periodId--;
    if (periodId < 0 || periodId >= task->nPeriods) {
        printf("Invalid period id.\n\n");
        return;
    }
    (task->nPeriods)--;
    for (i = periodId; i < task->nPeriods; i++) {
        task->periods[i].start = task->periods[i + 1].start;
        task->periods[i].end = task->periods[i + 1].end;
    }
    printf("Period removed.\n\n");
}

/*
    listPeriods()
    Lists task periods.
*/
void listPeriods(Task *task) {
    int i;
    string formatedStartTime;
    string formatedEndTime;
    string formatedDur;
    printf("  +--------------------------> Period list <--------------------------+\n");
    printf("  |\n");
    if (task->nPeriods == 0) {
        printf("  |   List is empty\n");
    }
    for (i = 0; i < task->nPeriods; i++) {
        formatedStartTime = formatTime(task->periods[i].start);
        formatedEndTime = formatTime(task->periods[i].end);
        formatedDur = formatDur(task->periods[i].end - task->periods[i].start);
        printf("  |   %2d. [%s  ...  %s]   %s   \"%s\"\n", i + 1, formatedStartTime.c_str(), formatedEndTime.c_str(), formatedDur.c_str(), task->periods[i].name.c_str());
    }
    printf("  |\n");
    printf("  +-------------------------------------------------------------------+\n\n");

}

/*
    periodsMenu()
    Enters periods menu.
*/
void periodsMenu(Task *task) {
    char *commandName;
    printf(" _________________________  Period Menu (%s)  _________________________\n\n", task->code.c_str());
    listPeriods(task);
    while (true) {
        printf(" _________________________  Period Menu (%s)  _________________________\n\n", task->code.c_str());
        periodWarning();
        commandName = getCommandName();
        if (strcmp(commandName, "pds") == 0) {
            if (validArgs(0)) listPeriods(task);
        } else if (strcmp(commandName, "set") == 0) {
            if (validArgs(4)) {
                editPeriod(task);
                listPeriods(task);
                saveAll();
            }
        } else if (strcmp(commandName, "add") == 0) {
           if (validArgs(4)) {
                addPeriod(task);
                listPeriods(task);
                saveAll();
            }
        } else if (strcmp(commandName, "rem") == 0) {
            if (validArgs(1)) {
                removePeriod(task);
                listPeriods(task);
                saveAll();
            }
        } else if (strcmp(commandName, "cd") == 0) {
            if (validArgs(1)) {
                if (strcmp("..", getToken(1)) == 0) {
                    curMenu = TASK_MENU;
                    return;
                } else {
                    printf("Type 'cd ..' to go back\n\n");
                }
            }
        } else if (generalCommands(commandName)) return;
    }
}
