#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "calendar.h"
#include "objectives.h"
#include "io.h"
#include "util.h"
#include "help.h"
#include "todo.h"

char *wDayName[] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};
char *wDayShort[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"};

/*
    updateCalendarSchedules()
    Fills calendar with to-do schedules.
*/
void updateCalendarSchedules(Calendar *calendar, Todo *todo) {
    int i;
    for (i = 0; i < todo->nSchedules; i++) {
        calendar->schedules[calendar->nSchedules] = todo->schedules[i];
        calendar->nSchedules++;
    }
}

/*
    updateCalendarTodo()
    Fills calendar with sub-to-dos of todo.
*/
void updateCalendarTodo(Calendar *calendar, Todo *todo) {
    int i;
    for (i = 0; i < todo->nSubtodos; i++) {
        if (todo->subtodos[i]->status == TODO_PRIORITY) {
            calendar->todos[calendar->nTodos] = todo->subtodos[i];
            calendar->nTodos++;
        }
        updateCalendarSchedules(calendar, todo->subtodos[i]);
        updateCalendarTodo(calendar, todo->subtodos[i]);
    }
}

/*
    updateCalendarTask()
    Fills calendar with to-dos of task.
*/
void updateCalendarTask(Calendar *calendar, Task *task) {
    int i;
    for (i = 0; i < task->nTodos; i++) {
        if (task->todos[i]->status == TODO_PRIORITY) {
            calendar->todos[calendar->nTodos] = task->todos[i];
            calendar->nTodos++;
        }
        updateCalendarSchedules(calendar, task->todos[i]);
        updateCalendarTodo(calendar, task->todos[i]);
    }
    for (i = 0; i < task->nSubtasks; i++) {
        updateCalendarTask(calendar, task->subtasks[i]);
    }
}

/*
    schedComp
    Schedule compare function.
*/
int schedComp(const void *elem1, const void *elem2) {
    Schedule *sched1 = *(Schedule **) elem1;
    Schedule *sched2 = *(Schedule **) elem2;
    if (sched1->date < sched2->date) return -1;
    if (sched1->date > sched2->date) return 1;
    if (sched1->timeSet) return 1;
    if (sched2->timeSet) return -1;
    return 0;
}

/*
    sortCalendar()
    Order schedules by date.
*/
void sortCalendar() {
    qsort(calendar->schedules, calendar->nSchedules, sizeof(Schedule *), schedComp);
}

void getTodoFullName(Todo *todo, char name[]) {
    char nameSufix[NAME_LEN + CODE_LEN];

    if (todo->type == ROOT) {
        sprintf(name, "%s", todo->parent.task->uniquePath);
    } else {
        getTodoFullName(todo->parent.todo, name);
    }

    sprintf(nameSufix, " > %s", todo->name);
    strcat(name, nameSufix);
}


/*
    printScheduled()
    Prints scheduled to-dos.
*/
void printScheduled() {
    int i;    
    time_t curDayStart = getDayStart(getCurrentTime());
    int totalSpent = 0, totalEstimated = 0;

    sortCalendar();

    printf(" ___________________  Scheduled  ___________________\n");

    if (calendar->nSchedules == 0) {
        printf("\n    No scheduled to-dos.\n");
    } else {
        Schedule *sched = calendar->schedules[calendar->nSchedules - 1];
        struct tm date, schedDate;
        localtime_r(&(sched->date), &date);
        time_t dayStart = getDayStart(sched->date) + SECS_IN_A_DAY;

        i = calendar->nSchedules - 1;
        do {
            sched = calendar->schedules[i];
            localtime_r(&(sched->date), &schedDate);
            if (sched->date < curDayStart) {
                if (totalEstimated != 0) {
                    printf("        Total (%04.1f/%04.1f)\n", totalSpent / 60.0, totalEstimated / 60.0);
                    totalSpent = totalEstimated = 0;
                }
                printf("\n ________________  Late  _________________\n\n");
            } else {
                while (sched->date < dayStart) {
                    dayStart -= SECS_IN_A_DAY;
                    localtime_r(&dayStart, &date);
                    if (totalEstimated != 0) {
                        printf("        Total (%04.1f/%04.1f)\n", totalSpent / 60.0, totalEstimated / 60.0);
                        totalSpent = totalEstimated = 0;
                    }
                    printf("\n _____________  %s (%02d/%02d)  _____________\n\n", wDayShort[date.tm_wday], date.tm_mday, date.tm_mon + 1);
                }
            }

            printf("    %2d: ", calendar->nSchedules - i);

            if (sched->timeSet) {
                printf("%02d:%02d ", schedDate.tm_hour, schedDate.tm_min);
            } else {
                printf("--:-- ");
            }

            char todoName[NAME_LEN];
            getTodoFullName(sched->todo, todoName);
            printf("(%04.1f/%04.1f) %s\n", sched->timeSpent / 60.0, sched->timeEstimate / 60.0, todoName);
            totalSpent += sched->timeSpent;
            totalEstimated += sched->timeEstimate;

            i--;
        } while (i >= 0);
    }
    if (totalEstimated != 0) {
        printf("        Total (%04.1f/%04.1f)\n", totalSpent / 60.0, totalEstimated / 60.0);
    }
    printf(" ___________________________________________________\n\n");
}

/*
    printPrioritized()
    Prints prioritized to-dos.
*/
void printPrioritized() {
    int i;    

    printf(" __________________  Prioritized  __________________\n\n");

    for (i = 0; i < calendar->nTodos; i++) {
        Todo *todo = calendar->todos[i];
        char todoName[NAME_LEN];
        getTodoFullName(todo, todoName);
        printf("    %2d: (%04.1f/%04.1f) %s\n", i + 1, todo->timeSpent / 60.0, todo->timeEstimate / 60.0, todoName);
    }

    printf(" ___________________________________________________\n\n");
}

void printCalendar() {
    printScheduled();
    printPrioritized();
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
int periodComp(const void *elem1, const void *elem2) {
    Period *p1 = *(Period **) elem1;
    Period *p2 = *(Period **) elem2;
    if (p1->start < p2->start) return -1;
    if (p1->start > p2->start) return 1;
    return 0;
}

/*
    sortPeriods()
    Order periods by time.
*/
void sortPeriods(Period *periods[], int n) {
    qsort(periods, n, sizeof(Period *), periodComp);
}

/*
    getPeriods()
    Inserts all TASK periods into PERIODS and the vector size into N.
*/
void getPeriods(Period *periods[], int *n, Task *task) {
    int i;
    for (i = 0; i < task->nPeriods; i++) {
        periods[(*n)++] = &(task->periods[i]);
    }
    for (i = 0; i < task->nSubtasks; i++) {
        getPeriods(periods, n, task->subtasks[i]);
    }
}

/*
    printWeekSummary()
    Prints all to-dos with no specified dates.
*/
void printWeekSummary(Task *root) {
    int n = 0;
    int i, p;
    long int curWeek[2];
    long int curTime;
    long int weekProgress;
    long int charInterval = 900;
    long int objStart;
    long int weekTime = 0;
    long int dayTime = 0;
    char formatText[NAME_LEN];
    Period *periodList[MAX_PERIODS*MAX_PERIODS];
    getPeriods(periodList, &n, root);
    sortPeriods(periodList, n);
    curTime = getCurrentTime();
    objStart = getTime(25, 8, 2019, 0, 0, 0);
    weekProgress = (curTime - objStart) % SECS_IN_A_WEEK;

    curWeek[0] = curTime - weekProgress;
    curWeek[1] = curWeek[0] + SECS_IN_A_WEEK;

    for (i = 0, p = 0; objStart + i < curWeek[1]; i += charInterval) {
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
            formatDate(charStart, formatText);
            dayTime = 0;
            printf("  %s %s [", wDayShort[(i / 86400) % 7], formatText);
        }
        if (charStart > curTime) printf(" ");
        else {
            while (p < n && periodList[p]->end <= charStart) p++;
            if (p == n || periodList[p]->start >= charEnd) {
                printf(".");
            } else {
                while (p < n && periodList[p]->start < charEnd) {
                    prodTime += min(periodList[p]->end, charEnd) - max(periodList[p]->start, charStart);
                    p++;
                }
                p--;
                dayTime += prodTime;
                if ((double) prodTime / charInterval > 0.5) printf("#");
                else printf(".");
            }
        }
        if ((i + charInterval) % 86400 == 0) {
            weekTime += dayTime;
            formatDur(dayTime, formatText);
            printf("] %s\n", formatText);
        }
        
        if ((i + charInterval) % SECS_IN_A_WEEK == 0) {
            formatDur(weekTime, formatText);
            printf("\n  Total time: %s\n", formatText);
        }
    }
    printf("\n");
}

/*
    updateCalendar()
    Updates the calendar.
*/
void updateCalendar() {
    calendar->nTodos = 0;
    calendar->nSchedules = 0;
    updateCalendarTask(calendar, rootTask);
    setUPath(rootTask, rootTask);
}

/*
    createCalendar()
    Creates the calendar.
*/
Calendar *createCalendar() {
    Calendar* calendar;
    calendar = (Calendar *) mallocSafe(sizeof(Calendar));
    calendar->nTodos = 0;
    calendar->nSchedules = 0;
    calendar->periodSched = NULL;
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
    char todoName[NAME_LEN];

    if (calendar->periodSched != NULL) {
        printf("There is already a period running.\n\n");
        return;
    }

    id = calendar->nSchedules - atoi(getToken(1));
    if (id < 0 || id >= calendar->nSchedules) {
        printf("Invalid to-do ID.\n\n");
        return;
    }
    
    calendar->periodSched = calendar->schedules[id];
    todo = calendar->periodSched->todo;
    getTodoFullName(todo, todoName);

    while (todo->type != ROOT) todo = todo->parent.todo;
    task = todo->parent.task;
    period = &(task->periods[task->nPeriods]);

    period->start = period->end = getCurrentTime();
    strcpy(period->name, todoName);
    (task->nPeriods)++;
    printf("Period \"%s\" started. Type 'stop' when done.\n\n", period->name);
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
    char formatedDur[10];

    if (calendar->periodSched == NULL) {
        printf("There is no period running.\n\n");
        return;
    }

    sched = calendar->periodSched;
    todo = sched->todo;
    while (todo->type != ROOT) todo = todo->parent.todo;
    task = todo->parent.task;
    period = &(task->periods[task->nPeriods - 1]);

    period->end = getCurrentTime();
    taskDur = period->end - period->start;
    sched->timeSpent += taskDur / 60;
    sched->todo->timeSpent += taskDur / 60;
    formatDur(taskDur, formatedDur);
    printf("Period \"%s\" stopped. Duration: %s.\n\n", period->name, formatedDur);
    calendar->periodSched = NULL;
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
    char formatedDur[10];

    if (calendar->periodSched == NULL) {
        printf("There is no period running.\n\n");
        return;
    }

    todo = calendar->periodSched->todo;
    while (todo->type != ROOT) todo = todo->parent.todo;
    task = todo->parent.task;
    period = &(task->periods[task->nPeriods - 1]);

    taskDur = getCurrentTime() - period->start;
    (task->nPeriods)--;
    formatDur(taskDur, formatedDur);
    printf("Period \"%s\" canceled. Duration: %s.\n\n", period->name, formatedDur);
    calendar->periodSched = NULL;
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
    char formatedDur[10];

    if (calendar->periodSched == NULL) {
        printf("There is no period running.\n\n");
        return;
    }

    todo = calendar->periodSched->todo;
    while (todo->type != ROOT) todo = todo->parent.todo;
    task = todo->parent.task;
    period = &(task->periods[task->nPeriods - 1]);

    period->end = getCurrentTime();
    taskDur = period->end - period->start;
    formatDur(taskDur, formatedDur);
    printf("Time spent in period \"%s\". Duration: %s.\n\n", period->name, formatedDur);
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
    char formatedDur[10];

    if (calendar->periodSched == NULL) {
        return;
    }

    todo = calendar->periodSched->todo;
    while (todo->type != ROOT) todo = todo->parent.todo;
    task = todo->parent.task;
    period = &(task->periods[task->nPeriods - 1]);

    period->end = getCurrentTime();
    taskDur = period->end - period->start;
    formatDur(taskDur, formatedDur);
    printf("Period \"%s\" running! Duration: %s.\n\n", period->name, formatedDur);
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
    int timeSet;
    int estimate;
    
    id = atoi(getToken(1)) - 1;
    if (id < 0 || id >= calendar->nTodos) {
        printf("Invalid to-do ID.\n\n");
        return;
    }

    todo = calendar->todos[id];
    timeSet = (getNComms() == 5);


    sscanf(getToken(2), "%d/%d", &day, &mon);

    date = getCurrentTime();
    structTime = localtime(&date);

    if (structTime->tm_mon > mon) structTime->tm_year++;
    structTime->tm_sec = 0;

    structTime->tm_mon = mon - 1;
    structTime->tm_mday = day;

    if (timeSet) {
        sscanf(getToken(3), "%d:%d", &hour, &min);

        structTime->tm_hour = hour;
        structTime->tm_min = min;
        estimate = 60 * atof(getToken(4));
    } else {
        structTime->tm_hour = 0;
        structTime->tm_min = 0;
        estimate = 60 * atof(getToken(3));
    }

    date = mktime(structTime);

    createSchedule(todo, timeSet, date, estimate);

    updateCalendar();

    printf("To-do date scheduled to %02d/%02d/%04d.\n\n", day, mon, 1900 + structTime->tm_year);
}

/*
    removeSchedule()
    Delete schedule.
*/
void removeSchedule() {
    int id, i;

    id = calendar->nSchedules - atoi(getToken(1));

    if (id < 0 || id >= calendar->nSchedules) {
        printf("Invalid schedule ID.\n\n");
        return;
    }

    Schedule *sched = calendar->schedules[id];
    Todo *todo = sched->todo;

    printf("Schedule '%s' removed.\n\n", todo->name);

    todo->nSchedules--;

    i = 0;

    while (todo->schedules[i] != sched) i++;

    while (i < todo->nSchedules) {
        todo->schedules[i] = todo->schedules[i + 1];
        i++;
    }

    updateCalendar();

    free(sched);
}

/*
    editSchedule()
    Edits schedule attributes.
*/
void editSchedule() {
    int id, hour, min, day, mon, year;
    Schedule *sched;
    
    id = calendar->nSchedules - atoi(getToken(2));

    if (id < 0 || id >= calendar->nSchedules) {
        printf("Invalid to-do ID.\n\n");
        return;
    }

    sched = calendar->schedules[id];

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
    } else if (strcmp(getToken(1), "estimate") == 0) {
        sched->timeEstimate = 60 * atof(getToken(3));
        printf("Changed time estimate to %.1f\n\n", atof(getToken(3)));

    } else {
        printf("Invalid edit option.\n\n");
    }
}

