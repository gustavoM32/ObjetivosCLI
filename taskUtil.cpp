#include <sstream>
#include <cstring>
#include "io.hpp"
#include "util.hpp"
#include "taskUtil.hpp"
#include "todo.hpp"

using namespace std;

/*
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
    newTask->status = TASK_ACTIVE;
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
    if (tasks.size() == 0) {
        printf("  |   You have no %s subtasks.\n", statusName.c_str());
        printf("  |\n");
    } else {
        statusName[0] = statusName[0] - 'a' + 'A';
        printf("  |   %s:\n", statusName.c_str());
        printf("  |\n");
        for (auto it = tasks.begin(); it != tasks.end(); it++) {
            Task *task = *it;
            printf("  |    (%s) %s", task->code.c_str(), task->name.c_str());
            int todos = countTodosTask(task);
            if (todos != 0) printf(" (%d)", todos);
            printf("\n");
        }
        printf("  |\n");
    }
}

/*
    Returns total period time of task pointed by 'task' and its subtasks.
*/
time_t getTaskTotalTime(Task* task, int option) {
    time_t totalTime = 0;

    totalTime += getTodoTotalTime(task->rootTodo, option);

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        totalTime += getTaskTotalTime(*it, option);
    }

    return totalTime;
}

/*;
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

/*;
    Prints week period time of task pointed by 'task' and all of its subtasks.
*/
void showTaskWeekTime(Task* task) {
    // long int totalTime;
    // string formatedTime;
    // totalTime = getTaskWeekTime(task);
    // formatedTime = formatDur(totalTime);
    // printf("Time spent in task (%s) %s this week: %s.\n\n", task->code.c_str(), task->name.c_str(), formatedTime.c_str());
}


/*
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
        printf("   ");
    }

    printf("* ");
    if (onlyIncomplete && task->status == TASK_INACTIVE) printf("(inactive) ");
    if (!onlyActive && !onlyIncomplete) {
        if (task->status == TASK_INACTIVE) printf("(inactive) ");
        else if (task->status == TASK_CANCELED) printf("(canceled) ");
        else if (task->status == TASK_COMPLETED) printf("(completed) ");
    }
    printf("%s - %s\n", task->code.c_str(), task->name.c_str());

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        printTaskTreeRec(*it, depth + 1, onlyActive, onlyIncomplete);
    }

}

/*
    Print all tasks in a tree structure.
*/
void printTaskTree() {
    if (getNComms() == 1) printTaskTreeRec(rootTask, 0, true, false);
    else if (strcmp(getToken(1), "incomplete") == 0) printTaskTreeRec(rootTask, 0, false, true);
    else if (strcmp(getToken(1), "all") == 0) printTaskTreeRec(rootTask, 0, false, false);
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