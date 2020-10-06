#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "io.h"
#include "util.h"
#include "task.h"

#define SENTENCE_LIMIT '\"'

static int nComms = 0;
static char buffer[MAX_COMMANDS][COMMAND_LEN];

/*
    getNComms()
    Returns the number of read commands;
*/
int getNComms() {
    return nComms;
}

/*
    getLine()
    Reads a line from stream and separate it into words
    and "sentences with spaces" and stores it into BUFFER.
*/
int getLine(FILE *stream) {
    char c;
    int inCommand = 0;
    int inSpaced = 0;
    int commN = 0;
    nComms = 0;
    while ((c = getc(stream)) != '\n') {
        if (!inCommand) {
            if (c == SENTENCE_LIMIT) {
                inCommand = 1;
                inSpaced = 1;
            } else if (c != ' ') {
                inCommand = 1;
                buffer[nComms][commN++] = c;
            }
        } else {
            if (inSpaced && c == SENTENCE_LIMIT) {
                inCommand = 0;
                inSpaced = 0;
                buffer[nComms++][commN] = '\0';
                commN = 0;
            } else if (!inSpaced && c == ' ') {
                inCommand = 0;
                buffer[nComms++][commN] = '\0';
                commN = 0;
            } else {
                buffer[nComms][commN++] = c;
            }
        }
    }
    if (inSpaced) {
        return 1;
    }
    if (inCommand) {
        buffer[nComms++][commN] = '\0';
    }
    return 0;
}

/*
    invalidArgs()
    Returns true if num of arguments in buffer equals NARGS.
    Returns false otherwise and a error message.
*/
int validArgs(int nArgs) {
    if (nComms - 1 != nArgs) {
        if (nArgs == 0) printf("Command accepts no arguments.\n\n");
        else if (nArgs == 1) printf("Command accepts 1 argument.\n\n");
        else printf("Command accepts %d arguments.\n\n", nArgs);

        return 0;
    }
    return 1;
}

/*
    getToken()
    Returns a string from buffer given its ID.
*/
char *getToken(int id) {
    if (id >= nComms) return NULL;
    return buffer[id];
}

/*
    getCommandName()
    Prompts user for a command until its name is not empty.
*/
char *getCommandName() {
    char *commandName = NULL;
    char *line;
    FILE *stream;
    while (commandName == NULL) {
        line = readline("> ");
        add_history(line);
        int size = strlen(line);
        char *newLine = mallocSafe(size + 1);
        strcpy(newLine, line);
        free(line);
        newLine[size] = '\n';
        size++;
        stream = fmemopen(newLine, size, "r");
        getLine(stream);
        printf("\n");
        commandName = getToken(0);
    }
    return commandName;
}

/*
    fpfInd()
    Prints indentation of depth 'depth' in file pointed by 'file'.
*/
void fpfInd(FILE* file, int depth) {
    int i;
    for (i = 0; i < depth; i++) {
        fprintf(file, "  ");
    }
}

/*
    saveTodo()
    Save all data from todo pointed by 'todo' in file pointed by 'output' with
    depth 'depth' in task tree.
*/
void saveTodo(Todo* todo, FILE* output, int depth) {
    int i;
    fpfInd(output, depth);
    fprintf(output, "\"%s\" %d %d %d\n", todo->name, todo->timeSpent, todo->timeEstimate, todo->status);
    fpfInd(output, depth);
    fprintf(output, "%d\n", todo->nSubtodos);
    for (i = 0; i < todo->nSubtodos; i++) {
        saveTodo(todo->subtodos[i], output, depth + 1);
    }
}