/*
    changePrioritizeStatus()
    Changes status of prioritized to-do.
*/
void changePrioritizeStatus() {
    int id;
    Todo *todo;
    char *statusName;
    int status;
    
    id = atoi(getToken(1));

    id--;

    if (id < 0 || id >= calendar->nTodos) {
        printf("Invalid to-do ID.\n\n");
        return;
    }

    todo = calendar->todos[id];

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

    printf("\"%s\" status is set to \"%s\".\n\n", todo->name, statusName);
}

/*
    calendarMenu()
    Enters calendar menu.
*/
void calendarMenu() {
    char *commandName;
    updateCalendar();
    printCalendar();
    while (true) {
        printf(" _________________________  Calendar  _________________________\n\n");
        periodWarning();
        commandName = getCommandName();
        if (strcmp(commandName, "sched") == 0) {
            if (getNComms() == 4 || getNComms() == 5) {
                scheduleTodoCalendar();
                printCalendar();
            } else {
                printf("Invalid number of arguments.\n\n");
            }
        } else if (strcmp(commandName, "edit") == 0) {
            if (validArgs(3)) {
                editSchedule();
                printCalendar();
            } else {
                printf("Invalid number of arguments.\n\n");
            }
        } else if (strcmp(commandName, "rem") == 0) {
            if (validArgs(1)) {
                removeSchedule();
                printCalendar();
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
                printCalendar();
                saveAll();
            }
        } else if (strcmp(commandName, "cal") == 0) {
            if (validArgs(0)) {
                printCalendar();
            }
        } else if (strcmp(commandName, "week") == 0) {
            if (validArgs(0)) printWeekSummary(rootTask);
        } else if (strcmp(commandName, "cd") == 0) {
            if (validArgs(1)) {
                if (strcmp("..", getToken(1)) == 0) {
                    return;
                } else {
                    printf("Type 'cd ..' to go back\n\n");
                }
            }
        } else if (strcmp(commandName, "help") == 0) {
            if (validArgs(0)) printHelp(TODOS_MENU);
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
    }
}
