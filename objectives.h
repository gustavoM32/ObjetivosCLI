#ifndef OBJECTIVES_H
#define OBJECTIVES_H
/*************************************/
#define NAME_LEN 100
#define CODE_LEN 4
#define PATH_LEN 100
#define NOTE_LEN 200
#define MAX_CHILDS 50
#define MAX_NOTES 50
#define MAX_PERIODS 500
#define COMMAND_LEN 200
#define MAX_COMMANDS 10
#define SECS_IN_A_DAY 86400
#define SECS_IN_A_WEEK 604800

enum TaskStatus {TASK_ACTIVE, TASK_INACTIVE, TASK_COMPLETED};
enum Menus {TASK_MENU, SUBTASKS_MENU, TODOS_MENU, PERIOD_MENU, WEEK_MENU};

typedef struct period Period;
typedef struct todo Todo;
typedef struct note Note;
typedef struct task Task;
typedef struct calendarItem CalendarItem;
typedef struct calendar Calendar;

struct period {
    long int start;
    long int end;
    char name[NAME_LEN];
};

struct todo {
    long int planned;
    char name[NAME_LEN];
    Task *parent;
};

struct note {
	long int date;
	char text[NOTE_LEN];
};

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

struct calendarItem {
    char path[NAME_LEN];
    char name[NAME_LEN];
    int date[3];
};

struct calendar {
    int nItems;
    Todo *items[MAX_CHILDS*MAX_CHILDS];
};

Task* rootTask;
Calendar* todoCalendar;

void freeAll();
/*************************************/
#endif
