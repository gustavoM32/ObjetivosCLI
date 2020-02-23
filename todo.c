#include <string.h>
#include "todo.h"
#include "io.h"
#include "util.h"
#include "help.h"

/*
    createTodo()
    Creates a to-do named 'name' and planned to be done in 'planned' and returns
    a pointer to it.
*/
Todo* createTodo(Task *task, char *name, time_t planned) {
    Todo* newTodo;
    newTodo = (Todo *) mallocSafe(sizeof(Todo));
    newTodo->planned = planned;
    strcpy(newTodo->name, name);
    newTodo->parent = task;
    return newTodo;
}

/*
    addTodo()
    Ask user for a to-do name and add it in task pointed by 'task' to-dos.
*/
void addTodo(Task *task) {
    char *name;
    name = getToken(1);
    if (name[0] == '\0') {
        printf("To-do name must not be empty.\n\n");
        return;
    }
    task->todos[task->nTodos++] = createTodo(task, name, 0);
    printf("Added to-do \"%s\"\n\n", name);
}

/*
    addTodoEsp()
    Ask user the interval [a ... b] and name prefix to create to-dos
    [prefix a ... prefix b] in task pointed by 'task'.
*/
void addTodoEsp(Task *task) {
    int start;
    int end;
    char *name;
    char copyName[NAME_LEN + 5];
    int i;
    sscanf(getToken(1), "%d-%d", &start, &end);
    name = getToken(2);
    if (strlen(name) == 0) {
        printf("To-do name must not be empty.\n\n");
        return;
    }
    for (i = start; i <= end; i++) {
        sprintf(copyName, "%s %d", name, i);
        task->todos[task->nTodos++] = createTodo(task, copyName, 0);
    }
    printf("Added to-dos '%s #', # = [%d, %d]\n\n", name, start, end);
}

/*
    removeTodo()
    Ask user for to-do ID and remove it from task pointed by 'task'.
*/
void removeTodo(Task* task) {
    int id;
    sscanf(getToken(1), "%d", &id);
    id--;
    if (id < 0 || task->nTodos <= id) {
        printf("Invalid to-do ID.\n\n");
        return;
    }
    task->nTodos--;
    printf("To-do \"%s\" removed.\n\n", task->todos[id]->name);
    while (id < task->nTodos) {
        task->todos[id] = task->todos[id + 1];
        id++;
    }
}

/*
    changeTodoDate()
    Changes planned day of to-do of task pointed by 'task'.
*/
void changeTodoDate(Task *task) {
    int id;
    int day, mon;
    time_t oldTime;
    struct tm *structTime;
    sscanf(getToken(1), "%d", &id);
    sscanf(getToken(2), "%d/%d", &day, &mon);
    id--;
    if (id < 0 || task->nTodos <= id) {
        printf("Invalid to-do ID.\n\n");
        return;
    }
    if (day == 0 && mon == 0) {
        task->todos[id]->planned = 0;
        printf("To-do date removed.\n\n");
        return;
    }
    oldTime = getCurrentTime();
    structTime = localtime(&oldTime);
    structTime->tm_hour = 12;
    structTime->tm_min = 0;
    structTime->tm_sec = 0;
    structTime->tm_mon = mon - 1;
    structTime->tm_mday = day;
    if (mktime(structTime) < oldTime - oldTime % 86400) structTime->tm_year++;
    task->todos[id]->planned = mktime(structTime);
    printf("To-do date changed to %02d/%02d/%04d.\n\n", day, mon, 1900 + structTime->tm_year);
}

/*
    changeTodoDateEsp()
    Changes planned day of to-dos with ID [a ... b] of task
    pointed by 'task'.
*/
void changeTodoDateEsp(Task *task) {
    int ids, ide;
    int day, mon;
    time_t oldTime;
    struct tm *structTime;
    time_t newTime;
    int i;
    sscanf(getToken(1), "%d-%d", &ids, &ide);
    sscanf(getToken(2), "%d/%d", &day, &mon);
    ids--;
    ide--;
    if (ids < 0 || ide < ids || task->nTodos <= ide) {
        printf("Invalid to-do ID interval.\n\n");
        return;
    }
    if (day == 0 && mon == 0) {
        newTime = 0;
        printf("To-do date removed.\n\n");
    } else {
        oldTime = getCurrentTime();
        structTime = localtime(&oldTime);
        structTime->tm_hour = 12;
        structTime->tm_mon = mon - 1;
        structTime->tm_mday = day;
   	    structTime->tm_min = 0;
    	structTime->tm_sec = 0;
	    if (mktime(structTime) < oldTime - oldTime % 86400) structTime->tm_year++;
        newTime = mktime(structTime);
 	    printf("To-dos dates changed to %02d/%02d/%04d.\n\n", day, mon, 1900 + structTime->tm_year);

    }
    for (i = ids; i <= ide; i++) {
        task->todos[i]->planned = newTime;
    }
}

/*
    startPeriod()
    Starts a period in task pointed by 'task'.
*/
void startPeriod(Task* task) {
    Period* period = &(task->periods[task->nPeriods]);
    int id;
    sscanf(getToken(1), "%d", &id);
    id--;
    period->start = period->end = getCurrentTime();
    strcpy(period->name, task->todos[id]->name);
    (task->nPeriods)++;
    printf("Period \"%s\" of (%s) started. Type 'stop' when done.\n\n", period->name, task->code);
}

