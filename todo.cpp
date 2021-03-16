#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <stdlib.h>
#include "todo.hpp"
#include "io.hpp"
#include "util.hpp"
#include "help.hpp"
#include "calendar.hpp"

using namespace std;

/*
    Creates a to-do named 'name'.
*/
Todo* createTodo(string name, Task *task, Todo *parent) {
    Todo* newTodo = new Todo;
    newTodo->name = name;
    newTodo->status = TODO_PENDING;
    newTodo->parent = parent;
    newTodo->task = task;
    return newTodo;
}

/*
	freeTodo()
	Frees the memory allocated to 'todo' and its subtodos.
*/
void freeTodo(Todo *todo) {
    for (auto it = todo->schedules.begin(); it != todo->schedules.end(); it++) {
        delete *it;
    }

    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        freeTodo(*it);
	}
	
    delete todo;
}

/*
    Get todo from path in text.
*/
Todo *getTodoFromPath(Task *task, char *text, int *lastId) {
    char *token;
    int id;
    Todo *todo = nullptr;

    token = strtok(text, ".");
    id = atoi(token) - 1;

    todo = ithTodo(task->rootTodo->subtodos, id);

    if (todo == nullptr) return nullptr;
   
    while ((token = strtok(nullptr, ".")) != nullptr) {
        id = atoi(token) - 1;
        if (todo == nullptr) return nullptr;
        todo = ithTodo(todo->subtodos, id);
    }

    if (lastId != nullptr) *lastId = id;

    return todo;
}

/*
    Return the status name given the status enun id.
*/
string getTodoStatusName(TodoStatus status) {
    if (status == TODO_PENDING) return "pending";
    if (status == TODO_COMPLETED) return "completed";
    if (status == TODO_COMPLETED_HIDDEN) return "completed hidden";
    return "invalid status";
}

bool habitToday(Todo *todo) {
    time_t curDayStart = getPersonalDayStart(getCurrentTime());
    todo->periods.sort(periodComp);
    if (!todo->periods.empty() && todo->periods.back()->start >= curDayStart) return true;
    return false;
}

int countHabitRecord(Todo *todo) {
    int count = habitToday(todo); // this functions also sorts the periods 
    time_t curDayStart = getPersonalDayStart(getCurrentTime());
    Period *period;

    auto it = todo->periods.rbegin();

    while (it != todo->periods.rend()) {
        period = *it;
        if (period->start < curDayStart) break;
        it++;
    }

    while (it != todo->periods.rend()) {
        curDayStart -= SECS_IN_A_DAY;
        period = *it;
        if (period->start >= curDayStart) {
            count++;
            while (it != todo->periods.rend()) {
                period = *it;
                if (period->start < curDayStart) break;
                it++;
            }
        } else {
            break;
        }
    }
    return count;
}

/*
    Ask user for a to-do name and add it in task pointed by 'task' to-dos.
*/
void addTodo(Task *task) {
    string name;
    int type = getNComms() == 2 ? ROOT : NODE;
    
    name = type == ROOT ? getToken(1) : getToken(2);

    if (name.size() == 0) {
        printf("To-do name must not be empty.\n\n");
        return;
    }

    Todo *parent;

    if (type == ROOT) {
        parent = task->rootTodo;
    } else {
        parent = getTodoFromPath(task, getToken(1), nullptr);
        if (parent == nullptr) return;
    }

    Todo *todo = createTodo(name, task, parent);
    parent->subtodos.push_back(todo);
    
    printf("Added to-do \"%s\"\n\n", name.c_str());
}

/*
    Ask user the interval [a ... b] and name prefix to create to-dos
    [prefix a ... prefix b] in task pointed by 'task'.
*/
// void addTodoEsp(Task *task) {
//     int start;
//     int end;
//     char *name;
//     char copyName[NAME_LEN + 5];
//     int i;
//     sscanf(getToken(1), "%d-%d", &start, &end);
//     name = getToken(2);
//     if (strlen(name) == 0) {
//         printf("To-do name must not be empty.\n\n");
//         return;
//     }
//     for (i = start; i <= end; i++) {
//         sprintf(copyName, "%s %d", name, i);
//         task->todos[task->nTodos++] = createTodo(task, copyName, 0); desatualizado
//     }
//     printf("Added to-dos '%s #', # = [%d, %d]\n\n", name, start, end);
// }

