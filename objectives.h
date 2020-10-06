#ifndef OBJECTIVES_H
#define OBJECTIVES_H
/*************************************/
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

struct todo {
    char name[NAME_LEN];
    int estimate;
    int status;
    int nSubtodos;
    Todo *subtodos[MAX_CHILDS];
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
     char path[NAME_LEN];
    char name[NAME_LEN];
    int date[3];
} CalendarItem;

typedef struct {
    int nItems;
    Todo *items[MAX_CHILDS*MAX_CHILDS];
} Calendar;

Task* rootTask;
Calendar* todoCalendar;

void freeAll();
/*************************************/
#endif