/*
    savePeriodTime()
    Saves current time as end time of running period of task pointed by
    'task'.
*/
void savePeriodTime(Task* task) {
    Period* period = &(task->periods[task->nPeriods - 1]);
    period->end = getCurrentTime();
}

/*
    stopPeriod()
    Stops running period of task pointed by 'task' and saves it.
*/
void stopPeriod(Task* task) {
    Period* period = &(task->periods[task->nPeriods - 1]);
    long int taskDur = 0;
    char formatedDur[10];
    savePeriodTime(task);
    taskDur = period->end - period->start;
    formatDur(taskDur, formatedDur);
    printf("Period \"%s\" of (%s) stopped. Duration: %s.\n\n", period->name, task->code, formatedDur);
}

/*
    cancelPeriod()
    Cancels running period of task pointed by 'task', not saving it.
*/
void cancelPeriod(Task* task) {
    Period* period = &(task->periods[task->nPeriods - 1]);
    long int taskDur = 0;
    char formatedDur[10];
    taskDur = getCurrentTime() - period->start;
    (task->nPeriods)--;
    formatDur(taskDur, formatedDur);
    printf("Period \"%s\" of (%s) stopped. Duration: %s.\n\n", period->name, task->code, formatedDur);

}

/*
    showTaskPeriodTime()
    Prints total time in running period of task pointed by 'task'.
*/
void showTaskPeriodTime(Task* task) {
    Period* period = &(task->periods[task->nPeriods - 1]);
    long int taskDur = 0;
    char formatedDur[10];
    taskDur = getCurrentTime() - period->start;
    formatDur(taskDur, formatedDur);
    printf("Time spent in period \"%s\" of (%s): %s.\n\n", period->name, task->code, formatedDur);
}

/*
    listTodos()
    Print list of to-dos of task pointed by 'task'.
*/
void listTodos(Task* task) {
    int i;
    struct tm *stm;
    printf("  +--------------------------> To-do list <--------------------------+\n");
    printf("  |\n");
    if (task->nTodos == 0) {
        printf("  |   List is empty\n");
    }
    for (i = 0; i < task->nTodos; i++) {

        if (task->todos[i]->planned != 0) {
            stm = localtime(&(task->todos[i]->planned));
            printf("  |   %2d. %s (%02d/%02d)\n", i + 1, task->todos[i]->name, stm->tm_mday, stm->tm_mon + 1);
        } else {
            printf("  |   %2d. %s\n", i + 1, task->todos[i]->name);
        }
    }
    printf("  |\n");
    printf("  +------------------------------------------------------------------+\n\n");
}

/*
    todosMenu()
    Enters in to-do menu of task pointed by 'task'.
*/
void todosMenu(Task* task) {
    char *commandName;
    bool running = false;
    printf(" _________________________  To-dos Menu (%s)  _________________________\n\n", task->code);
    listTodos(task);
    while (true) {
        printf(" _________________________  To-dos Menu (%s)  _________________________\n\n", task->code);
        commandName = getCommandName();
        if (strcmp(commandName, "add") == 0) {
            if (validArgs(1)) {
                addTodo(task);
				listTodos(task);
                saveAll();
            }
        } else if (strcmp(commandName, "addesp") == 0) {
            if (validArgs(2)) {
                addTodoEsp(task);
				listTodos(task);
                saveAll();
            }
        } else if (strcmp(commandName, "rem") == 0) {
            if (validArgs(1)) {
                removeTodo(task);
                saveAll();
            }
        } else if (strcmp(commandName, "tds") == 0) {
            if (validArgs(0)) listTodos(task);
        } else if (strcmp(commandName, "set") == 0) {
            if (validArgs(2)) {
                changeTodoDate(task);
				listTodos(task);
                saveAll();
            }
        } else if (strcmp(commandName, "setesp") == 0) {
            if (validArgs(2)) {
                changeTodoDateEsp(task);
				listTodos(task);
                saveAll();
            }
        } else if (strcmp(commandName, "start") == 0) {
            if (running) {
                printf("There is already a period running.\n\n");
            } else if (validArgs(1)) {
                running = true;
                startPeriod(task);
                saveAll();
            }
        } else if (strcmp(commandName, "stop") == 0) {
            if (!running) {
                printf("There is no period running to stop.\n\n");
            } else if (validArgs(0)) {
                running = false;
                stopPeriod(task);
                saveAll();
            }
        } else if (strcmp(commandName, "cancel") == 0) {
            if (!running) {
                printf("There is no period running to cancel.\n\n");
            } else if (validArgs(0)) {
                running = false;
                cancelPeriod(task);
                saveAll();
            }
        } else if (strcmp(commandName, "time") == 0) {
            if (!running) {
                printf("There is no period running.\n\n");
            } else if (validArgs(0)) {
                showTaskPeriodTime(task);
                savePeriodTime(task);
                saveAll();
            }
        } else if (strcmp(commandName, "help") == 0) {
            if (validArgs(0)) printHelp(TODOS_MENU);
        } else if (strcmp(commandName, "cd") == 0) {
            if (running) {
                printf("There a period running, can't go back.\n\n");
            } else if (validArgs(1)) {
                if (strcmp("..", getToken(1)) == 0) {
                    return;
                } else {
                    printf("Type 'cd ..' to go back\n\n");
                }
            }
        } else {
            notAvailable(commandName);
        }
    }
}
