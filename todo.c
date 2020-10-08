#include <string.h>
#include <stdlib.h>
#include "todo.h"
#include "io.h"
#include "util.h"
#include "help.h"
#include "calendar.h"

/*
    createTodo()
    Creates a to-do named 'name' with 'type'.
*/
Todo* createTodo(char *name, int type) {
    Todo* newTodo;
    newTodo = (Todo *) mallocSafe(sizeof(Todo));
    strcpy(newTodo->name, name);
    newTodo->timeSpent = 0;
    newTodo->timeEstimate = 0;
    newTodo->status = TODO_PENDING;
    newTodo->type = type;
    newTodo->nSubtodos = 0;
    newTodo->nSchedules = 0;
    return newTodo;
}

/*
	freeTodo()
	Frees the memory allocated to 'todo' and its subtodos.
*/
void freeTodo(Todo *todo) {
	int i;
    for (i = 0; i < todo->nSchedules; i++) {
        free(todo->schedules[i]);
    }
	for (i = 0; i < todo->nSubtodos; i++) {
		freeTodo(todo->subtodos[i]);
	}
	free(todo);
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
    if (id < 0 || id >= task->nTodos) {
        printf("Invalid to-do ID.\n\n");
        return NULL;
    }
    
    todo = task->todos[id];
   
    while ((token = strtok(NULL, ".")) != NULL) {
        id = atoi(token) - 1;
        if (id < 0 || id >= todo->nSubtodos) {
            printf("Invalid to-do ID.\n\n");
            return NULL;
        }
        todo = todo->subtodos[id];
    }

    if (lastId != NULL) *lastId = id;

    return todo;
}

/*
    addTodo()
    Ask user for a to-do name and add it in task pointed by 'task' to-dos.
*/
void addTodo(Task *task) {
    char *name;
    int type = getNComms() == 2 ? ROOT : NODE;
    
    name = type == ROOT ? getToken(1) : getToken(2);

    if (name[0] == '\0') {
        printf("To-do name must not be empty.\n\n");
        return;
    }

    if (type == ROOT) {
        task->todos[task->nTodos] = createTodo(name, type);
        task->todos[task->nTodos]->parent.task = task;
        task->nTodos++;
    } else {
        Todo *todo = getTodoFromPath(task, getToken(1), NULL);
        if (todo == NULL) return;
        todo->subtodos[todo->nSubtodos] = createTodo(name, type);
        todo->subtodos[todo->nSubtodos]->parent.todo = todo;
        todo->nSubtodos++;
    }

    printf("Added to-do \"%s\"\n\n", name);
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

    printf("To-do \"%s\" removed.\n\n", todo->name);

    if (todo->type == ROOT) {
        Task *parent = todo->parent.task;
        parent->nTodos--;
        while (id < parent->nTodos) {
            parent->todos[id] = parent->todos[id + 1];
            id++;
        }
    } else {
        Todo *parent = todo->parent.todo;
        parent->nSubtodos--;
        while (id < parent->nSubtodos) {
            parent->subtodos[id] = parent->subtodos[id + 1];
            id++;
        }
    }
    freeTodo(todo);
}

/*
    setEstimate()
    Sets the estimate of todo of 'task'.
*/
void setEstimate(Task* task) {
    Todo* todo = getTodoFromPath(task, getToken(1), NULL);
    
    if (todo == NULL) return;

    todo->timeEstimate = 60 * atof(getToken(2));
}

/*
    setTodoStatus()
    Changes status of to-do.
*/
void setTodoStatus(Task *task) {
    char *statusName;
    Todo *todo = getTodoFromPath(task, getToken(1), NULL);   

    if (todo == NULL) return;

    int status;
    statusName = toLowercase(getToken(2));
    
    if (strcmp(statusName, "pending") == 0) {
        status = TODO_PENDING;
    } else if (strcmp(statusName, "priority") == 0) {
        status = TODO_PRIORITY;
    } else {
        printf("\"%s\" is not a valid status.\n\n", statusName);
        return;
    }

    todo->status = status;
    printf("\"%s\" is set to \"%s\"\n\n", todo->name, statusName);
}

/*
    printTodoTree()
    Print to-do tree of to-do pointed by 'todo'.
*/
void printTodoTree(Todo* todo, int level, int id) {
    int i;
    printf("  |   ");
    for (i = 0; i < level; i++) {
        printf("  ");
    }
    printf("%2d %s", id, todo->name);
    if (todo->timeEstimate != 0) {
        printf(" (%.1f/%.1f)", todo->timeSpent / 60.0, todo->timeEstimate / 60.0);
    }
    if (todo->status == TODO_PRIORITY) {
        printf(" *");
    }
    printf("\n");
    for (i = 0; i < todo->nSubtodos; i++) {
        printTodoTree(todo->subtodos[i], level + 1, i + 1);
    }
}

/*
    listTodos()
    Print list of to-dos of task pointed by 'task'.
*/
void listTodos(Task* task) {
    int i;
    printf("  +--------------------------> To-do list <--------------------------+\n");
    printf("  |\n");
    if (task->nTodos == 0) {
        printf("  |   List is empty\n");
    }
    for (i = 0; i < task->nTodos; i++) {
        printTodoTree(task->todos[i], 0, i + 1);
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
        periodWarning();
        if (strcmp(commandName, "add") == 0) {
            if (getNComms() == 2 || getNComms() == 3) {
                addTodo(task);
                listTodos(task);
                saveAll();
            } else {
                printf("Invalid number of arguments.\n");
            }
        // } else if (strcmp(commandName, "addesp") == 0) {
        //     if (validArgs(2)) {
        //         addTodoEsp(task);
        //      listTodos(task);
        //         saveAll();
        //     }
        } else if (strcmp(commandName, "rem") == 0) {
            if (validArgs(1)) {
                removeTodo(task);
                listTodos(task);
                saveAll();
            }
        } else if (strcmp(commandName, "tds") == 0) {
            if (validArgs(0)) listTodos(task);
        } else if (strcmp(commandName, "set") == 0) {
            if (validArgs(2)) {
                setTodoStatus(task);
                listTodos(task);
                saveAll();
            }
        } else if (strcmp(commandName, "estimate") == 0) {
            if (validArgs(2)) {
                setEstimate(task);
                listTodos(task);
                saveAll();
            }
        // } else if (strcmp(commandName, "setesp") == 0) {
        //     if (validArgs(2)) {
        //         changeTodoDateEsp(task);
        //         listTodos(task);
        //         saveAll();
        //     }
        } else if (strcmp(commandName, "cal") == 0) {
            if (validArgs(0)) {
                calendarMenu();
                listTodos(task);
            }
        } else if (strcmp(commandName, "week") == 0) {
            if (validArgs(0)) printWeekSummary(rootTask);
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
