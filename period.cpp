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
    char *ext;
    char *type;
    long int old;
    long int *timestamp;
    int a, b, c;
    string oldFormatedTime;
    string newFormatedTime;
    int id = atoi(getToken(1)) - 1;
    ext = toLowercase(getToken(2));
    type = toLowercase(getToken(3));

    Period *period = ithPeriod(task->periods, id);
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
    addPeriod()
    Add period in task pointed by 'task' according to user input.
*/
void addPeriod(Task* task) {
    int day, month, year, hour, min, sec;
    Period *period = new Period;

    sscanf(getToken(1), "%d/%d/%d", &day, &month, &year);
    sscanf(getToken(2), "%d:%d:%d", &hour, &min, &sec);
    period->start = getTime(day, month, year + 2000, hour, min, sec);
    sscanf(getToken(3), "%d/%d/%d", &day, &month, &year);
    sscanf(getToken(4), "%d:%d:%d", &hour, &min, &sec);
    period->end = getTime(day, month, year + 2000, hour, min, sec);
    task->periods.push_back(period);
    printf("Period added.\n\n");
}

/*
    removePeriod()
    Ask user for a period Id of task pointed by 'task and remove if it exists.
*/
void removePeriod(Task *task) {
    int id = atoi(getToken(1)) - 1;
    Period *period = ithPeriod(task->periods, id);

    if (period == nullptr) return;

    task->periods.remove(period);
    delete period;

    printf("Period removed.\n\n");
}

/*
    listPeriods()
    Lists task periods.
*/
void listPeriods(Task *task) {
    int i = 1;
    string formatedStartTime;
    string formatedEndTime;
    string formatedDur;
    printf("  +--------------------------> Period list <--------------------------+\n");
    printf("  |\n");
    if (task->periods.size() == 0) {
        printf("  |   List is empty\n");
    }
    for (auto it = task->periods.begin(); it != task->periods.end(); it++) {
        Period *period = *it;
        formatedStartTime = formatTime(period->start);
        formatedEndTime = formatTime(period->end);
        formatedDur = formatDur(period->end - period->start);
        printf("  |   %2d. [%s  ...  %s]   %s   \"%s\"\n", i + 1, formatedStartTime.c_str(), formatedEndTime.c_str(), formatedDur.c_str(), period->name.c_str());
        i++;
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
