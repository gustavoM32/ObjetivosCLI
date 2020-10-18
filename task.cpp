#include <string.h>
#include <stdlib.h>
#include "task.hpp"
#include "calendar.hpp"
#include "util.hpp"
#include "io.hpp"
#include "help.hpp"

static Task *selectedTask;

/*
    findTaskByName()
    Returns index of pointer to subtask named 'name' in subtask vector of task
    'parent'. Returns -1 if there's no such subtask.
*/
int findTaskByName(Task* parent, char* name) {
    int i;
    for (i = 0; i < parent->nSubtasks; i++) {
        if (strcmp(parent->subtasks[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/*
    findTaskByCode()
    Returns index of pointer to subtask with code 'code' in subtask vector of task
    'parent'. Returns -1 if there's no such subtask.
*/
int findTaskByCode(Task* parent, char* code) {
    int i;
    for (i = 0; i < parent->nSubtasks; i++) {
        if (strcmp(parent->subtasks[i]->code, code) == 0) {
            return i;
        }
    }
    return -1;
}

/*
    getSubtask()
    Gets subtask code from user and returns a pointer to it if it exist in task
    pointed by 'task'.
*/
Task* getSubtask(Task* task, char *subtaskCode) {
    int id;
    subtaskCode = toUppercase(getToken(1));

    id = findTaskByCode(task, subtaskCode);
    if (id == -1) {
        printf("\"%s\" has no subtask with code \"%s\".\n\n", task->name, subtaskCode);
        return NULL;
    }
    return task->subtasks[id];
}

/*
    createTask()
    Creates task named 'name' and with code 'code' and returns a pointer to it.
*/
Task* createTask(char *name, char *code) {
    Task* newTask;
    newTask = (Task *) mallocSafe(sizeof(Task));
    strcpy(newTask->name, name);
    strcpy(newTask->uniquePath, "");
    strcpy(newTask->code, code);
    newTask->nNotes = 0;
    newTask->nTodos = 0;
    newTask->nSubtasks = 0;
    newTask->nPeriods = 0;
    newTask->status = TASK_ACTIVE;
    newTask->parent = NULL;
    return newTask;
}

/*
	freeTask()
	Frees the memory allocated to 'task' and its substasks.
*/
void freeTask(Task *task) {
	int i;
	for (i = 0; i < task->nTodos; i++) {
		freeTodo(task->todos[i]);
	}
	for (i = 0; i < task->nSubtasks; i++) {
		freeTask(task->subtasks[i]);
	}
	free(task);
}

/*
    addSubtask()
    Gets user new subtask code and name and adds it to task 'parent' subtask
    list.
*/
void addSubtask(Task* parent) {
    char *subtaskName;
    char *subtaskCode;
    subtaskCode = toUppercase(getToken(1));
    subtaskName = getToken(2);
    if (strlen(subtaskCode) == 0 || strlen(subtaskCode) > 5) {
        printf("Subtask code must be 1-5 characters long.\n\n");
        return;
    } else if (findTaskByCode(parent, subtaskCode) != -1) {
        printf("\"%s\" already has subtask with code \"%s\".\n\n", parent->name, subtaskCode);
        return;
    }
    if (strlen(subtaskName) == 0) {
        printf("Subtask name must not be empty.\n\n");
        return;
    } else if (findTaskByName(parent, subtaskName) != -1) {
        printf("\"%s\" already has subtask named \"%s\".\n\n", parent->name, subtaskName);
        return;
    }
    parent->subtasks[parent->nSubtasks] = createTask(subtaskName, subtaskCode);
    parent->subtasks[parent->nSubtasks]->parent = parent;
    (parent->nSubtasks)++;
    printf("Added task \"%s\".\n\n", subtaskName);
}

/*
    removeSubtask()
    Gets subtask code from user and deletes it from subtask list of task pointed
    by 'parent'
*/
void removeSubtask(Task *parent) {
    char *subtaskCode;
    int id;
    subtaskCode = toUppercase(getToken(1));
    id = findTaskByCode(parent, subtaskCode);
    if (id == -1) {
        printf("There's no task with code \"%s\".\n\n", subtaskCode);
        return;
    }

    printf("Removed task (%s) %s.\n\n", parent->subtasks[id]->code, parent->subtasks[id]->name);

    freeTask(parent->subtasks[id]);

    parent->nSubtasks--;
    while (id < parent->nSubtasks) {
        parent->subtasks[id] = parent->subtasks[id + 1];
        id++;
    }
}

void listStatusTasks(Task* tasks[], int count, char statusName[]) {
    int i;
    if (count == 0) {
        printf("  |   You have no %s subtasks.\n", statusName);
        printf("  |\n");
    } else {
        statusName[0] = statusName[0] - 'a' + 'A';
        printf("  |   %s:\n", statusName);
        printf("  |\n");
        for (i = 0; i < count; i++) {
            printf("  |    (%s) %s (%d)\n", tasks[i]->code, tasks[i]->name, countTodosTask(tasks[i]));
        }
        printf("  |\n");
    }
}

/*
    listSubtasks()
    Lists all subtasks of task pointed by 'task'.
*/
void listSubtasks(Task* task) {
    int i;
    char statusNames[4][NAME_LEN] = {"active", "inactive", "completed", "canceled"};
    int count[4] = {0, 0, 0, 0};
    Task* lists[4][MAX_CHILDS];
    for (i = 0; i < task->nSubtasks; i++) {
        int status = task->subtasks[i]->status;
        lists[status][count[status]] = task->subtasks[i];
        count[status]++;
    }
    if ((count[0] + count[1] + count[2] + count[3]) == 0) {
        printf("  +---------------------------> Subtasks <---------------------------+\n");
        printf("  |\n");
        printf("  |   This task has no subtasks.\n");
        printf("  |\n");
        printf("  +------------------------------------------------------------------+\n\n");
        return;
    }
    printf("  +---------------------------> Subtasks <---------------------------+\n");
    printf("  |\n");
    for (i = 0; i < 4; i++) {
        listStatusTasks(lists[i], count[i], statusNames[i]);
    }
    printf("  +------------------------------------------------------------------+\n\n");
}

/*
    getTaskTotalTime()
    Returns total period time of task pointed by 'task' and its subtasks.
*/
long int getTaskTotalTime(Task* task) {
    long int totalTime = 0;
    int i;
    for (i = 0; i < task->nPeriods; i++) {
        totalTime += task->periods[i].end - task->periods[i].start;
    }
    for (i = 0; i < task->nSubtasks; i++) {
        totalTime += getTaskTotalTime(task->subtasks[i]);
    }
    return totalTime;
}

/*
    showTaskTotalTime();
    Prints total period time of task pointed by 'task' and all of its subtasks.
*/
void showTaskTotalTime(Task* task) {
    long int totalTime;
    char formatedTime[10];
    totalTime = getTaskTotalTime(task);
    formatDur(totalTime, formatedTime);
    printf("Time spent in task (%s) %s: %s.\n\n", task->code, task->name, formatedTime);
}

/*
    getTaskWeekTime()
    Returns week period time of task pointed by 'task' and its subtasks.
*/
long int getTaskWeekTime(Task* task) {
    long int curTime = getCurrentTime();
    long int objStart = getTime(25, 8, 2019, 0, 0, 0);
    long int weekProgress = (curTime - objStart) % SECS_IN_A_WEEK;
    long int inicioSemana = curTime - weekProgress;
    long int totalTime = 0;
    int i;
    for (i = 0; i < task->nPeriods; i++) {
        long int inicio = inicioSemana;
        long int timeThisWeek;
        if (task->periods[i].start > inicio) inicio = task->periods[i].start;
        timeThisWeek = task->periods[i].end - inicio;
        if (timeThisWeek > 0) totalTime += timeThisWeek;
    }
    for (i = 0; i < task->nSubtasks; i++) {
        totalTime += getTaskWeekTime(task->subtasks[i]);
    }
    return totalTime;
}

/*
    showTaskWeekTime();
    Prints week period time of task pointed by 'task' and all of its subtasks.
*/
void showTaskWeekTime(Task* task) {
    long int totalTime;
    char formatedTime[10];
    totalTime = getTaskWeekTime(task);
    formatDur(totalTime, formatedTime);
    printf("Time spent in task (%s) %s this week: %s.\n\n", task->code, task->name, formatedTime);
}


/*
    renameTask()
    Renames task pointed by 'task' to name inputed by user.
*/
void renameTask(Task* task) {
    char *taskName;
    taskName = getToken(1);
    if (strlen(taskName) == 0) {
        printf("Subtask name must not be empty.\n\n");
        return;
    } else if (task->parent == NULL) {
        printf("Can't rename main task.\n\n");
        return;
    } else if (findTaskByName(task->parent, taskName) != -1) {
        printf("\"%s\" already has subtask named \"%s\".\n\n", task->parent->name, taskName);
        return;
    }
    printf("\"%s\" renamed to \"%s\"\n\n", task->name, taskName);
    strcpy(task->name, taskName);
}

/*
    renameTask()
    Changes code of task pointed by 'task' to code inputed by user.
*/
void changeCode(Task* task) {
    char *code;
    code = toUppercase(getToken(1));
    if (strlen(code) == 0 || strlen(code) > 5) {
        printf("Subtask code must be 1-5 characters long.\n\n");
        return;
    } else if (task->parent == NULL) {
        printf("Can't change main task code.\n\n");
        return;
    } else if (findTaskByCode(task->parent, code) != -1) {
        printf("\"%s\" already has subtask with code \"%s\".\n\n", task->parent->code, code);
        return;
    }
    printf("\"%s\" code changed to \"%s\"\n\n", task->code, code);
    strcpy(task->code, code);
}

/*
    selectSubtask()
    Saves address of subtask of 'parent' inputed by user.
*/
void selectSubtask(Task* parent) {
    Task *task;
    char *taskCode;
    taskCode = toUppercase(getToken(1));
    task = getSubtask(parent, taskCode);
    if (task == NULL) return;
    selectedTask = task;
    printf("Selected task (%s) %s.\n\n", selectedTask->code, selectedTask->name);
}

/*
    moveTask()
    Moves selected task to 'parent' subtask list.
*/
void moveTask(Task* newParent) {
    int id;

    if (selectedTask == NULL) {
        printf("There's no selected task.\n\n");
        return;
    } else if (findTaskByCode(newParent, selectedTask->code) != -1) {
        printf("\"%s\" already has subtask with code \"%s\".\n\n", newParent->name, selectedTask->code);
        return;
    } else if (findTaskByName(newParent, selectedTask->name) != -1) {
        printf("\"%s\" already has subtask named \"%s\".\n\n", newParent->name, selectedTask->name);
        return;
    }

    printf("Moved task (%s) \"%s\" from (%s) to (%s)\n\n", selectedTask->code, selectedTask->name, selectedTask->parent->code, newParent->code);

    /* Removes from old parent */
    selectedTask->parent->nSubtasks--;
    for (id = 0; selectedTask->parent->subtasks[id] != selectedTask; id++);
    while (id < selectedTask->parent->nSubtasks) {
        selectedTask->parent->subtasks[id] = selectedTask->parent->subtasks[id + 1];
        id++;
    }

    /* Adds in new parent */
    newParent->subtasks[newParent->nSubtasks] = selectedTask;
    selectedTask->parent = newParent;
    (newParent->nSubtasks)++;

    selectedTask = NULL;
}

/*
    allCompleted()
    Returns 1 if all subtasks of task pointed by 'task' are completed, and 0
    otherwise.
*/
bool allCompleted(Task* task) {
    bool res = true;
    int i;
    for (i = 0; i < task->nSubtasks && res; i++) {
        res = task->subtasks[i]->status == TASK_COMPLETED && allCompleted(task->subtasks[i]);
    }
    return res;
}

/*
    setSubtaskStatus()
    Changes status of a subtask of task pointed by 'task' to status inputed by
    user.
*/
void setSubtaskStatus(Task *task) {
    char *statusName;
    Task *subtask;
    int status;
    subtask = getSubtask(task, getToken(1));
    statusName = toLowercase(getToken(2));
    if (strcmp(statusName, "active") == 0) {
        status = TASK_ACTIVE;
    } else if (strcmp(statusName, "inactive") == 0) {
        status = TASK_INACTIVE;
    } else if (strcmp(statusName, "completed") == 0) {
        status = TASK_COMPLETED;
    } else if (strcmp(statusName, "canceled") == 0) {
        status = TASK_CANCELED;
    } else {
        printf("\"%s\" is not a valid status.\n\n", statusName);
        return;
    }
    if (subtask == NULL) return;
    if (status == TASK_COMPLETED && !allCompleted(subtask)) {
        printf("You can only complete \"%s\" when all its subtasks are completed.\n\n", subtask->name);
        return;
    }
    subtask->status = status;
    printf("\"%s\" is set to \"%s\"\n\n", subtask->name, statusName);
}

/*
    addNote()
    Ask user for note content and add it in task pointed by 'task' notes.
*/
void addNote(Task *task) {
    char *noteText;
    noteText = getToken(1);
    if (noteText[0] == '\0') {
        printf("Note text must not be empty.\n\n");
        return;
    }
	task->notes[task->nNotes].date = getCurrentTime();
	strcpy(task->notes[task->nNotes++].text, noteText);
    printf("Added note\n\n");
}

/*
    removeNote()
    Ask user for note ID and remove it from task pointed by 'task'.
*/
void removeNote(Task* task) {
    int id;
    sscanf(getToken(1), "%d", &id);
    id--;
    if (id < 0 || task->nNotes <= id) {
        printf("Invalid note ID.\n\n");
        return;
    }
    task->nNotes--;
    printf("Note %d removed.\n\n", id + 1);
    while (id < task->nNotes) {
        task->notes[id] = task->notes[id + 1];
        id++;
    }
}

/*
    listNotes()
    Print list of notes of task pointed by 'task'.
*/
void listNotes(Task* task) {
    int i;
    struct tm *stm;
    printf("  +--------------------------> Notes <--------------------------+\n");
    printf("  |\n");
    if (task->nNotes == 0) {
        printf("  |   No notes\n");
    }
    for (i = 0; i < task->nNotes; i++) {
        stm = localtime(&(task->notes[i].date));
        printf("  |   %2d. (%02d/%02d/%04d) %s\n", i + 1, stm->tm_mday, stm->tm_mon + 1, stm->tm_year + 1900, task->notes[i].text);
    }
    printf("  |\n");
    printf("  +------------------------------------------------------------------+\n\n");
}

/*
    getTasks()
    Inserts all TASK subtasks into tasks and the vector size into N.
*/
void getTasks(Task *tasks[], int *n, Task *task) {
    int i;
    for (i = 0; i < task->nSubtasks; i++) {
        tasks[(*n)++] = task->subtasks[i];
        getTasks(tasks, n, task->subtasks[i]);
    }
}

/*
    searchTask()
    Asks user for task suffix path and returns pointer to task.
*/
Task *searchTask(Task *root) {
    int nResults = 1;
    int old, newI, i, j, id;
    Task *results[MAX_CHILDS * MAX_CHILDS];
    int nInputs = getNComms() - 1;
    char *inputCodes[MAX_CHILDS];
    for (i = 0; i < nInputs; i++) {
        inputCodes[i] = toUppercase(getToken(i+1));
    }
    results[0] = root;

    getTasks(results, &nResults, root);
    setUPath(root, root);
    for (old = 0, newI = 0; old < nResults; old++) {
        int pathLen = 0;
        char codes[MAX_CHILDS][CODE_LEN];
        bool match = true;
        i = 0;
        while (true) {
            for (j = 0; j < 3; j++) {
                codes[pathLen][j] = results[old]->uniquePath[i+j];
            }
            codes[pathLen][3] = '\0';
            pathLen++;
            if (results[old]->uniquePath[i+3] == '\0') break;
            i += 6;
        }
        if (pathLen < nInputs) continue;
        for (i = pathLen - 1, j = nInputs - 1; j >= 0; i--, j--) {
            if (strcmp(inputCodes[j], codes[i]) != 0) match = false;
        }
        if (!match) {
            continue;
        }
        if (newI == old) {
            newI++;
            continue;
        }
        results[newI++] = results[old];
    }
    nResults = newI;
    if (newI == 0) {
        printf("No task found.\n\n");
        return NULL;
    }
    if (newI == 1) return results[0];
    printf("Multiple tasks match the path given:\n\n");
    for (i = 0; i < nResults; i++) {
        printf("   %d. %s (%s)\n\n", i + 1, results[i]->name, results[i]->uniquePath);
    }
    printf("Select desired task: ");
    scanf("%d", &id);
    getchar();
    printf("\n");
    id--;
    if (id < 0 || id >= nResults) {
        printf("Invalid ID.\n\n");
        return NULL;
    }
    return results[id];
}

/*
    subtasksMenu()
    Enters in subtask menu of task pointed by 'task'.
*/
void subtasksMenu(Task* task) {
    char *commandName;
    printf(" ________________________  Subtasks Menu (%s)  ________________________\n\n", task->code);
    listSubtasks(task);
    while (true) {
        printf(" ________________________  Subtasks Menu (%s)  ________________________\n\n", task->code);
        commandName = getCommandName();
        if (strcmp(commandName, "add") == 0) {
            if (validArgs(2)) {
                addSubtask(task);
                saveAll();
            }
        } else if (strcmp(commandName, "rem") == 0) {
            if (validArgs(1)) {
                removeSubtask(task);
                saveAll();
            }
        } else if (strcmp(commandName, "select") == 0) {
            if (validArgs(1)) selectSubtask(task);
        } else if (strcmp(commandName, "move") == 0) {
            if (validArgs(0)) {
                moveTask(task);
                saveAll();
            }
        } else if (strcmp(commandName, "sts") == 0) {
            if (validArgs(0)) listSubtasks(task);
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

/*
    taskMenu()
    Enters in task menu of task pointed by 'task'.
*/
void taskMenu(Task* task) {
    char *commandName;
    bool showHead = true;
    while (true) {
        if (showHead) {
            printf("***************************  Task Menu (%s)  ***************************\n\n", task->code);
            listSubtasks(task);
            showHead = false;
        }
        printf("***************************  Task Menu (%s)  ***************************\n\n", task->code);
        printf(" - %s\n\n", task->name);
        periodWarning();
        commandName = getCommandName();
        if (strcmp(commandName, "rename") == 0) {
            if (validArgs(1)) {
                renameTask(task);
                saveAll();
            }
        } else if (strcmp(commandName, "code") == 0) {
            if (validArgs(1)) {
                changeCode(task);
                saveAll();
            }
        } else if (strcmp(commandName, "set") == 0) {
            if (validArgs(2)) {
                setSubtaskStatus(task);
                saveAll();
            }
        } else if (strcmp(commandName, "time") == 0) {
            if (validArgs(0)) showTaskTotalTime(task);
        } else if (strcmp(commandName, "weektime") == 0) {
            if (validArgs(0)) showTaskWeekTime(task);
        } else if (strcmp(commandName, "noteadd") == 0) {
            if (validArgs(1)) {
                addNote(task);
            }
        } else if (strcmp(commandName, "noterem") == 0) {
            if (validArgs(1)) {
                removeNote(task);
            }
        } else if (strcmp(commandName, "pds") == 0) {
            if (task == rootTask) {
                printf("Main task doesn't have periods.\n\n");
            } else {
                if (validArgs(0)) {
                    curMenu = PERIODS_MENU;
                    return;
                }
            }
        } else if (strcmp(commandName, "sts") == 0) {
            if (validArgs(0)) {
                curMenu = SUBTASKS_MENU;
                return;
            }
        } else if (strcmp(commandName, "tds") == 0) {
            if (validArgs(0)) {
                curMenu = TODOS_MENU;
                return;
            }
        } else if (strcmp(commandName, "nts") == 0) {
            if (validArgs(0)) {
                listNotes(task);
            }
        // } else if (strcmp(commandName, "alltds") == 0) {
            // if (validArgs(0)) printNoDateTodos(todoCalendar, rootTask);
        } else if (strcmp(commandName, "ls") == 0) {
            if (validArgs(0)) {
                listSubtasks(task);
                listTodos(task, 0);
            }
        } else if (strcmp(commandName, "cd") == 0) {
            if (validArgs(1)) {
                if (strcmp("..", getToken(1)) == 0) {
                    if (task == rootTask) {
                        printf("Can't go to main task parent.\n\n");
                    } else {
                        curTask = task->parent;
                        return;
                    }
                } else {
                    Task *nextTask = getSubtask(task, getToken(1));
                    if (nextTask != NULL) {
                        curTask = nextTask;
                        return;
                    }
                }
            }
        } else if (strcmp(commandName, "goto") == 0) {
            if (getNComms() > 1) {
                curTask = searchTask(rootTask);
                if (curTask != NULL) break;
                    curTask = task;
            } else {
                printf("Usage: goto [...] [<parent code>] <task code>\n\n");
            }
        } else if (generalCommands(commandName)) return;
    }
}
