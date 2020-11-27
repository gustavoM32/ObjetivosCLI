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
    createTodo()
    Creates a to-do named 'name' with 'type'.
*/
Todo* createTodo(string name, int type) {
    Todo* newTodo = new Todo;
    newTodo->name = name;
    newTodo->timeSpent = 0;
    newTodo->timeEstimate = 0;
    newTodo->status = TODO_PENDING;
    newTodo->type = type;
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
    getTodoFromPath()
    Get todo from path in text.
*/
Todo *getTodoFromPath(Task *task, char *text, int *lastId) {
    char *token;
    int id;
    Todo *todo = NULL;

    token = strtok(text, ".");
    id = atoi(token) - 1;

    todo = ithTodo(task->todos, id);

    if (todo == nullptr) return nullptr;
   
    while ((token = strtok(NULL, ".")) != NULL) {
        id = atoi(token) - 1;
        if (todo == nullptr) return nullptr;
        todo = ithTodo(todo->subtodos, id);
    }

    if (lastId != nullptr) *lastId = id;

    return todo;
}

/*
    addTodo()
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

    if (type == ROOT) {
        Todo *todo = createTodo(name, type);
        task->todos.push_back(todo);
        todo->parent.task = task;
    } else {
        Todo *todo = getTodoFromPath(task, getToken(1), nullptr);
        if (todo == nullptr) return;
        todo = createTodo(name, type);
        task->todos.push_back(todo);
        todo->parent.todo = todo;
    }

    printf("Added to-do \"%s\"\n\n", name.c_str());
}

/*
    addTodoEsp()
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
    removeTodo()
    Ask user for to-do ID and remove it from task pointed by 'task'.
*/
void removeTodo(Task* task) {
    int id;
    Todo *todo = getTodoFromPath(task, getToken(1), &id);

    if (todo == NULL) return;

    printf("To-do \"%s\" removed.\n\n", todo->name.c_str());

    if (todo->type == ROOT) {
        Task *parent = todo->parent.task;
        parent->todos.remove(todo);
    } else {
        Todo *parent = todo->parent.todo;
        parent->subtodos.remove(todo);
        parent->timeSpent += todo->timeSpent;
    }

    freeTodo(todo);
}

/*
    editTodo()
    Sets the estimate of todo of 'task'.
*/
void editTodo(Task* task) {
    Todo* todo = getTodoFromPath(task, getToken(2), NULL);
    string name;
    
    if (todo == NULL) return;

    if (strcmp(getToken(1), "estimate") == 0) {
        int estimate = atof(getToken(3));
        if (estimate < 0) {
            printf("Estimate can't be negative.\n\n");
            return;
        }  
        todo->timeEstimate = 60 * estimate;
        printf("Changed to-do '%s' time estimate to %d.\n\n", todo->name.c_str(), estimate);
    } else if (strcmp(getToken(1), "name") == 0) {
        name = getToken(3);
        if (name[0] == '\0') {
            printf("To-do name must not be empty.\n\n");
            return;
        }
        printf("Changed to-do name from %s to %s.\n\n", todo->name.c_str(), name.c_str());
        todo->name = name;

    }
}

bool descendentsCompleted(Todo *todo) {
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        Todo *todo = *it;
        if (todo->status != TODO_COMPLETED || !descendentsCompleted(todo)) return false;
    }
    return true;
}

bool changeTodoStatus(Todo *todo, int status) {
    if (status == TODO_COMPLETED) {
        if (!descendentsCompleted(todo)) {
            printf("All sub to-dos must be completed before to-do can be set as completed.\n\n");
            return false;
        }
    } else if (status == TODO_PENDING) {
        Todo *parent = todo;
        while (parent->type != ROOT) {
            if (parent->status == TODO_COMPLETED) parent->status = TODO_PENDING;
            parent = todo->parent.todo;
        }
        if (parent->status == TODO_COMPLETED) parent->status = TODO_PENDING;
        for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
            Todo *todo = *it;
            if (todo->status == TODO_PRIORITY) todo->status = TODO_PENDING;
        }
    } else if (status == TODO_PRIORITY) {
        for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
            Todo *todo = *it;
            if (todo->status == TODO_PENDING) todo->status = TODO_PRIORITY;
        }
    }

    todo->status = status;
    return true;
}

