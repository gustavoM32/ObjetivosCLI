#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "io.hpp"
#include "util.hpp"
#include "task.hpp"

#define SENTENCE_LIMIT '\"'

using namespace std;

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
        char *newLine = (char *) mallocSafe(size + 1);
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
    fpfInd(output, depth);
    fprintf(output, "\"%s\" %d %d %d\n", todo->name.c_str(), todo->timeSpent, todo->timeEstimate, todo->status);
    fpfInd(output, depth);
    fprintf(output, "%ld\n", todo->schedules.size());
    for (auto it = todo->schedules.begin(); it != todo->schedules.end(); it++) {
        Schedule *sched = *it;
        fpfInd(output, depth);
        fprintf(output, "%d %d %d %ld\n", sched->timeSpent, sched->timeEstimate, sched->timeSet, sched->date);
    }
    fpfInd(output, depth);
    fprintf(output, "%ld\n", todo->subtodos.size());
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        saveTodo(*it, output, depth + 1);
    }
}

/*
    saveTask()
    Save all data from task pointed by 'task' in file pointed by 'output' with
    depth 'depth' in task tree.
*/
void saveTask(Task* task, FILE* output, int depth) {
    fpfInd(output, depth);
    fprintf(output, "\"%s\" %s %d\n", task->name.c_str(), task->code.c_str(), task->status);
    fpfInd(output, depth);
    fprintf(output, "%ld\n", task->notes.size());
    for (auto it = task->notes.begin(); it != task->notes.end(); it++) {
        Note *note = *it;
        fpfInd(output, depth);
        fprintf(output, "\"%s\" %ld\n", note->text.c_str(), note->date);
    }
	fpfInd(output, depth);
    fprintf(output, "%ld\n", task->todos.size());
    for (auto it = task->todos.begin(); it != task->todos.end(); it++) {
        saveTodo(*it, output, depth);
    }
    fpfInd(output, depth);
    fprintf(output, "%ld\n", task->periods.size());
    for (auto it = task->periods.begin(); it != task->periods.end(); it++) {
        Period *period = *it;
        fpfInd(output, depth);
        fprintf(output, "%ld %ld \"%s\"\n", period->start, period->end, period->name.c_str());
    }
    fpfInd(output, depth);
    fprintf(output, "%ld\n", task->subtasks.size());
    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        saveTask(*it, output, depth + 1);
    }
}

/*
    saveAll()
    Saves all tasks in a file.
*/
void saveAll() {
    FILE* output;
    copyFile("data/tasks.txt.bk2", "data/tasks.txt.bk3");
    copyFile("data/tasks.txt.bk1", "data/tasks.txt.bk2");
    copyFile("data/tasks.txt", "data/tasks.txt.bk1");
    output = fopenSafe("data/tasks.txt", "w");
    fprintf(output, "%ld\n", rootTask->subtasks.size());
    for (auto it = rootTask->subtasks.begin(); it != rootTask->subtasks.end(); it++) {
        saveTask(*it, output, 1);
    }
    printf("Tasks saved!\n\n");
    fclose(output);
}

/*
    loadTodo()
    Gets todo data from file pointed by 'input'.
*/
Todo* loadTodo(FILE* input, int type) {
    int count;
    string todoName;
    Todo* todo;

    getLine(input);
    todoName = getToken(0);
    todo = createTodo(todoName, type);
    todo->timeSpent = atoi(getToken(1));
    todo->timeEstimate = atoi(getToken(2));
    todo->status = atoi(getToken(3));

	getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Schedule *sched = new Schedule;
        sched->todo = todo;
        getLine(input);
        sched->timeSpent = atoi(getToken(0));
        sched->timeEstimate = atoi(getToken(1));
        sched->timeSet = atoi(getToken(2));
        sched->date = atol(getToken(3));
        todo->schedules.push_back(sched);
    }

	getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Todo *subtodo = loadTodo(input, NODE);
        todo->subtodos.push_back(subtodo);
        subtodo->parent.todo = todo;
    }
    return todo;
}

/*
    loadTask()
    Gets task data from file pointed by 'input'.
*/
Task* loadTask(FILE* input) {
    int count;
    string taskName;
    string taskCode;
    Task* task;

    getLine(input);
    taskName = getToken(0);
    taskCode = getToken(1);
    task = createTask(taskName, taskCode);
    task->status = atoi(getToken(2));

    getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Note *note = new Note;
        getLine(input);
        note->text = getToken(0);
        note->date = atol(getToken(1));
        task->notes.push_back(note);
    }
	
    getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Todo *todo = loadTodo(input, ROOT);
        todo->parent.task = task;
        task->todos.push_back(todo);
    }

    getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Period *period = new Period;
        getLine(input);
        period->start = atol(getToken(0));
        period->end = atol(getToken(1));
        period->name = getToken(2);
        task->periods.push_back(period);
    }

    getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Task *subtask = loadTask(input);
        subtask->parent = task;
        task->subtasks.push_back(subtask);
    }
    return task;
}

/*
    loadAll()
    Loads all task data from a file.
*/
void loadAll() {
    FILE* input;
    int count;
    input = fopenSafe("data/tasks.txt", "r");

    getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Task *subtask = loadTask(input);
        rootTask->subtasks.push_back(subtask);
        subtask->parent = rootTask;
    }
    printf("Tasks loaded!\n\n");
    fclose(input);
}