/*
    Add all periods from 'todo' and from all of its descendants todo.
*/
void addPeriodsFromTodo(Todo *destTodo, Todo *todo) {
    for (auto it = todo->periods.begin(); it != todo->periods.end(); it++) {
        (*it)->todo = destTodo;
        destTodo->periods.push_back(*it);
    }

    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        addPeriodsFromTodo(destTodo, *it);
    }
}

/*
    Ask user for to-do ID and remove it from task pointed by 'task'.
*/
void removeTodo(Task* task) {
    int id;
    Todo *todo = getTodoFromPath(task, getToken(1), &id);

    if (todo == nullptr) return;

    if (calendar->periodSched != nullptr) {
        cout << "Não é possível remover tarefa enquanto um período está rodando\n\n";
        return;
    }

    list<Period *> periods;
    getPeriodsFromTodo(periods, todo);

    if (periods.size() > 0) {
        printf("Periods from to-do and their descendants will be moved to parent. Are you sure? ");

        string op;
        cin >> op;

        if (op != "y") return;
    }

    printf("To-do \"%s\" removed.\n\n", todo->name.c_str());

    addPeriodsFromTodo(todo->parent, todo);
    todo->parent->periods.sort(periodComp);
    todo->parent->subtodos.remove(todo);

    freeTodo(todo);
}

/*
    Sets the estimate of todo of 'task'.
*/
void editTodo(Task* task) {
    Todo* todo = getTodoFromPath(task, getToken(2), nullptr);
    string name;
    
    if (todo == nullptr) return;

    if (strcmp(getToken(1), "name") == 0) {
        name = getToken(3);
        if (name[0] == '\0') {
            printf("To-do name must not be empty.\n\n");
            return;
        }
        printf("Changed to-do name from %s to %s.\n\n", todo->name.c_str(), name.c_str());
        todo->name = name;

    }
}

/*
    Movo first to-do to before second to-do.
*/
void moveTodo(Task* task) {
    list<size_t> origPath;
    list<size_t> destPath;
    
    getIdPath(getToken(2), origPath);
    getIdPath(getToken(3), destPath);

    auto origMatch = origPath.begin();
    auto destMatch = destPath.begin();

    while (origMatch != origPath.end() && destMatch != destPath.end()) {
        if (*origMatch != *destMatch) break;
        origMatch++;
        destMatch++;
    }

    if (origMatch == origPath.end()) {
        cout << "Destination can't be inside origin\n" << endl;
        return;
    }

    if (origPath.size() == 0 || destPath.size() == 0) return;

    Todo *origParent = task->rootTodo;
    Todo *destParent = task->rootTodo;

    while (origPath.size() > 1) {
        origParent = ithTodo(origParent->subtodos, origPath.front() - 1);
        if (origParent == nullptr) return;
        origPath.pop_front();
    }

    while (destPath.size() > 1) {
        destParent = ithTodo(destParent->subtodos, destPath.front() - 1);
        if (destParent == nullptr) return;
        destPath.pop_front();
    }

    size_t origId = origPath.front() - 1;
    size_t destId = destPath.front() - 1;

    if (origId >= origParent->subtodos.size()) {
        cout << "Invalid origin ID.\n" << endl;    
        return;
    }

    if (destId >= destParent->subtodos.size() + 1) {
        cout << "Invalid destination ID.\n" << endl;    
        return;
    }

    auto origIt = origParent->subtodos.begin();
    auto destIt = destParent->subtodos.begin();

    while (origId--) origIt++;
    while (destId--) destIt++;

    Todo *orig = *origIt;
    string option = getToken(1);
    
    if (option == "before") {
        destParent->subtodos.insert(destIt, orig);
    } else if (option == "inside") {
        if (destIt == destParent->subtodos.end()) {
            cout << "Invalid destination ID.\n" << endl;    
            return;
        }
        orig->parent = *destIt; 
        (*destIt)->subtodos.push_back(orig);
    } else {
        cout << "Invalid option.\n" << endl;
        return;
    }

    origParent->subtodos.erase(origIt);

    cout << "Todo '" << orig->name << "' moved.\n" << endl;
}

bool descendentsCompleted(Todo *todo) {
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        Todo *todo = *it;
        if (!todoCompleted(todo) || !descendentsCompleted(todo)) return false;
    }
    return true;
}

