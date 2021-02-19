#ifndef OBJECTIVES_HPP
#define OBJECTIVES_HPP
/*************************************/
#include <string>
#include <list>
#include <ctime>

#define COMMAND_LEN 200
#define MAX_COMMANDS 10
#define LINE_LEN 160
#define SECS_IN_A_DAY 86400
#define SECS_IN_A_WEEK 604800

#define PERSONAL_DAY_START 360 // 6 * 60 + 0

#define PROGRAM_LEVEL "█"
#define MAIN_LEVEL "▄"
#define SECONDARY_LEVEL "─"

enum TaskStatus {TASK_ACTIVE, TASK_INACTIVE, TASK_COMPLETED, TASK_CANCELED};
enum TodoStatus {TODO_PENDING, TODO_COMPLETED, TODO_COMPLETED_HIDDEN, TODO_HABIT};
enum TodoType {ROOT, NODE};
enum Menus {TASK_MENU, SUBTASKS_MENU, TODOS_MENU, PERIODS_MENU, CALENDAR_MENU};

typedef struct task Task;
typedef struct todo Todo;

typedef struct {
    long int start;
    long int end;
    std::string name; // remover após conversão
    Todo *todo;
} Period;

typedef struct {
    Todo *todo;
    int timeEstimate;
    int timeSet;
    time_t date;
} Schedule;

struct todo {
    std::string name;
    int status;
    std::list<Schedule *> schedules;
    std::list<Period *> periods;
    std::list<Todo *> subtodos;
    Todo *parent;
    Task *task;
};

typedef struct {
	long int date;
	std::string text;
    bool motivation;
} Note;

struct task {
    std::string name;
    std::string code;
    std::string plan;
    int color;
    int status;
	std::list<Note *> history;
    Todo *rootTodo;
    std::list<Task *> subtasks;
    Task *parent;
};

typedef struct {
    std::list<Todo *> habits;
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
