#ifndef OBJECTIVES_HPP
#define OBJECTIVES_HPP
/*************************************/
#include <string>
#include <list>
#include <map>
#include <ctime>

#define LINE_LEN 160
#define SECS_IN_A_DAY 86400
#define SECS_IN_A_WEEK 604800

#define PERSONAL_DAY_START 360 // 6 * 60 + 0

#define PROGRAM_LEVEL "█"
#define MAIN_LEVEL "▄"
#define SECONDARY_LEVEL "─"

#define DEFAULT_TASK_COLOR "CYAN"

enum TaskStatus {TASK_ACTIVE, TASK_INACTIVE, TASK_CANCELED, TASK_COMPLETED};
enum TodoStatus {TODO_PENDING, TODO_COMPLETED, TODO_COMPLETED_HIDDEN, TODO_HABIT};
enum TodoType {ROOT, NODE};
enum Menus {TASK_MENU, SUBTASKS_MENU, TODOS_MENU, PERIODS_MENU, CALENDAR_MENU};

struct Task;
struct Todo;

struct Period {
    long int start;
    long int end;
    std::string name; // remover após conversão
    Todo *todo;
};

struct Schedule {
    Todo *todo;
    int timeEstimate;
    int timeSet;
    time_t date;
};

struct Todo {
    std::string name;
    int status;
    std::list<Schedule *> schedules;
    std::list<Period *> periods;
    std::list<Todo *> subtodos;
    Todo *parent;
    Task *task;
};

struct Note {
	long int date;
	std::string text;
    bool motivation;
};

struct Task {
    std::string name;
    std::string code;
    std::list<std::string> plan_order;
    std::map<std::string, std::string> plans; // (plan name, plan text)
    std::string color;
    int status;
	std::list<Note *> history;
    Todo *rootTodo;
    std::list<Task *> subtasks;
    Task *parent;
};

struct Calendar{
    std::list<Todo *> habits;
    std::list<Schedule *> schedules;
    Schedule *periodSched;
};

extern Task *rootTask;
extern Calendar *calendar;
extern int lastMenu;
extern int curMenu;
extern Task *curTask;

void freeAll();

bool generalCommands(std::string commandName);
/*************************************/
#endif