bool changeTodoStatus(Todo *todo, int status) {
    bool toComplete = status == TODO_COMPLETED || status == TODO_COMPLETED_HIDDEN;

    if (toComplete) {
        if (!todoCompleted(todo) && !descendentsCompleted(todo)) {
            cout << "Para completar um to-do, todos seus filhos precisam estar completos.\n\n";
            return false;
        }
        for (auto it = todo->schedules.begin(); it != todo->schedules.end(); it++) {
            delete *it;
        }
        todo->schedules.clear();
    } else {
        Todo *parent = todo;
        while (parent->parent != nullptr) {
            if (todoCompleted(parent)) parent->status = TODO_PENDING;
            parent = parent->parent;
        }
        if (todoCompleted(parent)) parent->status = TODO_PENDING;
        if (status == TODO_PENDING) todo->schedules.clear();
    }

    todo->status = status;
    return true;
}

/*
    Changes status of to-do.
*/
void setTodoStatus(Task *task) {
    TodoStatus status;
    Todo *todo;
    size_t start, end;
    int mult = strcmp(getToken(1), "esp") == 0;

    if (mult && getNComms() == 3) todo = task->rootTodo;
    else todo = getTodoFromPath(task, getToken(1 + mult), nullptr);
    
    if (todo == nullptr) return;

    if (strcmp(getToken(0), "clear") == 0) status = TODO_PENDING;
    else if (strcmp(getToken(0), "complete") == 0) status = TODO_COMPLETED;
    else if (strcmp(getToken(0), "hide") == 0) status = TODO_COMPLETED_HIDDEN;
    else status = TODO_HABIT; // because of the menu condition, getToken(0) must be "habit"

    if (mult) {
        sscanf(getToken(getNComms() - 1), "%ld-%ld", &start, &end);
        start--;
        end--;
        if (start > end || start < 0 || end >= todo->subtodos.size()) {
            printf("Invalid interval.\n\n");
            return;
        }
        
        size_t i = 0;
        auto it = todo->subtodos.begin();

        while (i < start) {
            it++;
            i++;
        }

        while (i <= end) {
            if (!changeTodoStatus(*it, status)) return;
            it++;
            i++;
        }

        printf("To-dos %ld-%ld, status changed to \"%s\".\n\n", start + 1, end + 1, getTodoStatusName(status).c_str());
    } else {
        if (!changeTodoStatus(todo, status)) return;
        printf("\"%s\" is set to \"%s\".\n\n", todo->name.c_str(), getTodoStatusName(status).c_str());
    }
}

/*
    Create a schedule for todo.
*/
void createSchedule(Todo *todo, int timeSet, time_t date, int estimate) {
    Schedule *sched = new Schedule;
    sched->timeSet = timeSet;
    sched->date = date;
    sched->todo = todo;
    sched->timeEstimate = estimate;
    todo->schedules.push_back(sched);
}

void scheduleTodo(Task *task) {
    int day, mon, hour, min;
    Todo *todo;
    time_t date;
    struct tm *structTime;
    int args;
    int estimate;
    
    todo = getTodoFromPath(task, getToken(1), nullptr);

    if (todo == nullptr) return;

    args = getNComms();

    if (args != 2) {
        sscanf(getToken(2), "%d/%d", &day, &mon);

        date = getCurrentTime();
        structTime = localtime(&date);

        if (structTime->tm_mon > mon) structTime->tm_year++;
        structTime->tm_sec = 0;
        structTime->tm_mon = mon - 1;
        structTime->tm_mday = day;
        if (args == 5) {
            sscanf(getToken(3), "%d:%d", &hour, &min);
        } else if (args == 4) {
            hour = min = 0;
        }
        structTime->tm_hour = hour;
        structTime->tm_min = min;
        estimate = 60 * atof(getToken(args - 1));
        date = mktime(structTime);
        printf("To-do date scheduled to %02d/%02d/%04d.\n\n", day, mon, 1900 + structTime->tm_year);
    } else {
        estimate = 0;
        date = 0;
        printf("Todo added to calendar.\n\n");
    }

    createSchedule(todo, args == 5, date, estimate);


}

