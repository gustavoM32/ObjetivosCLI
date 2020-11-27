#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <cstring>
#include <stdlib.h>
#include "task.hpp"
#include "calendar.hpp"
#include "util.hpp"
#include "io.hpp"
#include "help.hpp"

using namespace std;

static Task *selectedTask;

/*
    findTaskByName()
    Returns index of pointer to subtask named 'name' in subtask vector of task
    'parent'. Returns -1 if there's no such subtask.
*/
Task *findTaskByName(Task* parent, string &name) {
    for (auto it = parent->subtasks.begin(); it != parent->subtasks.end(); it++) {
        Task *subtask = *it;
        if (subtask->name == name) {
            return subtask;
        }
    }
    return nullptr;
}

/*
    findTaskByCode()
    Returns index of pointer to subtask with code 'code' in subtask vector of task
    'parent'. Returns -1 if there's no such subtask.
*/
Task *findTaskByCode(Task* parent, string &code) {
    for (auto it = parent->subtasks.begin(); it != parent->subtasks.end(); it++) {
        Task *subtask = *it;
        if (subtask->code == code) {
            return subtask;
        }
    }
    return nullptr;
}

/*
    getSubtask()
    Gets subtask code from user and returns a pointer to it if it exist in task
    pointed by 'task'.
*/
Task* getSubtask(Task* task) {
    string subtaskCode = toUppercase(getToken(1));

    Task *subtask = findTaskByCode(task, subtaskCode);
    
    if (subtask == nullptr) {
        printf("\"%s\" has no subtask with code \"%s\".\n\n", task->name.c_str(), subtaskCode.c_str());
    }

    return subtask;
}

/*
    createTask()
    Creates task named 'name' and with code 'code' and returns a pointer to it.
*/
Task* createTask(string name, string code) {
    Task* newTask = new Task;
    newTask->name = name;
    // newTask->uniquePath = "";
    newTask->code = code;
    newTask->status = TASK_ACTIVE;
    newTask->parent = NULL;
    return newTask;
}

/*
	freeTask()
	Frees the memory allocated to 'task' and its substasks.
*/
void freeTask(Task *task) {
    for (auto it = task->todos.begin(); it != task->todos.end(); it++) {
        freeTodo(*it);
    }
    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        freeTask(*it);
    }

    delete task;
}

/*
    addSubtask()
    Gets user new subtask code and name and adds it to task 'parent' subtask
    list.
*/
void addSubtask(Task* parent) {
    string subtaskName;
    string subtaskCode;
    subtaskCode = toUppercase(getToken(1));
    subtaskName = getToken(2);

    if (subtaskCode.size() || subtaskCode.size() > 5) {
        printf("Subtask code must be 1-5 characters long.\n\n");
        return;
    } else if (findTaskByCode(parent, subtaskCode) != nullptr) {
        printf("\"%s\" already has subtask with code \"%s\".\n\n", parent->name.c_str(), subtaskCode.c_str());
        return;
    }

    if (subtaskName.size() == 0) {
        printf("Subtask name must not be empty.\n\n");
        return;
    } else if (findTaskByName(parent, subtaskName) != nullptr) {
        printf("\"%s\" already has subtask named \"%s\".\n\n", parent->name.c_str(), subtaskName.c_str());
        return;
    }

    Task *task = createTask(subtaskName.c_str(), subtaskCode.c_str());
    parent->subtasks.push_back(task);
    task->parent = parent;

    printf("Added task \"%s\".\n\n", subtaskName.c_str());
}

/*
    removeSubtask()
    Gets subtask code from user and deletes it from subtask list of task pointed
    by 'parent'
*/
void removeSubtask(Task *parent) {
    string subtaskCode;
    subtaskCode = toUppercase(getToken(1));
    Task *task = findTaskByCode(parent, subtaskCode);

    if (task == nullptr) {
        printf("There's no task with code \"%s\".\n\n", subtaskCode.c_str());
        return;
    }

    printf("Removed task (%s) %s.\n\n", task->code.c_str(), task->name.c_str());
    
    freeTask(task);

    parent->subtasks.remove(task);
}

void listStatusTasks(list<Task *> tasks, string statusName) {
    if (tasks.size() == 0) {
        printf("  |   You have no %s subtasks.\n", statusName.c_str());
        printf("  |\n");
    } else {
        statusName[0] = statusName[0] - 'a' + 'A';
        printf("  |   %s:\n", statusName.c_str());
        printf("  |\n");
        for (auto it = tasks.begin(); it != tasks.end(); it++) {
            Task *task = *it;
            printf("  |    (%s) %s (%d)\n", task->code.c_str(), task->name.c_str(), countTodosTask(task));
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
    string statusNames[4] = {"active", "inactive", "completed", "canceled"};
    list<Task *> lists[4];
    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        Task *subtask = *it;
        int status = subtask->status;
        lists[status].push_back(subtask);
    }
    if ((lists[0].size() + lists[1].size() + lists[2].size() + lists[3].size()) == 0) {
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
        listStatusTasks(lists[i], statusNames[i]);
    }
    printf("  +------------------------------------------------------------------+\n\n");
}

/*
    getTaskTotalTime()
    Returns total period time of task pointed by 'task' and its subtasks.
*/
long int getTaskTotalTime(Task* task) {
    long int totalTime = 0;

    for (auto it = task->periods.begin(); it != task->periods.end(); it++) {
        Period *period = *it;
        totalTime += period->end - period->start;
    }

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        totalTime += getTaskTotalTime(*it);
    }

    return totalTime;
}

