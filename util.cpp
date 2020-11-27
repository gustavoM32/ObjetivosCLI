#include <string>
#include <sstream>
#include <iomanip>
#include <list>
#include <ctime>
#include <stdlib.h>
#include "util.hpp"
#include "objectives.hpp"

using namespace std;

/**
    mallocSafe()
    This function does the same thing as malloc but exits the program if it
    can't allocate memory.
*/
void* mallocSafe(int nBytes) {
    void* ptr = malloc(nBytes);
    if (ptr == NULL) {
        printf("Out of memory! Exiting...\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

/**
    fopenSafe()
    This function does the same thing as fopen but exits the program if is
    unable to open the file.
*/
FILE* fopenSafe(char* fileName, char* mode) {
    FILE* file;
	file = fopen(fileName, mode);
    if (file == NULL) {
	    printf("Error opening file.\n\n");
	    exit(EXIT_FAILURE);
	}
    return file;
}

/**
    getCurrentTime()
    This function returns current time in seconds since the start of 1970.
*/
long int getCurrentTime() {
    time_t curTime;
    time(&curTime);
    return curTime;
}

/**
    getDayStart()
    Returns the start of the day of supplied date in local time.
*/
time_t getDayStart(time_t time) {
    struct tm *stm = localtime(&time);
    stm->tm_sec = 0;
    stm->tm_min = 0;
    stm->tm_hour = 0;
    return mktime(stm);
}

/**
    formatDur()
    This function returns in 'timeString' the 'totalTime' formated as 'h:mm:ss'.
*/
string formatDur(long int totalTime) {
    int hours, minutes, seconds;
    stringstream s;
    seconds = totalTime % 60;
    totalTime /= 60;
    minutes = totalTime % 60;
    totalTime /= 60;
    hours = totalTime;
    s << setfill('0')
        << hours << ":"
        << setw(2) << minutes << ":"
        << setw(2) << seconds;
    return s.str();
}

/**
    formatTime()
    This function returns in 'timeString' the 'time' formated as
    'dd/mm/yy hh:mm:ss'.
*/
string formatTime(long int time) {
    struct tm *structTime;
    stringstream s;
    structTime = localtime(&time);
    s << setfill('0')
        << setw(2) << structTime->tm_mday << "/"
        << setw(2) << structTime->tm_mon + 1 << "/"
        << setw(2) << structTime->tm_year - 100 << " "
        << setw(2) << structTime->tm_hour << ":"
        << setw(2) << structTime->tm_min << ":"
        << setw(2) << structTime->tm_sec;
    return s.str();
}

/**
    formatDate()
    This function returns 'time' formated as 'dd/mm'.
*/
string formatDate(long int time) {
    struct tm *structTime;
    stringstream s;
    structTime = localtime(&time);
    s << setfill('0')
        << setw(2) << structTime->tm_mday << "/"
        << setw(2) << structTime->tm_mon + 1;
    return s.str();
}

/*
    getTime()
    Returns time in seconds of date 'day'/'month'/'year' and time
    'hour':'min':'sec'.
*/
long int getTime(int day, int month, int year, int hour, int min, int sec) {
    struct tm *structTime;
    time_t curTime = time(NULL);
    structTime = localtime(&curTime);
    structTime->tm_mday = day;
    structTime->tm_mon = month - 1;
    structTime->tm_year = year - 1900;
    structTime->tm_hour = hour;
    structTime->tm_min = min;
    structTime->tm_sec = sec;
    return mktime(structTime);
}

/*
    changeTime()
    Returns new time from 'oldTime' with changed 'hour', 'min' and 'sec'.
*/
long int changeTime(long int oldTime, int hour, int min, int sec) {
    struct tm *structTime;
    structTime = localtime(&oldTime);
    structTime->tm_hour = hour;
    structTime->tm_min = min;
    structTime->tm_sec = sec;
    return mktime(structTime);
}

/*
    changeDate()
    Returns new time from 'oldTime' with changed 'day', 'month' and 'year'.
*/
long int changeDate(long int oldTime, int day, int month, int year) {
    struct tm *structTime;
    structTime = localtime(&oldTime);
    if (year >= 1900) structTime->tm_year = year - 1900;
    else structTime->tm_year += year;
    structTime->tm_mon = month - 1;
    structTime->tm_mday = day;
    return mktime(structTime);
}

/*
    formatTaskName()
    Takes input and removes starting spaces and ends it after a newline or end
    of string character is found
*/
void formatTaskName(char* input) {
    int iId;
    int oId;
    for (iId = 0; input[iId] == ' '; iId++);
    for (oId = 0; input[iId] != '\n' && input[iId] != '\0'; oId++) {
        input[oId] = input[iId];
        iId++;
    }
    input[oId] = '\0';
}

/*
    readFileString()
    Returns in 'name' the next string in file 'input', skipping spaces and
    newline chars until the start of string and ends it when a newline char is
    found or 'size' is reached.
*/
void readFileString(char* name, int size, FILE* input) {
    char readChar;
    int i = 0;
    readChar = getc(input);
    while (readChar == '\n' || readChar == ' ') {
        readChar = getc(input);
    }
    while (readChar != '\n' || i == size - 1) {
        name[i] = readChar;
        i++;
        readChar = getc(input);
    }
    name[i] = '\0';
}

/*
    toLowercase()
    Converts string 's' to lowercase.
*/
char *toLowercase(char *s) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        if ('A' <= s[i] && s[i] <= 'Z') {
            s[i] += 'a' - 'A';
        }
    }
    return s;
}

/*
    toUppercase()
    Converts string 's' to uppercase.
*/
char *toUppercase(char *s) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        if ('a' <= s[i] && s[i] <= 'z') {
            s[i] += 'A' - 'a';
        }
    }
    return s;
}