/*
    Print to-do tree of to-do pointed by 'todo'.
*/
void printTodoTree(Todo* todo, list<int> &path, bool showHidden) {
    int i;
    string status;

    if (todo->status == TODO_HABIT) status = "h";
    else if (todo->schedules.size() != 0) status = "*";
    else if (todo->status == TODO_PENDING) status = " ";
    else if (todo->status == TODO_COMPLETED) status = "x";
    else if (showHidden) status = "X";
    else return;

    stringstream todoLine;

    todoLine << "    ";
    for (auto it = path.begin(); it != path.end(); it++) {
        if (it == path.begin()) continue;
        cout << "   ";
    }

    time_t timeSpentThisTodo = getTodoTime(todo);
    time_t timeSpentTotal = getTodoTotalTime(todo);

    todoLine << getColor(BRIGHT_BLUE) << setw(2) << path.back() << getColor(BRIGHT_WHITE);
    todoLine << colorString(" [", BRIGHT_CYAN) << colorString(status, YELLOW) << colorString("] ", BRIGHT_CYAN);
    todoLine << todo->name;
    todoLine << " (" << setprecision(1) << fixed << timeSpentThisTodo / 3600.0 << "h";
    if (timeSpentThisTodo == timeSpentTotal) todoLine << ")";
    else todoLine << " | " << setprecision(1) << fixed << timeSpentTotal / 3600.0 << "h)";
    
    cout << todoLine.str() << "\n";

    i = 1;
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        path.push_back(i);
        printTodoTree(*it, path, showHidden);
        path.pop_back();
        i++;
    }
}

/*
    Print list of to-dos of task pointed by 'task'.
*/
void listTodos(Task* task, bool showHidden) {
    if (task->rootTodo->subtodos.size() == 0) {
        printf("    Não há to-dos.\n");
    } else {
        list<int> path;
        int i = 1;
        for (auto it = task->rootTodo->subtodos.begin(); it != task->rootTodo->subtodos.end(); it++) {
            path.push_back(i);
            printTodoTree(*it, path, showHidden);
            path.pop_back();
            i++;
        }
    }
    cout << "\n";
}

/*
    Enters in to-do menu of task pointed by 'task'.
*/
void todosMenu(Task* task) {
    char *commandName;
    list<string> todoStatusCommands = {"clear", "complete", "hide", "habit"};
    
    printTitle("To-dos - " + task->code, MAIN_LEVEL);
    listTodos(task, 0);
    while (true) {
        printTitle("To-dos - " + task->code, MAIN_LEVEL);
        commandName = getCommandName();
        periodWarning();
        if (strcmp(commandName, "add") == 0) {
            if (getNComms() == 2 || getNComms() == 3) {
                addTodo(task);
                listTodos(task, 0);
                saveAll();
            } else {
                printf("Número inválido de argumentos.\n");
            }
        // } else if (strcmp(commandName, "addesp") == 0) {
        //     if (validArgs(2)) {
        //         addTodoEsp(task);
        //      listTodos(task, 0);
        //         saveAll();
        //     }
        } else if (strcmp(commandName, "rem") == 0) {
            if (validArgs(1)) {
                removeTodo(task);
                listTodos(task, 0);
                saveAll();
            }
        } else if (strcmp(commandName, "edit") == 0) {
            if (validArgs(3)) {
                editTodo(task);
                listTodos(task, 0);
                saveAll();
            }
        } else if (strcmp(commandName, "move") == 0) {
            if (validArgs(3)) {
                moveTodo(task);
                listTodos(task, 0);
                saveAll();
            }
        } else if (strcmp(commandName, "sched") == 0) {
            if (getNComms() == 2 || getNComms() == 4 || getNComms() == 5) {
                scheduleTodo(task);
                listTodos(task, 0);
                saveAll();
            } else {
                printf("Número inválido de argumentos.\n\n");
            }
        } else if (strcmp(commandName, "tds") == 0) {
            if (getNComms() == 1) {
                listTodos(task, false);
            } else if (getNComms() == 2) {
                if (strcmp(getToken(1), "all") == 0) {
                    listTodos(task, true);
                } else {
                    printf("Argumento inválido.\n\n");
                }
            } else {
                printf("Número inválido de argumentos.\n");
            }
        } else if (isInList(commandName, todoStatusCommands)) {
            if (getNComms() >= 2 || getNComms() <= 4) {
                setTodoStatus(task);
                listTodos(task, 0);
                saveAll();
            } else {
                printf("Número inválido de argumentos.\n");
            }
        } else if (strcmp(commandName, "cd") == 0) {
            if (validArgs(1)) {
                if (strcmp("..", getToken(1)) == 0) {
                    curMenu = TASK_MENU;
                    return;
                } else {
                    printf("Digite 'cd ..' para voltar\n\n");
                }
            }
        } else if (generalCommands(commandName)) return;
    }
}
