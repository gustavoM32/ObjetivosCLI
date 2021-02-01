#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <list>
#include <ctime>
#include <limits>
#include <stdlib.h>
#include "util.hpp"
#include "objectives.hpp"

using namespace std;

/*
    This function does the same thing as malloc but exits the program if it
    can't allocate memory.
*/
void* mallocSafe(int nBytes) {
    void* ptr = malloc(nBytes);
    if (ptr == nullptr) {
        printf("Out of memory! Exiting...\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

/*
    This function does the same thing as fopen but exits the program if is
    unable to open the file.
*/
FILE* fopenSafe(char* fileName, char* mode) {
    FILE* file;
	file = fopen(fileName, mode);
    if (file == nullptr) {
	    printf("Error opening file.\n\n");
	    exit(EXIT_FAILURE);
	}
    return file;
}

/*
    This function returns current time in seconds since the start of 1970.
*/
long int getCurrentTime() {
    time_t curTime;
    time(&curTime);
    return curTime;
}

int getYear(time_t time) {
    struct tm *stm = localtime(&time);
    return stm->tm_year + 1900;
}

/*
    Returns the start of the day of supplied date in local time.
*/
time_t getDayStart(time_t time) {
    struct tm *stm = localtime(&time);
    stm->tm_sec = 0;
    stm->tm_min = 0;
    stm->tm_hour = 0;
    return mktime(stm);
}

time_t getPersonalDayStart(time_t time) {
    time_t personalDayStart = getDayStart(time) + 60 * PERSONAL_DAY_START;
    if (personalDayStart > time) personalDayStart -= SECS_IN_A_DAY;
    return personalDayStart;
}

/*
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

/*
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

/*
    This function returns 'time' formated as 'dd/mm'.
*/
string formatDate(long int time, bool showYear) {
    struct tm *structTime;
    stringstream s;
    structTime = localtime(&time);
    s << setfill('0');
    s << setw(2) << structTime->tm_mday;
    s << "/" << setw(2) << structTime->tm_mon + 1;
    if (showYear) s << "/" << setw(2) << structTime->tm_year % 100;
    return s.str();
}

/*
    Returns time in seconds of date 'day'/'month'/'year' and time
    'hour':'min':'sec'.
*/
long int getTime(int day, int month, int year, int hour, int min, int sec) {
    struct tm *structTime;
    time_t curTime = time(nullptr);
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
    Replaces new line characters to escaped ones.
*/
string escapeNewLines(string s) {
    size_t start_pos = 0;
    while((start_pos = s.find("\n", start_pos)) != string::npos) {
        s.replace(start_pos, 1, "\\n");
        start_pos += 2;
    }
    return s;
}

/*
    Replaces escaped new line characters to real ones.
*/
string unescapeNewLines(string s) {
    size_t start_pos = 0;
    while((start_pos = s.find("\\n", start_pos)) != string::npos) {
        s.replace(start_pos, 2, "\n");
        start_pos += 1;
    }
    return s;
}

/*
    Copy file with name orig to dest.
*/
void copyFile(char *orig, char *dest) {
    FILE *input;
    FILE *output;
    char c;
    input = fopen(orig, "r");
    output = fopen(dest, "w");
    if (input == nullptr || output == nullptr) {
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
    Warns user that the command doesn't exist in current scope.
*/
void notAvailable(char* userCommand) {
    printf("Command \"%s\" does not exist or is not available.\n\n", userCommand);
}

/*
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
    Count the number of incomplete to-dos in a todo.
*/
int countTodosTodo(Todo *todo) {
    int res = 0;
    
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        Todo *subtodo = *it;
        if (!todoCompleted(subtodo)) {
            int subTodosCount = countTodosTodo(subtodo);
            if (subTodosCount == 0) res += 1;
            else res += subTodosCount;
        }
    }

    return res;
}

/*
    Count the number of incomplete to-dos in a task.
*/
int countTodosTask(Task *task) {
    if (task->status != TASK_ACTIVE) return 0;

    int res = countTodosTodo(task->rootTodo);

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        res += countTodosTask(*it);
    }

    return res;
}

/*
    Returns 1 if a word is in an array of words
*/

int isInList(string word, list<string> l) {
    for (auto it = l.begin(); it != l.end(); it++) {
        if (word == *it) return true;
    }
    return false;
}

/*
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

/*
    periodComp
    Period compare function.
*/
bool periodComp(const Period *p1, const Period *p2) {
    if (p1->start < p2->start) return true;
    return false;
}

/*
    schedComp
    Schedule compare function.
*/
bool schedComp(const Schedule *sched1, const Schedule *sched2) {
    if (sched1->date < sched2->date) return true;
    if (sched1->date > sched2->date) return false;
    if (sched1->timeSet) return false;
    if (sched2->timeSet) return true;
    return false;
}

/*
    Order periods by time.
*/
void sortPeriods(list<Period *> &periods) {
    periods.sort(periodComp);
}

/*
    Inserts all TODO periods into PERIODS and the vector size into N.
*/
void getPeriodsFromTodo(list<Period *> &periods, Todo *todo) {
    for (auto it = todo->periods.begin(); it != todo->periods.end(); it++) {
        periods.push_back(*it);
    }
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        getPeriodsFromTodo(periods, *it);
    }
}


/*
    Inserts all TASK periods into PERIODS and the vector size into N.
*/
void getPeriodsFromTask(list<Period *> &periods, Task *task) {    
    getPeriodsFromTodo(periods, task->rootTodo);

    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        getPeriodsFromTask(periods, *it);
    }
}

time_t periodIntersect(time_t start1, time_t end1, time_t start2, time_t end2) {
    if (end1 <= start2) return 0;
    if (end2 <= start1) return 0;
    return min(end1, end2) - max(start1, start2);
}

/*
    Returns the time spent in to-do.
*/
time_t getTodoTime(Todo *todo) {
    time_t time = 0;
    for (auto it = todo->periods.begin(); it != todo->periods.end(); it++) {
        Period *period = *it;
        time += period->end - period->start;
    }
    return time;
}

/*
    Returns the time spent in to-do and its descendants.
*/
time_t getTodoTotalTime(Todo *todo) {
    time_t time = getTodoTime(todo);
    for (auto it = todo->subtodos.begin(); it != todo->subtodos.end(); it++) {
        time += getTodoTotalTime(*it);
    }
    return time;
}

/*
    Get a list of ids from a id path.
*/
void getIdPath(string path, list<size_t> &ids) {
    for (size_t i = 0; i < path.size(); i++) {
        if (path[i] >= '0' && path[i] <= '9') continue;
        path[i] = ' ';
    }
    stringstream pathStream(path);
    while (pathStream.good()) {
        size_t num;
        pathStream >> num;
        ids.push_back(num);
    }

    if (!pathStream.eof()) {
        cerr << "Error getting ids\n" << endl;
        ids.clear();
    }
}

string getTodoFullName(Todo *todo) {
    string name = "";

    if (todo->parent->parent == nullptr) {
        name += todo->task->code;
    } else {
        name += getTodoFullName(todo->parent);
    }

    name += " > ";
    name += todo->name;
    return name;
}

string getTodoPath(Todo *todo) {
    string name = "";

    if (todo->parent == nullptr) {
        return todo->task->name;
    }

    if (todo->parent->parent != nullptr) {
        name += getTodoPath(todo->parent);
        name += " > ";
    }

    name += todo->name;
    return name;
}

/*
    Return 'true' if 'todo' is completed
*/
bool todoCompleted(Todo *todo) {
    return todo->status == TODO_COMPLETED || todo->status == TODO_COMPLETED_HIDDEN;
}

/*
    Add 'color' to string 'text'
*/
string colorString(string text, Color color) {
    stringstream colored;
    colored << "\033[" << color << "m" << text << "\033[0m";
    return colored.str();
}

string getColor(Color color) {
    return "\033[" + to_string(color) + "m";
}

void printLine(string symbol, bool extraSpace) {
    string line = "";
    for (int i = 0; i < LINE_LEN; i++) {
        line += symbol;
    }
    line += "\n";
    if (extraSpace) line += "\n";
    cout << colorString(line, CYAN);
}

void printTitle(string text, string symbol, bool extraSpace) {
    int totalSymbol = (LINE_LEN - text.size() - 4);
    int halfSymbol = totalSymbol / 2;
    string title = "";
    for (int i = 0; i < totalSymbol - halfSymbol; i++) {
        title += symbol;
    }
    title += "  ";
    title += text;
    title += "  ";
    for (int i = 0; i < halfSymbol; i++) {
        title += symbol;
    }
    title += "\n";
    if (extraSpace) title += "\n";
    cout << colorString(title, CYAN);
}