/*
    copyFile()
    Copy file with name orig to dest.
*/
void copyFile(char *orig, char *dest) {
    FILE *input;
    FILE *output;
    char c;
    input = fopen(orig, "r");
    output = fopen(dest, "w");
    if (input == NULL || output == NULL) {
        printf("Erro na cópia de \"%s\" para \"%s\"!\n", orig, dest);
        return;
    }
    while ((c = getc(input)) != EOF) {
        fprintf(output, "%c", c);
    }
    fclose(input);
    fclose(output);
}

/*
    notAvailable()
    Warns user that the command doesn't exist in current scope.
*/
void notAvailable(char* userCommand) {
    printf("Command \"%s\" does not exist or is not available.\n\n", userCommand);
}

/*
    countTasks()
    DOCUMENTAR.
*/
int countTasks(Task *task, string &code) {
    int count = 0;
    if (task->code == code) count++;
    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        count += countTasks(*it, code);
    }
    return count;
}

/*
    setUPath()
    DOCUMENTAR.
*/
void setUPath(Task *root, Task *task) {
    string uniquePath;

    if (countTasks(root, task->code) == 1 || task->parent == NULL) {
        task->uniquePath = task->code;
    } else {
        uniquePath += task->parent->uniquePath;
        uniquePath += " > ";
        uniquePath += task->code;
        task->uniquePath = uniquePath;
    }

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        setUPath(root, *it);
    }
}

/*
    countTodosTodo()
    Count the number of to-dos in a todo.
*/
int countTodosTodo(Todo *todo) {
    int res = 0;
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        Todo *subtodo = *it;
        if (subtodo->status != TODO_COMPLETED)
            res += 1 + countTodosTodo(subtodo);
    }
    return res;
}

/*
    countTodosTask()
    Count the number of to-dos in a task.
*/
int countTodosTask(Task *task) {
    int res = 0;

    for (auto it = task->todos.begin(); it != task->todos.end(); it++) {
        Todo *todo = *it;
        if (todo->status != TODO_COMPLETED)
            res += 1 + countTodosTodo(todo);
    }

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        res += countTodosTask(*it);
    }

    return res;
}

/*
    isInList()
    Returns 1 if a word is in an array of words
*/

int isInList(string word, list<string> l) {
    for (auto it = l.begin(); it != l.end(); it++) {
        if (word == *it) return true;
    }
    return false;
}

/*
    ithTask()
    Returns the ith task in a list, nullptr if i is a invalid ID.
*/
Task *ithTask(list<Task *> &tasks, size_t i) {
    if (i < 0 || i >= tasks.size()) {
        printf("Invalid ID.\n\n");
        return nullptr;
    }

    for (auto it = tasks.begin(); it != tasks.end(); it++) {
        if (i == 0) return *it;
        i--;
    }
    return nullptr;
}

/*
    ithTodo()
    Returns the ith todo in a list, nullptr if i is a invalid ID.
*/
Todo *ithTodo(list<Todo *> &todos, size_t i) {
    if (i < 0 || i >= todos.size()) {
        printf("Invalid ID.\n\n");
        return nullptr;
    }

    for (auto it = todos.begin(); it != todos.end(); it++) {
        if (i == 0) return *it;
        i--;
    }
    return nullptr;
}

/*
    ithPeriod()
    Returns the ith period in a list, nullptr if i is a invalid ID.
*/
Period *ithPeriod(list<Period *> &periods, size_t i) {
    if (i < 0 || i >= periods.size()) {
        printf("Invalid ID.\n\n");
        return nullptr;
    }

    for (auto it = periods.begin(); it != periods.end(); it++) {
        if (i == 0) return *it;
        i--;
    }
    return nullptr;
}

/*
    ithSchedule()
    Returns the ith schedule in a list, nullptr if i is a invalid ID.
*/
Schedule *ithSchedule(list<Schedule *> &schedules, size_t i) {
    if (i < 0 || i >= schedules.size()) {
        printf("Invalid ID.\n\n");
        return nullptr;
    }

    for (auto it = schedules.begin(); it != schedules.end(); it++) {
        if (i == 0) return *it;
        i--;
    }
    return nullptr;
}

/*
    ithNote()
    Returns the ith note in a list, nullptr if i is a invalid ID.
*/
Note *ithNote(list<Note *> &notes, size_t i) {
    if (i < 0 || i >= notes.size()) {
        printf("Invalid ID.\n\n");
        return nullptr;
    }

    for (auto it = notes.begin(); it != notes.end(); it++) {
        if (i == 0) return *it;
        i--;
    }
    return nullptr;
}