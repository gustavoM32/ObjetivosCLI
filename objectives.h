#ifndef OBJECTIVES_H
#define OBJECTIVES_H
/*************************************/
#include <time.h>

#define NAME_LEN 100
#define CODE_LEN 6
#define PATH_LEN 100
#define NOTE_LEN 200
#define MAX_CHILDS 50
#define MAX_NOTES 50
#define MAX_PERIODS 500
#define COMMAND_LEN 200
#define MAX_COMMANDS 10
#define SECS_IN_A_DAY 86400
#define SECS_IN_A_WEEK 604800

enum TaskStatus {TASK_ACTIVE, TASK_INACTIVE, TASK_COMPLETED, TASK_CANCELED};
enum TodoStatus {TODO_PENDING, TODO_PRIORITY};
enum TodoType {ROOT, NODE};
enum Menus {TASK_MENU, SUBTASKS_MENU, TODOS_MENU, PERIOD_MENU, WEEK_MENU};

typedef struct task Task;
typedef struct todo Todo;

typedef struct {
    long int start;
    long int end;
    char name[NAME_LEN];
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
    char name[NAME_LEN];
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
    char name[NAME_LEN];
    char uniquePath[PATH_LEN];
    char code[CODE_LEN];
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

Task* rootTask;

void freeAll();
/*************************************/
#endif