/*
    setTodoStatus()
    Changes status of to-do.
*/
void setTodoStatus(Task *task) {
    char *statusName;
    int status;
    Todo *todo;
    size_t start, end;
    int mult = (getNComms() == 3);

    todo = getTodoFromPath(task, getToken(1), NULL);
    
    if (todo == NULL) return;

    if (strcmp(getToken(0), "unset") == 0) {
        status = TODO_PENDING;
        statusName = "pending";
    } else if (strcmp(getToken(0), "set") == 0) {
        status = TODO_PRIORITY;
        statusName = "prioritized";
    } else {
        status = TODO_COMPLETED;
        statusName = "completed";
    }

    if (mult) {
        sscanf(getToken(2), "%ld-%ld", &start, &end);
        start--;
        end--;
        if (start > end || start < 0 || end >= todo->subtodos.size()) {
            printf("Invalid interval.\n\n");
            return;
        }
        
        size_t i = 0;
        for (auto it = todo->subtodos.begin(); i <= end; it++) {
            if (!changeTodoStatus(*it, status)) return;
            i++;
        }

        printf("To-dos %ld-%ld, status changed to \"%s\".\n\n", start + 1, end + 1, statusName);
    } else {
        if (!changeTodoStatus(todo, status)) return;
        printf("\"%s\" is set to \"%s\".\n\n", todo->name.c_str(), statusName);
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
    sched->timeSpent = 0;
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
    
    todo = getTodoFromPath(task, getToken(1), NULL);

    if (todo == NULL) return;

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
    printTodoTree()
    Print to-do tree of to-do pointed by 'todo'.
*/
void printTodoTree(Todo* todo, int level, int id, int showCompleted) {
    int i;
    char status;

    if (todo->status == TODO_PENDING) status = ' ';
    else if (todo->status == TODO_PRIORITY) status = '*';
    else status = 'x';

    printf("  |   ");
    for (i = 0; i < level; i++) {
        printf("  ");
    }
    printf("%2d [%c] %s", id, status, todo->name.c_str());
    if (todo->timeSpent != 0) printf(" (%.1fh)", todo->timeSpent / 60.0);
    if (todo->schedules.size() != 0) printf(" (%ld schedules)", todo->schedules.size());
    printf("\n");

    if (!showCompleted && todo->status == TODO_COMPLETED) return;

    i = 1;
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        printTodoTree(*it, level + 1, i, showCompleted);
        i++;
    }
}

/*
    listTodos()
    Print list of to-dos of task pointed by 'task'.
*/
void listTodos(Task* task, int showCompleted) {
    int i;
    printf("  +--------------------------> To-do list <--------------------------+\n");
    printf("  |\n");
    if (task->todos.size() == 0) {
        printf("  |   List is empty\n");
    }

    i = 1;
    for (auto it = task->todos.begin(); it != task->todos.end(); it++) {
        printTodoTree(*it, 0, i, showCompleted);
        i++;
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
    list<string> todoStatusCommands = {"set", "unset", "complete"};

    printf(" _________________________  To-dos Menu (%s)  _________________________\n\n", task->code.c_str());
    listTodos(task, 0);
    while (true) {
        printf(" _________________________  To-dos Menu (%s)  _________________________\n\n", task->code.c_str());
        commandName = getCommandName();
        periodWarning();
        if (strcmp(commandName, "add") == 0) {
            if (getNComms() == 2 || getNComms() == 3) {
                addTodo(task);
                listTodos(task, 0);
                saveAll();
            } else {
                printf("Invalid number of arguments.\n");
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
        } else if (strcmp(commandName, "sched") == 0) {
            if (getNComms() == 2 || getNComms() == 4 || getNComms() == 5) {
                scheduleTodo(task);
                listTodos(task, 0);
            } else {
                printf("Invalid number of arguments.\n\n");
            }
        } else if (strcmp(commandName, "tds") == 0) {
            if (getNComms() == 1) {
                listTodos(task, 0);
            } else if (getNComms() == 2) {
                if (strcmp(getToken(1), "all") == 0) {
                    listTodos(task, 1);
                } else {
                    printf("Invalid argument.\n\n");
                }
            } else {
                printf("Invalid number of arguments.\n");
            }
        } else if (isInList(commandName, todoStatusCommands)) {
            if (getNComms() == 2 || getNComms() == 3) {
                setTodoStatus(task);
                listTodos(task, 0);
                saveAll();
            } else {
                printf("Invalid number of arguments.\n");
            }
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
