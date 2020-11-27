#ifndef OBJECTIVES_HPP
#define OBJECTIVES_HPP
/*************************************/
#include <string>
#include <list>
#include <ctime>

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
    std::list<Todo *> subtodos;
    std::list<Schedule *> schedules;
    std::list<Period *> periods;
    int type;
    TodoParent parent;
};

typedef struct {
	long int date;
	std::string text;
} Note;

struct task {
    std::string name;
    std::string uniquePath;
    std::string code;
    int status;
	std::list<Note *> notes;
    std::list<Todo *> todos;
    std::list<Period *> periods;
    std::list<Task *> subtasks;
    Task *parent;
};

typedef struct {
    std::list<Todo *> todos;
    std::list<Schedule *> schedules;
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