/*
    showTaskTotalTime();
    Prints total period time of task pointed by 'task' and all of its subtasks.
*/
void showTaskTotalTime(Task* task) {
    long int totalTime;
    string formatedTime;
    totalTime = getTaskTotalTime(task);
    formatedTime = formatDur(totalTime);
    printf("Time spent in task (%s) %s: %s.\n\n", task->code.c_str(), task->name.c_str(), formatedTime.c_str());
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

    for (auto it = task->periods.begin(); it != task->periods.end(); it++) {
        Period *period = *it;
        long int inicio = inicioSemana;
        long int timeThisWeek;
        if (period->start > inicio) inicio = period->start;
        timeThisWeek = period->end - inicio;
        if (timeThisWeek > 0) totalTime += timeThisWeek;
    }

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        totalTime += getTaskWeekTime(*it);
    }

    return totalTime;
}

/*
    showTaskWeekTime();
    Prints week period time of task pointed by 'task' and all of its subtasks.
*/
void showTaskWeekTime(Task* task) {
    long int totalTime;
    string formatedTime;
    totalTime = getTaskWeekTime(task);
    formatedTime = formatDur(totalTime);
    printf("Time spent in task (%s) %s this week: %s.\n\n", task->code.c_str(), task->name.c_str(), formatedTime.c_str());
}


/*
    renameTask()
    Renames task pointed by 'task' to name inputed by user.
*/
void renameTask(Task* task) {
    string taskName;
    taskName = getToken(1);
    if (taskName.size() == 0) {
        printf("Subtask name must not be empty.\n\n");
        return;
    } else if (task->parent == NULL) {
        printf("Can't rename main task.\n\n");
        return;
    } else if (findTaskByName(task->parent, taskName) != nullptr) {
        printf("\"%s\" already has subtask named \"%s\".\n\n", task->parent->name.c_str(), taskName.c_str());
        return;
    }
    printf("\"%s\" renamed to \"%s\"\n\n", task->name.c_str(), taskName.c_str());
    task->name = taskName;
}

/*
    renameTask()
    Changes code of task pointed by 'task' to code inputed by user.
*/
void changeCode(Task* task) {
    string code;
    code = toUppercase(getToken(1));
    if (code.size() == 0 || code.size() > 5) {
        printf("Subtask code must be 1-5 characters long.\n\n");
        return;
    } else if (task->parent == NULL) {
        printf("Can't change main task code.\n\n");
        return;
    } else if (findTaskByCode(task->parent, code) != nullptr) {
        printf("\"%s\" already has subtask with code \"%s\".\n\n", task->parent->code.c_str(), code.c_str());
        return;
    }
    printf("\"%s\" code changed to \"%s\"\n\n", task->code.c_str(), code.c_str());
    task->code = code;
}

/*
    selectSubtask()
    Saves address of subtask of 'parent' inputed by user.
*/
void selectSubtask(Task* parent) {
    Task *task = getSubtask(parent);

    if (task == nullptr) return;

    selectedTask = task;
    printf("Selected task (%s) %s.\n\n", selectedTask->code.c_str(), selectedTask->name.c_str());
}

/*
    moveTask()
    Moves selected task to 'parent' subtask list.
*/
void moveTask(Task* newParent) {
    if (selectedTask == nullptr) {
        printf("There's no selected task.\n\n");
        return;
    } else if (findTaskByCode(newParent, selectedTask->code) != nullptr) {
        printf("\"%s\" already has subtask with code \"%s\".\n\n", newParent->name.c_str(), selectedTask->code.c_str());
        return;
    } else if (findTaskByName(newParent, selectedTask->name) != nullptr) {
        printf("\"%s\" already has subtask named \"%s\".\n\n", newParent->name.c_str(), selectedTask->name.c_str());
        return;
    }

    printf("Moved task (%s) \"%s\" from (%s) to (%s)\n\n", selectedTask->code.c_str(), selectedTask->name.c_str(), selectedTask->parent->code.c_str(), newParent->code.c_str());

    /* Removes from old parent */
    selectedTask->parent->subtasks.remove(selectedTask);

    /* Adds in new parent */
    newParent->subtasks.push_back(selectedTask);
    
    selectedTask = nullptr;
}

