#ifndef OBJECTIVES_HPP
#define OBJECTIVES_HPP
/*************************************/
#include <string>
#include <time.h>

#define CODE_LEN 6
#define NOTE_LEN 200
#define MAX_CHILDS 50
#define MAX_NOTES 50
#define MAX_PERIODS 500
#define COMMAND_LEN 200
#define MAX_COMMANDS 10
#define SECS_IN_A_DAY 86400
#define SECS_IN_A_WEEK 604800

enum TaskStatus {TASK_ACTIVE, TASK_INACTIVE, TASK_COMPLETED, TASK_CANCELED};
enum TodoStatus {TODO_PENDING, TODO_PRIORITY, TODO_COMPLETED};
enum TodoType {ROOT, NODE};
enum Menus {TASK_MENU, SUBTASKS_MENU, TODOS_MENU, PERIODS_MENU, CALENDAR_MENU};

typedef struct task Task;
typedef struct todo Todo;

typedef struct {
    long int start;
    long int end;
    std::string name; // remove
} Period;

typedef union {
    Task *task;
    Todo *todo;
} TodoParent;

typedef struct {
    Todo *todo;
    int timeSpent;
    int timeEstimate;
    int timeSet;
    time_t date;
} Schedule;

struct todo {
    std::string name;
    int timeSpent;
    int timeEstimate;
    int status;
    int nSubtodos;
    Todo *subtodos[MAX_CHILDS];
    int nSchedules;
    Schedule *schedules[MAX_CHILDS];
    int type;
    TodoParent parent;
};

typedef struct {
	long int date;
	char text[NOTE_LEN];
} Note;

struct task {
    std::string name;
    std::string uniquePath;
    std::string code;
    int status;
	int nNotes;
	Note notes[MAX_NOTES];
    int nTodos;
    Todo *todos[MAX_CHILDS];
    int nPeriods;
    Period periods[MAX_PERIODS];
    int nSubtasks;
    Task *subtasks[MAX_CHILDS];
    Task *parent;
};

typedef struct {
    int nTodos;
    Todo *todos[MAX_CHILDS*MAX_CHILDS];
    int nSchedules;
    Schedule *schedules[MAX_CHILDS*MAX_CHILDS];
    Schedule *periodSched;
} Calendar;

extern Task *rootTask;
extern Calendar *calendar;
extern int lastMenu;
extern int curMenu;
extern Task *curTask;

void freeAll();

bool generalCommands(char *commandName);
/*************************************/
#endif
