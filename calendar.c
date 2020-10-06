#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "calendar.h"
#include "objectives.h"
#include "util.h"

char *wDayName[] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};
char *wDayShort[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"};

/*
    createCalendar()
    DOCUMENTAR.
*/
Calendar *createCalendar() {
    Calendar* newCalendar;
    newCalendar = (Calendar *) mallocSafe(sizeof(Calendar));
    newCalendar->nItems = 0;
    return newCalendar;
}

/*
    fillCalendar()
    DOCUMENTAR.
*/
void fillCalendar(Calendar *calendar, Task *task) {
    int i;
    for (i = 0; i < task->nTodos; i++) {
        calendar->items[calendar->nItems++] = task->todos[i];
    }
    for (i = 0; i < task->nSubtasks; i++) {
        fillCalendar(calendar, task->subtasks[i]);
    }
}

/*
    sortCalendar()
    DOCUMENTAR.
*/
// void sortCalendar(Todo **items, int s, int e) {
//     int i, j, k;
//     int m = (s + e) / 2;
//     int size1 = m - s + 1;
//     int size2 = e - m;
//     Todo **v1;
//     Todo **v2;
//     if (e == s) return;
//     sortCalendar(items, s, m);
//     sortCalendar(items, m + 1, e);
//     v1 = (Todo **) mallocSafe(size1 * sizeof(Todo *));
//     v2 = (Todo **) mallocSafe(size2 * sizeof(Todo *));
//     for (i = 0; i < size1; i++) {
//         v1[i] = items[s + i];
//     }
//     for (i = 0; i < size2; i++) {
//         v2[i] = items[m + 1 + i];
//     }
//     i = 0;
//     j = 0;
//     k = s;
//     while (i < size1 && j < size2) {
//         if (v1[i]->planned == 0 || (v2[j]->planned != 0 && v1[i]->planned >= v2[j]->planned)) {
//             items[k++] = v1[i++];
//         } else {
//             items[k++] = v2[j++];
//         }
//     }
//     while (i < size1) {
//         items[k++] = v1[i++];
//     }
//     while (j < size2) {
//         items[k++] = v2[j++];
//     }
//     free(v1);
//     free(v2);
// }

/*
    printCalendar()
    DOCUMENTAR.
*/
// void printCalendar(Calendar *calendar, Task *root) {
//     int item;
//     Todo *curTodo;
//     struct tm *stm;
//     long int curDay = getCurrentTime() - 3600*3;
//     long int prevDay;
//     setUPath(root, root);
//     calendar->nItems = 0;
//     fillCalendar(calendar, root);
//     if (calendar->nItems == 0) {
//         printf("Empty calendar.\n\n");
//         return;
//     }
//     sortCalendar(calendar->items, 0, calendar->nItems - 1);
    /*
    if (item < calendar->nItems && calendar->items[item]->planned == 0)
        printf("  ********************  Sem data  ********************\n\n");
    for (item = 0; item < calendar->nItems && (curTodo = calendar->items[item])->planned == 0; item++) {
        printf("      * %s (%s)\n\n", curTodo->name, curTodo->parent->uniquePath);
        item++;
    }
    */
//     curTodo = calendar->items[0];
//     for (item = 0; item < calendar->nItems && (curTodo = calendar->items[item])->planned == 0; item++);
//     while (item < calendar->nItems && curTodo->planned / SECS_IN_A_DAY >= curDay / SECS_IN_A_DAY) {
//         stm = localtime(&(curTodo->planned));
//         printf("  ********************  %s (%02d/%02d)  ********************\n\n", wDayName[stm->tm_wday], stm->tm_mday, stm->tm_mon + 1);
//         while (item < calendar->nItems) {
//             prevDay = curTodo->planned;
//             curTodo = calendar->items[item];
//             if (curTodo->planned / SECS_IN_A_DAY != prevDay / SECS_IN_A_DAY) break;
//             printf("      * %s (%s)\n\n", curTodo->name, curTodo->parent->uniquePath);
//             item++;
//         }
//     }
//     if (item < calendar->nItems) printf("  ********************  Atrasados  ********************\n\n");
//     while (item < calendar->nItems) {
//         curTodo = calendar->items[item];
//         stm = localtime(&(curTodo->planned));
//         printf("      * (%02d/%02d) %s (%s)\n\n", stm->tm_mday, stm->tm_mon + 1, curTodo->name, curTodo->parent->uniquePath);
//         item++;
//     }
// }

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
    sortPeriods()
    DOCUMENTAR.
*/
void sortPeriods(Period **items, int s, int e) {
    int i, j, k;
    int m = (s + e) / 2;
    int size1 = m - s + 1;
    int size2 = e - m;
    Period **v1;
    Period **v2;
    if (e == s) return;
    sortPeriods(items, s, m);
    sortPeriods(items, m + 1, e);
    v1 = (Period **) mallocSafe(size1 * sizeof(Period *));
    v2 = (Period **) mallocSafe(size2 * sizeof(Period *));
    for (i = 0; i < size1; i++) {
        v1[i] = items[s + i];
    }
    for (i = 0; i < size2; i++) {
        v2[i] = items[m + 1 + i];
    }
    i = 0;
    j = 0;
    k = s;
    while (i < size1 && j < size2) {
        if (v1[i]->start < v2[j]->start) {
            items[k++] = v1[i++];
        } else {
            items[k++] = v2[j++];
        }
    }
    while (i < size1) {
        items[k++] = v1[i++];
    }
    while (j < size2) {
        items[k++] = v2[j++];
    }
    free(v1);
    free(v2);
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
    sortPeriods(periodList, 0, n-1);
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