#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "objectives.hpp"
#include "util.hpp"
#include "task.hpp"
#include "taskUtil.hpp"
#include "io.hpp"
#include "calendar.hpp"
#include "calendarUtil.hpp"
#include "help.hpp"

using namespace std;

Task* rootTask;
Calendar *calendar;
int lastMenu;
int curMenu;
Task* curTask;

void freeAll() {
	freeTask(rootTask);
    delete calendar;
}

bool generalCommands(string commandName) {
    if (commandName == "goto") {
        if (validArgs(1)) {
            Task *nextTask = searchTask();
            if (nextTask != nullptr) {
                curTask = nextTask;
                curMenu = TASK_MENU;
                return true;
            }
        }
    } else if (commandName == "cal") {
        if (validArgs(0)) {
            lastMenu = curMenu;
            curMenu = CALENDAR_MENU;
            return true;
        }
    } else if (commandName == "week") {
        if (getNComms() == 1) printWeekSummary(false);
        else if (getNComms() == 2 && getToken(1) == "all") printWeekSummary(true);
        else cout << "Opção inválida\n\n";
    } else if (commandName == "tree") {
        if (getNComms() == 1 || getNComms() == 2) {
            printTaskTree();
        } else {
            printf("Número inválido de argumentos.\n\n");
        }
    } else if (commandName == "help") {
        if (validArgs(0)) printHelp(curMenu);
    } else if (commandName == "save") {
        if (validArgs(0)) saveAll();
    } else if (commandName == "exit") {
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

int main(int argc, char **argv) {
    calendar = createCalendar();
    printTitle("Objetivos", PROGRAM_LEVEL);
    printf("Digite 'ajuda' para uma lista de comandos.\n\n");
    
    rootTask = createTask("Objetivos", "OBJS");

    system("mkdir -p data");
    system("mkdir -p plans");

    // if (argc > 1) loadAllOld();
    // else loadAll();

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
