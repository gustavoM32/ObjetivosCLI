#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "objectives.hpp"
#include "util.hpp"
#include "task.hpp"
#include "io.hpp"
#include "calendar.hpp"
#include "help.hpp"

Task* rootTask;
Calendar *calendar;
int lastMenu;
int curMenu;
Task* curTask;

void freeAll() {
	freeTask(rootTask);
    free(calendar);
}

bool generalCommands(char *commandName) {
    if (strcmp(commandName, "goto") == 0) {
        if (getNComms() > 1) {
            Task *nextTask = searchTask(rootTask);
            if (nextTask != NULL) {
                curTask = nextTask;
                curMenu = TASK_MENU;
                return true;
            }
        } else {
            printf("Usage: goto [...] [<parent code>] <task code>\n\n");
        }
    } else if (strcmp(commandName, "cal") == 0) {
        if (validArgs(0)) {
            lastMenu = curMenu;
            curMenu = CALENDAR_MENU;
            return true;
        }
    } else if (strcmp(commandName, "week") == 0) {
        if (validArgs(0)) printWeekSummary(rootTask);
    } else if (strcmp(commandName, "help") == 0) {
        if (validArgs(0)) printHelp(curMenu);
    } else if (strcmp(commandName, "save") == 0) {
        if (validArgs(0)) saveAll();
    } else if (strcmp(commandName, "exit") == 0) {
        if (validArgs(0)) {
            freeAll();
            printf("Exiting...\n\n");
            exit(EXIT_SUCCESS);
        }
    } else {
        notAvailable(commandName);
    }
    return false;
};

int main() {
    calendar = createCalendar();
    printf("*** Welcome to Control! ***\n\n");
    printf("Type 'help' for a list of commands.\n\n");
    
    rootTask = createTask("Objetivos", "OBJ");
    loadAll();
    curMenu = TASK_MENU;
    curTask = rootTask;
    while (curMenu != -1) {
        switch (curMenu) {
            case TASK_MENU:
                taskMenu(curTask);
                break;
            case SUBTASKS_MENU:
                subtasksMenu(curTask);
                break;
            case TODOS_MENU:
                todosMenu(curTask);
                break;
            case PERIODS_MENU:
                periodsMenu(curTask);
                break;
            case CALENDAR_MENU:
                calendarMenu();
                break;
            default:
                curMenu = -1;
        }
    }
	freeAll();
    return EXIT_SUCCESS;
}