/*
    saveTask()
    Save all data from task pointed by 'task' in file pointed by 'output' with
    depth 'depth' in task tree.
*/
void saveTask(Task* task, FILE* output, int depth) {
    int i;
    fpfInd(output, depth);
    fprintf(output, "\"%s\" %s %d\n", task->name, task->code, task->status);
    fpfInd(output, depth);
    fprintf(output, "%d\n", task->nNotes);
    for (i = 0; i < task->nNotes; i++) {
        fpfInd(output, depth);
        fprintf(output, "\"%s\" %ld\n", task->notes[i].text, task->notes[i].date);
    }
	fpfInd(output, depth);
    fprintf(output, "%d\n", task->nTodos);
    for (i = 0; i < task->nTodos; i++) {
        saveTodo(task->todos[i], output, depth);
    }
    fpfInd(output, depth);
    fprintf(output, "%d\n", task->nPeriods);
    for (i = 0; i < task->nPeriods; i++) {
        fpfInd(output, depth);
        fprintf(output, "%ld %ld \"%s\"\n", task->periods[i].start, task->periods[i].end, task->periods[i].name);
    }
    fpfInd(output, depth);
    fprintf(output, "%d\n", task->nSubtasks);
    for (i = 0; i < task->nSubtasks; i++) {
        saveTask(task->subtasks[i], output, depth + 1);
    }
}

/*
    saveAll()
    Saves all tasks in a file.
*/
void saveAll() {
    FILE* output;
    int i;
    copyFile("data/tasks.txt.bk2", "data/tasks.txt.bk3");
    copyFile("data/tasks.txt.bk1", "data/tasks.txt.bk2");
    copyFile("data/tasks.txt", "data/tasks.txt.bk1");
    output = fopenSafe("data/tasks.txt", "w");
    fprintf(output, "%d\n", rootTask->nSubtasks);
    for (i = 0; i < rootTask->nSubtasks; i++) {
        saveTask(rootTask->subtasks[i], output, 1);
    }
    printf("Tasks saved!\n\n");
    fclose(output);
}

/*
    loadTodo()
    Gets todo data from file pointed by 'input'.
*/
Todo* loadTodo(FILE* input, int type) {
    int i;
    char todoName[NAME_LEN];
    Todo* todo;
    getLine(input);
    strcpy(todoName, getToken(0));
    todo = createTodo(todoName, type);
    todo->timeSpent = atoi(getToken(1));
    todo->timeEstimate = atoi(getToken(2));
    todo->status = atoi(getToken(3));
	getLine(input);
    todo->nSubtodos = atoi(getToken(0));
    for (i = 0; i < todo->nSubtodos; i++) {
        todo->subtodos[i] = loadTodo(input, NODE);
        todo->subtodos[i]->parent.todo = todo;
    }
    return todo;
}

/*
    loadTask()
    Gets task data from file pointed by 'input'.
*/
Task* loadTask(FILE* input) {
    int i;
    char taskName[NAME_LEN];
    char taskCode[CODE_LEN];
    Task* task;
    getLine(input);
    strcpy(taskName, getToken(0));
    strcpy(taskCode, getToken(1));
    task = createTask(taskName, taskCode);
    task->status = atoi(getToken(2));
	getLine(input);
    task->nNotes = atoi(getToken(0));
    for (i = 0; i < task->nNotes; i++) {
        getLine(input);
        strcpy(task->notes[i].text, getToken(0));
        task->notes[i].date = atol(getToken(1));
    }
	
    getLine(input);
    task->nTodos = atoi(getToken(0));
    for (i = 0; i < task->nTodos; i++) {
        task->todos[i] = loadTodo(input, ROOT);
        task->todos[i]->parent.task = task;
    }
    getLine(input);
    task->nPeriods = atoi(getToken(0));
    for (i = 0; i < task->nPeriods; i++) {
        getLine(input);
        task->periods[i].start = atol(getToken(0));
        task->periods[i].end = atol(getToken(1));
        strcpy(task->periods[i].name, getToken(2));
    }
    getLine(input);
    task->nSubtasks = atoi(getToken(0));
    for (i = 0; i < task->nSubtasks; i++) {
        task->subtasks[i] = loadTask(input);
        task->subtasks[i]->parent = task;
    }
    return task;
}

/*
    loadAll()
    Loads all task data from a file.
*/
void loadAll() {
    FILE* input;
    int i;
    input = fopenSafe("data/tasks.txt", "r");
    getLine(input);
    rootTask->nSubtasks = atoi(getToken(0));
    for (i = 0; i < rootTask->nSubtasks; i++) {
        rootTask->subtasks[i] = loadTask(input);
        rootTask->subtasks[i]->parent = rootTask;
    }
    printf("Tasks loaded!\n\n");
    fclose(input);
}
