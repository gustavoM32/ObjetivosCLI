#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include "io.hpp"
#include "util.hpp"
#include "taskUtil.hpp"
#include "todo.hpp"

using namespace std;

/*
    Returns index of pointer to subtask named 'name' in subtask vector of task
    'parent'. Returns nullptr if there's no such subtask.
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
    Returns index of pointer to subtask with code 'code' in subtask vector of task
    'parent'. Returns nullptr if there's no such subtask.
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
    Creates task named 'name' and with code 'code' and returns a pointer to it.
*/
Task* createTask(string name, string code) {
    Task* newTask = new Task;
    newTask->name = name;
    newTask->code = code;
    newTask->plan = "";
    newTask->status = TASK_ACTIVE;
    newTask->color = "NONE";
    newTask->rootTodo = createTodo("To-dos", newTask, nullptr);
    newTask->parent = nullptr;
    return newTask;
}

/*
	freeTask()
	Frees the memory allocated to 'task' and its substasks.
*/
void freeTask(Task *task) {
    freeTodo(task->rootTodo);

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        freeTask(*it);
    }

    delete task;
}

void listStatusTasks(list<Task *> tasks, string statusName) {
    if (tasks.size() > 0) {
        string color;
        if (statusName == "active") color = "CYAN";
        else if (statusName == "inactive") color = "BRIGHT_BLACK";
        else if (statusName == "completed") color = "GREEN";
        else if (statusName == "canceled") color = "RED";

        for (auto it = tasks.begin(); it != tasks.end(); it++) {
            Task *subtask = *it;
            stringstream subtaskName;
            subtaskName << subtask->code;
            int todos = countTodosTask(subtask);
            if (todos != 0) {
                subtaskName << " (" << todos << ")";
            }
            cout << colorString(subtaskName.str(), color);
            cout << "    ";
        }
    }
}

/*
    Returns total period time of task pointed by 'task' and its subtasks.
*/
time_t getTaskTotalTime(Task* task) {
    time_t totalTime = 0;

    totalTime += getTodoTotalTime(task->rootTodo);

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        totalTime += getTaskTotalTime(*it);
    }

    return totalTime;
}

/*
    Returns week period time of task pointed by 'task' and its subtasks.
*/
// long int getTaskWeekTime(Task* task) {
//     long int curTime = getCurrentTime();
//     long int objStart = getTime(25, 8, 2019, 0, 0, 0);
//     long int weekProgress = (curTime - objStart) % SECS_IN_A_WEEK;
//     long int inicioSemana = curTime - weekProgress;
//     long int totalTime = 0;

//     for (auto it = task->periods.begin(); it != task->periods.end(); it++) {
//         Period *period = *it;
//         long int inicio = inicioSemana;
//         long int timeThisWeek;
//         if (period->start > inicio) inicio = period->start;
//         timeThisWeek = period->end - inicio;
//         if (timeThisWeek > 0) totalTime += timeThisWeek;
//     }

//     for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
//         totalTime += getTaskWeekTime(*it);
//     }

//     return totalTime;
// }

/*
    Returns 1 if all subtasks of task pointed by 'task' are completed, and 0
    otherwise.
*/
bool allCompleted(Task* task) {
    bool res = true;
    for (auto it = task->subtasks.begin(); it != task->subtasks.end() && res; it++) {
        Task *subtask = *it;
        res = (subtask->status == TASK_COMPLETED || subtask->status == TASK_CANCELED) && allCompleted(subtask);
    }
    return res;
}

/*
    Searches recursively for task with 'code' among 'task' descendants
*/
Task *searchTaskRec(Task *task, string &code) {
    if (task->code == code) return task;

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        Task *subtask = searchTaskRec(*it, code);
        if (subtask != nullptr) return subtask;
    }

    return nullptr;
}

/*
    Asks user for task code and searches among all tasks.
*/
Task *searchTask() {
    string code = toUppercase(getToken(1));
    Task *task = searchTaskRec(rootTask, code);

    if (task == nullptr) {
        printf("No task found.\n\n");
        return nullptr;
    }

    return task;
}

/*
    Print task in a tree structure.
*/
void printTaskTreeRec(Task *task, int depth, bool onlyActive, bool onlyIncomplete) {
    if (onlyActive && task->status != TASK_ACTIVE) return;
    if (onlyIncomplete && task->status == TASK_COMPLETED) return;

    for (int i = 0; i < depth; i++) {
        cout << "   ";
    }

    cout << getColor(getTaskColor(task)) << "* ";
    if (onlyIncomplete && task->status == TASK_INACTIVE) cout << "(inactive) ";
    if (!onlyActive && !onlyIncomplete) {
        if (task->status == TASK_INACTIVE) cout << "(inactive) ";
        else if (task->status == TASK_CANCELED) cout << "(canceled) ";
        else if (task->status == TASK_COMPLETED) cout << "(completed) ";
    }

    cout << task->code << " - " << task->name << "\n";

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        printTaskTreeRec(*it, depth + 1, onlyActive, onlyIncomplete);
    }
    cout << getColor("BRIGHT_WHITE");
}

/*
    Print all tasks in a tree structure.
*/
void printTaskTree() {
    if (getNComms() == 1) printTaskTreeRec(rootTask, 0, true, false);
    else if (getToken(1) == "incomplete") printTaskTreeRec(rootTask, 0, false, true);
    else if (getToken(1) == "all") printTaskTreeRec(rootTask, 0, false, false);
    else printf("Invalid option.\n");

    printf("\n");
}

/*
    Return true if 'code' is already being used in 'task' or its descendants.
*/
bool codeUsed(Task *task, string &code) {
    if (task->code == code) return true;

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        if (codeUsed(*it, code)) return true;
    }

    return false;
}

Task *getColorRoot(Task *task) {
    while (task != nullptr) {
        if (task->color != "NONE") return task;
        task = task->parent;
    }
    return nullptr;
}

string getTaskColor(Task *task) {
    task = getColorRoot(task);
    if (task != nullptr) return task->color;
    return DEFAULT_TASK_COLOR;
}

string getMotivation(Task *task) {
    for (auto it = task->history.rbegin(); it != task->history.rend(); it++) {
        if ((*it)->motivation) return (*it)->text + colorString(" (" + formatDate((*it)->date, true) + ")", "BRIGHT_CYAN");
    }
    return "Indefinida";
}

void printRecentHistory(Task* task) {
    cout << colorString("  Histórico recente: ", "BRIGHT_BLUE");
    if (task->history.size() == 0) {
        printf("Sem histórico\n\n");
        return;
    }

    cout << "\n";

    auto it = task->history.begin();

    for (int i = 0; i < (int) task->history.size() - 3; i++) {
        it++;
    }

    while (it != task->history.end()) {
        Note *note = *it;
        cout << colorString("  * ", "BRIGHT_BLUE") << colorString("(" + formatDate(note->date, true) + ") ", "BRIGHT_CYAN");
        if (note->motivation) cout << "(Motivação) ";
        cout << note->text << "\n";
        it++;
    }
    cout << "\n";
}