/*
    allCompleted()
    Returns 1 if all subtasks of task pointed by 'task' are completed, and 0
    otherwise.
*/
bool allCompleted(Task* task) {
    bool res = true;
    for (auto it = task->subtasks.begin(); it != task->subtasks.end() && res; it++) {
        Task *subtask = *it;
        res = subtask->status == TASK_COMPLETED && allCompleted(subtask);
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
    subtask = getSubtask(task);
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
        printf("You can only complete \"%s\" when all its subtasks are completed.\n\n", subtask->name.c_str());
        return;
    }
    subtask->status = status;
    printf("\"%s\" is set to \"%s\"\n\n", subtask->name.c_str(), statusName);
}

/*
    addNote()
    Ask user for note content and add it in task pointed by 'task' notes.
*/
void addNote(Task *task) {
    string noteText;
    noteText = getToken(1);
    if (noteText.size() == 0) {
        printf("Note text must not be empty.\n\n");
        return;
    }
    Note *note = new Note;
    note->date = getCurrentTime();
    note->text = noteText;
    task->notes.push_back(note);
    printf("Added note\n\n");
}

/*
    removeNote()
    Ask user for note ID and remove it from task pointed by 'task'.
*/
void removeNote(Task* task) {
    int id = atoi(getToken(1)) - 1;

    Note *note = ithNote(task->notes, id);

    if (note == nullptr) return;

    printf("Note %d removed.\n\n", id + 1);

    task->notes.remove(note);
}

/*
    listNotes()
    Print list of notes of task pointed by 'task'.
*/
void listNotes(Task* task) {
    int i = 1;
    struct tm *stm;
    printf("  +--------------------------> Notes <--------------------------+\n");
    printf("  |\n");
    if (task->notes.size() == 0) {
        printf("  |   No notes\n");
    }
    for (auto it = task->notes.begin(); it != task->notes.end(); it++) {
        Note *note = *it;
        stm = localtime(&(note->date));
        printf("  |   %2d. (%02d/%02d/%04d) %s\n", i, stm->tm_mday, stm->tm_mon + 1, stm->tm_year + 1900, note->text.c_str());
        i++;
    }
    printf("  |\n");
    printf("  +------------------------------------------------------------------+\n\n");
}

/*
    getTasks()
    Inserts all TASK subtasks into tasks and the vector size into N.
*/
void getTasks(list<Task *> &tasks, Task *task) {
    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        tasks.push_back(*it);
        getTasks(tasks, *it);
    }
}

/*
    extractCodes()
    Extract codes from path into codes list.
*/
void extractCodes(string path, list<string> &codes) {
    stringstream pathstream;
    pathstream.str(path);
    while (!pathstream.eof()) {
        string code;
        pathstream >> code;
        codes.push_back(code);
        pathstream >> code;
    }
}

/*
    searchTask()
    Asks user for task suffix path and returns pointer to task.
*/
Task *searchTask(Task *root) {
    int i, id;
    list<Task *> tasks;
    list<Task *> results;
    list<string> inputCodes;
    int nInputs = getNComms() - 1;
    for (i = 1; i <= nInputs; i++) {
        inputCodes.push_back(toUppercase(getToken(i)));
    }

    tasks.push_back(root);

    getTasks(tasks, root);
    setUPath(root, root);

    for (auto it = tasks.begin(); it != tasks.end(); it++) {
        Task *task = *it;
        list<string> codes;

        extractCodes(task->uniquePath, codes);

        if (codes.size() < inputCodes.size()) continue;

        auto inputCode = inputCodes.rbegin();
        auto code = codes.rbegin();

        while (inputCode != inputCodes.rend()) {
            if (*inputCode != *code) break;
            inputCode++;
            code++;
        }

        if (inputCode != inputCodes.rend()) {
            continue;
        }

        results.push_back(task);
    }

    if (results.size() == 0) {
        printf("No task found.\n\n");
        return nullptr;
    }

    if (results.size() == 1) return results.front();

    printf("Multiple tasks match the path given:\n\n");

    i = 1;
    for (auto it = results.begin(); it != results.end(); it++) {
        Task *result = *it;
        printf("   %d. %s (%s)\n\n", i, result->name.c_str(), result->uniquePath.c_str());
        i++;
    }

    printf("Select desired task: ");
    scanf("%d", &id);
    getchar();
    printf("\n");

    return ithTask(results, id - 1);
}

/*
    subtasksMenu()
    Enters in subtask menu of task pointed by 'task'.
*/
void subtasksMenu(Task* task) {
    char *commandName;
    printf(" ________________________  Subtasks Menu (%s)  ________________________\n\n", task->code.c_str());
    listSubtasks(task);
    while (true) {
        printf(" ________________________  Subtasks Menu (%s)  ________________________\n\n", task->code.c_str());
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
            printf("***************************  Task Menu (%s)  ***************************\n\n", task->code.c_str());
            listSubtasks(task);
            showHead = false;
        }
        printf("***************************  Task Menu (%s)  ***************************\n\n", task->code.c_str());
        printf(" - %s\n\n", task->name.c_str());
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
                    Task *nextTask = getSubtask(task);
                    if (nextTask != NULL) {
                        curTask = nextTask;
                        return;
                    }
                }
            }
        } else if (generalCommands(commandName)) return;
    }
}
