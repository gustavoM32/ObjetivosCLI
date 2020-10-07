#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "calendar.h"
#include "objectives.h"
#include "io.h"
#include "util.h"
#include "help.h"

char *wDayName[] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};
char *wDayShort[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"};

/*
    fillCalendarSchedules()
    Fills calendar with to-do schedules.
*/
void fillCalendarSchedules(Calendar *calendar, Todo *todo) {
    int i;
    for (i = 0; i < todo->nSchedules; i++) {
        calendar->schedules[calendar->nSchedules] = todo->schedules[i];
        calendar->nSchedules++;
    }
}

/*
    fillCalendarTodo()
    Fills calendar with sub-to-dos of todo.
*/
void fillCalendarTodo(Calendar *calendar, Todo *todo) {
    int i;
    for (i = 0; i < todo->nSubtodos; i++) {
        if (todo->subtodos[i]->status == TODO_PRIORITY) {
            calendar->todos[calendar->nTodos] = todo->subtodos[i];
            calendar->nTodos++;
        }
        fillCalendarSchedules(calendar, todo->subtodos[i]);
        fillCalendarTodo(calendar, todo->subtodos[i]);
    }
}

/*
    fillCalendarTask()
    Fills calendar with to-dos of task.
*/
void fillCalendarTask(Calendar *calendar, Task *task) {
    int i;
    for (i = 0; i < task->nTodos; i++) {
        if (task->todos[i]->status == TODO_PRIORITY) {
            calendar->todos[calendar->nTodos] = task->todos[i];
            calendar->nTodos++;
        }
        fillCalendarSchedules(calendar, task->todos[i]);
        fillCalendarTodo(calendar, task->todos[i]);
    }
    for (i = 0; i < task->nSubtasks; i++) {
        fillCalendarTask(calendar, task->subtasks[i]);
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
void sortCalendar(Calendar *calendar) {
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
void printScheduled(Calendar *calendar) {
    int i;    
    time_t curDayStart = getDayStart(getCurrentTime());

    sortCalendar(calendar);

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
                printf("\n ________________  Late  _________________\n\n");
            } else {
                while (sched->date < dayStart) {
                    dayStart -= SECS_IN_A_DAY;
                    localtime_r(&dayStart, &date);
                    printf("\n _____________  %s (%02d/%02d)  _____________\n\n", wDayShort[date.tm_wday], date.tm_mday, date.tm_mon + 1);
                }
            }

            printf("    %2d: ", calendar->nSchedules - i);

            if (sched->timeSet) {
                printf("%02d:%02d ", schedDate.tm_hour, schedDate.tm_min);
            } else {
                printf("  :   ");
            }

            char todoName[NAME_LEN];
            getTodoFullName(sched->todo, todoName);
            printf("(%.1f/%.1f) %s\n", sched->timeSpent / 60.0, sched->timeEstimate / 60.0, todoName);

            i--;
        } while (i >= 0);
    }

    printf(" ___________________________________________________\n\n");
}

/*
    printPrioritized()
    Prints prioritized to-dos.
*/
void printPrioritized(Calendar *calendar) {
    int i;    

    printf(" __________________  Prioritized  __________________\n\n");

    for (i = 0; i < calendar->nTodos; i++) {
        Todo *todo = calendar->todos[i];
        char todoName[NAME_LEN];
        getTodoFullName(todo, todoName);
        printf("    %2d: (%.1f/%.1f) %s\n", i + 1, todo->timeSpent / 60.0, todo->timeEstimate / 60.0, todoName);
    }

    printf(" ___________________________________________________\n\n");
}

void printCalendar(Calendar *calendar) {
    printScheduled(calendar);
    printPrioritized(calendar);
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
//     fillCalendar(calendar, root);
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
    scheduleTodo()
    Creates a schedule for a todo.
*/
void scheduleTodo(Calendar *calendar) {
    int id, day, mon, hour, min;
    Todo *todo;
    Schedule *sched;
    time_t date;
    struct tm *structTime;
    
    sscanf(getToken(1), "%d", &id);
    id--;
    if (id < 0 || id >= calendar->nTodos) {
        printf("Invalid to-do ID.\n\n");
        return;
    }

    sched = (Schedule *) mallocSafe(sizeof(Schedule));
    todo = calendar->todos[id];
    sched->timeSet = (getNComms() == 5);


    sscanf(getToken(2), "%d/%d", &day, &mon);

    date = getCurrentTime();
    structTime = localtime(&date);

    if (structTime->tm_mon > mon) structTime->tm_year++;
    structTime->tm_sec = 0;

    structTime->tm_mon = mon - 1;
    structTime->tm_mday = day;

    if (sched->timeSet) {
        sscanf(getToken(3), "%d:%d", &hour, &min);

        structTime->tm_hour = hour;
        structTime->tm_min = min;
        sched->timeEstimate = 60 * atof(getToken(4));
    } else {
        structTime->tm_hour = 0;
        structTime->tm_min = 0;
        sched->timeEstimate = 60 * atof(getToken(3));
    }

    sched->date = mktime(structTime);

    sched->todo = todo;
    todo->schedules[todo->nSchedules] = sched;
    todo->nSchedules++;
    calendar->schedules[calendar->nSchedules] = sched;
    calendar->nSchedules++;

    printf("To-do date scheduled to %02d/%02d/%04d.\n\n", day, mon, 1900 + structTime->tm_year);
}

/*
    removeSchedule()
    Delete schedule.
*/
void removeSchedule(Calendar *calendar) {
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
    calendar->nSchedules--;

    while (todo->schedules[i] != sched) i++;

    while (i < todo->nSchedules) {
        todo->schedules[i] = todo->schedules[i + 1];
        i++;
    }

    while (id < calendar->nSchedules) {
        calendar->schedules[id] = calendar->schedules[id + 1];
        id++;
    }

    free(sched);
}

/*
    editSchedule()
    Edits schedule attributes.
*/
void editSchedule(Calendar *calendar) {
    int id, hour, min;
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

    } else if (strcmp(getToken(1), "estimate") == 0) {
        sched->timeEstimate = 60 * atof(getToken(3));
        printf("Changed time estimate to %.1f\n\n", atof(getToken(3)));

    } else {
        printf("Invalid edit option.\n\n");
    }
}

/*
    createCalendar()
    Creates a updated calendar.
*/
Calendar *createCalendar() {
    Calendar* calendar;
    calendar = (Calendar *) mallocSafe(sizeof(Calendar));
    calendar->nTodos = 0;
    calendar->nSchedules = 0;
    fillCalendarTask(calendar, rootTask);
    setUPath(rootTask, rootTask);
    return calendar;
}

/*
    calendarMenu()
    Enters calendar menu.
*/
void calendarMenu() {
    Calendar *calendar = createCalendar();
    char *commandName;
    printCalendar(calendar);
    while (true) {
        printf(" _________________________  Calendar  _________________________\n\n");
        commandName = getCommandName();
        if (strcmp(commandName, "sched") == 0) {
            if (getNComms() == 4 || getNComms() == 5) {
                scheduleTodo(calendar);
                printCalendar(calendar);
            } else {
                printf("Invalid number of arguments.\n\n");
            }
        } else if (strcmp(commandName, "edit") == 0) {
            if (validArgs(3)) {
                editSchedule(calendar);
                printCalendar(calendar);
            } else {
                printf("Invalid number of arguments.\n\n");
            }
        } else if (strcmp(commandName, "rem") == 0) {
            if (validArgs(1)) {
                removeSchedule(calendar);
                printCalendar(calendar);
            }
        } else if (strcmp(commandName, "cal") == 0) {
            if (validArgs(0)) {
                printCalendar(calendar);
            }
        } else if (strcmp(commandName, "cd") == 0) {
            if (validArgs(1)) {
                if (strcmp("..", getToken(1)) == 0) {
                    free(calendar);
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
                free(calendar);
				freeAll();
                printf("Exiting...\n\n");
                exit(EXIT_SUCCESS);
            }
        } else {
            notAvailable(commandName);
        }
    }
}
