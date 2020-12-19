#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <readline/readline.h>
#include <readline/history.h>
#include "ioold.hpp"
#include "util.hpp"
#include "task.hpp"

#define SENTENCE_LIMIT '\"'

using namespace std;

////// remover tudo abaixo

bool uppercase(string s) {
    string up = s;
    for (int i = 0; i < up.size(); i++) {
        if (up[i] >= 'a' && up[i] <= 'z') up[i] = up[i] - 'a' + 'A';
    }
    return up == s;
}

// CON > OBJ
// CON > EST

Todo *getTodo(list<Todo *> l, string name) {
    for (auto it = l.begin(); it != l.end(); it++) {
        Todo *todo = *it;
        if (todo->name == name) return todo;
    }
    return nullptr;
}

void periodPath(Task *task, Period *period) {
    string periodName = period->name;
    vector<string> words;
    stringstream path;
    path.str(periodName);
    string worder;
    while (!path.eof()) {
        string word;
        path >> word;
        if (word == ">") {
            words.push_back(worder);
            worder.clear();
        } else {
            if (worder.size() != 0) worder += " ";
            worder += word;
        }
    }
    if (worder.size() != 0) words.push_back(worder);

    vector<string> filtered;

    if (words.size() == 1) {
        filtered.push_back(words[0]);
    } else {
        int i = 0;
        bool isObj = (uppercase(words[i]) && words[i].size());
        if (isObj) {
            i++;
            if (words[1] == "OBJ" || words[1] == "EST") i++;
        }
        while (i < words.size()) {
            filtered.push_back(words[i]);
            i++;
        }
    }

    int i = 0;

    Todo *parent = task->rootTodo;
    Todo *todo = getTodo(task->rootTodo->subtodos, filtered[i]);

    while (i < filtered.size()) {
        if (todo == nullptr) {
            todo = createTodo(filtered[i], task, parent);
            parent->subtodos.push_back(todo);
        }
        
        parent = todo;
        i++;
        if (i == filtered.size()) break;
        todo = getTodo(todo->subtodos, filtered[i]);
    }

    period->todo = todo;
    todo->periods.push_back(period);
    todo->periods.sort(periodComp);

    // for (int i = 0; i < filtered.size(); i++) {
    //     cout << filtered[i];
    //     if (i + 1 != filtered.size()) cout << " > ";
    // }
    // cout << endl;
}

///// remover tudo acima

/*
    Gets todo data from file pointed by 'input'.
*/
Todo* loadTodoOld(FILE* input, Task *task, Todo *parent = nullptr) {
    int count;
    string todoName;
    Todo* todo;
	int deletar;

    getLine(input);
    todoName = getToken(0);
    todo = createTodo(todoName, task);
    deletar = atoi(getToken(1));
    deletar = atoi(getToken(2));
    todo->status = atoi(getToken(3));

	getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Schedule *sched = new Schedule;
        sched->todo = todo;
        getLine(input);
        deletar = atoi(getToken(0));
        sched->timeEstimate = atoi(getToken(1));
        sched->timeSet = atoi(getToken(2));
        sched->date = atol(getToken(3));
        todo->schedules.push_back(sched);
    }

	getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Todo *subtodo = loadTodoOld(input, task, todo);
        todo->subtodos.push_back(subtodo);
    }
    return todo;
}

/*
    Gets task data from file pointed by 'input'.
*/
Task* loadTaskOld(FILE* input) {
    int count;
    string taskName;
    string taskCode;
    Task* task;

    getLine(input);
    taskName = getToken(0);
    taskCode = getToken(1);
    task = createTask(taskName, taskCode);
    task->status = atoi(getToken(2));
    task->rootTodo = createTodo("To-dos", task);

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
        Todo *todo = loadTodoOld(input, task, task->rootTodo);
        task->rootTodo->subtodos.push_back(todo);
    }

    string deletar;

    getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Period *period = new Period;
        getLine(input);
        period->start = atol(getToken(0));
        period->end = atol(getToken(1));
        period->name = getToken(2);
        periodPath(task, period);
    }

    getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Task *subtask = loadTaskOld(input);
        subtask->parent = task;
        task->subtasks.push_back(subtask);
    }
    return task;
}

/*
    Loads all task data from a file.
*/
void loadAllOld() {
    FILE* input;
    int count;
    input = fopenSafe("data/tasks.txt", "r");

    getLine(input);
    count = atoi(getToken(0));
    while (count--) {
        Task *subtask = loadTaskOld(input);
        rootTask->subtasks.push_back(subtask);
        subtask->parent = rootTask;
    }
    printf("Tasks loaded!\n\n");
    fclose(input);
}