#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "io.hpp"
#include "util.hpp"
#include "task.hpp"


#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#define SENTENCE_LIMIT '\"'

using namespace std;

static vector<string> buffer;

/*
    Returns the number of read commands;
*/
int getNComms() {
    return buffer.size();
}

/*
    Reads a line from stream and separate it into words
    and "sentences with spaces" and stores it into BUFFER.
*/
int getLine(FILE *stream) {
    char c;
    int inCommand = 0;
    int inSpaced = 0;
    buffer.clear();
    string command = "";
    while ((c = getc(stream)) != '\n') {
        if (!inCommand) {
            if (c == SENTENCE_LIMIT) {
                inCommand = 1;
                inSpaced = 1;
            } else if (c != ' ') {
                inCommand = 1;
                command += c;
            }
        } else {
            if (inSpaced && c == SENTENCE_LIMIT) {
                inCommand = 0;
                inSpaced = 0;
                buffer.push_back(command);
                command = "";
            } else if (!inSpaced && c == ' ') {
                inCommand = 0;
                buffer.push_back(command);
                command = "";
            } else {
                command += c;
            }
        }
    }
    if (inSpaced) {
        return 1;
    }
    if (inCommand) {
        buffer.push_back(command);
    }
    return 0;
}

/*
    Returns true if num of arguments in buffer equals NARGS.
    Returns false otherwise and a error message.
*/
int validArgs(int nArgs) {
    if ((int) buffer.size() - 1 != nArgs) {
        if (nArgs == 0) printf("Command accepts no arguments.\n\n");
        else if (nArgs == 1) printf("Command accepts 1 argument.\n\n");
        else printf("Command accepts %d arguments.\n\n", nArgs);

        return 0;
    }
    return 1;
}

/*
    Returns a string from buffer given its ID.
*/
string getToken(int id) {
    if (id >= (int) buffer.size()) return "";
    return buffer[id];
}

/*
    Prompts user for a command until its name is not empty.
*/
string getCommandName() {
    string commandName = "";
    char *line;
    FILE *stream;
    while (commandName == "") {
        line = readline("> ");
        add_history(line);
        int size = strlen(line);
        char *newLine = new char[size + 1];
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
    Prints indentation of depth 'depth' in file pointed by 'file'.
*/
void fpfInd(FILE* file, int depth) {
    int i;
    for (i = 0; i < depth; i++) {
        fprintf(file, "  ");
    }
}

/*
    Save all data from todo pointed by 'todo' in file pointed by 'output' with
    depth 'depth' in task tree.
*/
void saveTodo(Todo* todo, FILE* output, int depth) {
    fpfInd(output, depth);
    fprintf(output, "\"%s\" %d\n", todo->name.c_str(), todo->status);
    fpfInd(output, depth);
    fprintf(output, "%ld\n", todo->schedules.size());
    for (auto it = todo->schedules.begin(); it != todo->schedules.end(); it++) {
        Schedule *sched = *it;
        fpfInd(output, depth);
        fprintf(output, "%d %d %ld\n", sched->timeEstimate, sched->timeSet, sched->date);
    }
    fpfInd(output, depth);
    fprintf(output, "%ld\n", todo->periods.size());
    for (auto it = todo->periods.begin(); it != todo->periods.end(); it++) {
        Period *period = *it;
        fpfInd(output, depth);
        fprintf(output, "%ld %ld\n", period->start, period->end);
    }
    fpfInd(output, depth);
    fprintf(output, "%ld\n", todo->subtodos.size());
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        saveTodo(*it, output, depth + 1);
    }
}

/*
    Save all data from task pointed by 'task' in file pointed by 'output' with
    depth 'depth' in task tree.
*/
void saveTask(Task* task, FILE* output, int depth) {
    fpfInd(output, depth);
    fprintf(output, "\"%s\" \"%s\" %d \"%s\"\n", task->name.c_str(), task->code.c_str(), task->status, task->color.c_str());
    fpfInd(output, depth);
    fprintf(output, "\"%s\"\n", escapeString(task->plan).c_str());
    fpfInd(output, depth);
    fprintf(output, "%ld\n", task->history.size());
    for (auto it = task->history.begin(); it != task->history.end(); it++) {
        Note *note = *it;
        fpfInd(output, depth);
        fprintf(output, "\"%s\" %ld %d\n", note->text.c_str(), note->date, (int) note->motivation);
    }

    saveTodo(task->rootTodo, output, depth + 1);

    fpfInd(output, depth + 1);
    fprintf(output, "%ld\n", task->subtasks.size());
    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        saveTask(*it, output, depth + 1);
    }
}

/*
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
        saveTask(*it, output, 0);
    }
    printf("Dados salvos!\n\n");
    fclose(output);
}

/*
    Gets todo data from file pointed by 'input'.
*/
void loadTodo(FILE* input, Task *task, Todo *parent) {
    int count;
    string todoName;
    Todo* todo;

    getLine(input);
    todoName = getToken(0);
    todo = createTodo(todoName, task, parent);
    todo->status = stoi(getToken(1));

	getLine(input);
    count = stoi(getToken(0));
    while (count--) {
        Schedule *sched = new Schedule;
        sched->todo = todo;
        getLine(input);
        sched->timeEstimate = stoi(getToken(0));
        sched->timeSet = stoi(getToken(1));
        sched->date = stol(getToken(2));
        todo->schedules.push_back(sched);
    }

    getLine(input);
    count = stoi(getToken(0));
    while (count--) {
        Period *period = new Period;
        getLine(input);
        period->start = stol(getToken(0));
        period->end = stol(getToken(1));
        period->todo = todo;
        todo->periods.push_back(period);
    }

	getLine(input);
    count = stoi(getToken(0));
    while (count--) {
        loadTodo(input, task, todo);
    }

    if (parent != nullptr) todo->parent->subtodos.push_back(todo);
    else task->rootTodo = todo;
}

/*
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
    task->status = stoi(getToken(2));
    task->color = getToken(3);

    getLine(input);
    string planText = getToken(0);
    task->plan = unescapeString(planText);

    getLine(input);
    count = stoi(getToken(0));
    while (count--) {
        Note *note = new Note;
        getLine(input);
        note->text = getToken(0);
        note->date = stol(getToken(1));
        note->motivation = stoi(getToken(2));
        task->history.push_back(note);
    }

    loadTodo(input, task, nullptr);
    
    getLine(input);
    count = stoi(getToken(0));
    while (count--) {
        Task *subtask = loadTask(input);
        subtask->parent = task;
        task->subtasks.push_back(subtask);
    }
    return task;
}

/*
    Loads all task data from a file.
*/
void loadAll() {
    FILE* input;
    int count;

    input = fopen("data/tasks.txt", "r");
    if (input == nullptr) return;

    getLine(input);
    count = stoi(getToken(0));
    while (count--) {
        Task *subtask = loadTask(input);
        rootTask->subtasks.push_back(subtask);
        subtask->parent = rootTask;
    }
    printf("Dados carregados!\n\n");
    fclose(input);
}
