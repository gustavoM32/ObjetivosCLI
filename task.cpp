#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <cstring>
#include <stdlib.h>
#include "calendar.hpp"
#include "help.hpp"
#include "io.hpp"
#include "util.hpp"
#include "task.hpp"
#include "taskUtil.hpp"

using namespace std;

static Task *selectedTask;

/*
    Gets user new subtask code and name and adds it to task 'parent' subtask
    list.
*/
void addSubtask(Task* parent) {
    string subtaskName;
    string subtaskCode;
    subtaskCode = toUppercase(getToken(1));
    subtaskName = getToken(2);

    if (subtaskCode.size() < 1 || subtaskCode.size() > 5) {
        printf("Subtask code must be 1-5 characters long.\n\n");
        return;
    } else if (codeUsed(rootTask, subtaskCode)) {
        printf("Code \"%s\" is not available.\n\n", subtaskCode.c_str());
        return;
    }

    if (subtaskName.size() == 0) {
        printf("Subtask name must not be empty.\n\n");
        return;
    } else if (findTaskByName(parent, subtaskName) != nullptr) {
        printf("\"%s\" already has subtask named \"%s\".\n\n", parent->name.c_str(), subtaskName.c_str());
        return;
    }

    Task *task = createTask(subtaskName.c_str(), subtaskCode.c_str());
    task->parent = parent;
    parent->subtasks.push_back(task);

    printf("Added task \"%s\".\n\n", subtaskName.c_str());
}

/*
    Gets subtask code from user and deletes it from subtask list of task pointed
    by 'parent'
*/
void removeSubtask(Task *parent) {
    string subtaskCode;
    subtaskCode = toUppercase(getToken(1));
    Task *task = findTaskByCode(parent, subtaskCode);

    if (task == nullptr) {
        printf("There's no task with code \"%s\".\n\n", subtaskCode.c_str());
        return;
    }

    printf("Removed task (%s) %s.\n\n", task->code.c_str(), task->name.c_str());
    
    freeTask(task);

    parent->subtasks.remove(task);
}

/*
    Saves address of subtask of 'parent' inputed by user.
*/
void selectSubtask(Task* parent) {
    Task *task = getSubtask(parent);

    if (task == nullptr) return;

    selectedTask = task;
    printf("Selected task (%s) %s.\n\n", selectedTask->code.c_str(), selectedTask->name.c_str());
}

/*
    Moves selected task to 'parent' subtask list.
*/
void moveTask(Task* newParent) {
    if (selectedTask == nullptr) {
        printf("There's no selected task.\n\n");
        return;
    } else if (findTaskByCode(newParent, selectedTask->code) != nullptr) {
        printf("\"%s\" already has subtask with code \"%s\".\n\n", newParent->name.c_str(), selectedTask->code.c_str());
        return;
    } else if (findTaskByName(newParent, selectedTask->name) != nullptr) {
        printf("\"%s\" already has subtask named \"%s\".\n\n", newParent->name.c_str(), selectedTask->name.c_str());
        return;
    }

    printf("Moved task (%s) \"%s\" from (%s) to (%s)\n\n", selectedTask->code.c_str(), selectedTask->name.c_str(), selectedTask->parent->code.c_str(), newParent->code.c_str());

    selectedTask->parent->subtasks.remove(selectedTask);

    selectedTask->parent = newParent;
    newParent->subtasks.push_back(selectedTask);
    
    selectedTask = nullptr;
}

/*
    Lists all subtasks of task pointed by 'task'.
*/
void listSubtasks(Task* task) {
    int i;
    string statusNames[4] = {"active", "inactive", "completed", "canceled"};
    list<Task *> lists[4];
    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        Task *subtask = *it;
        int status = subtask->status;
        lists[status].push_back(subtask);
    }
    cout << "    ";
    if ((lists[0].size() + lists[1].size() + lists[2].size() + lists[3].size()) == 0) {
        cout << colorString("Não há sub-objetivos.\n", CYAN);
    } else {
        for (i = 0; i < 4; i++) {
            listStatusTasks(lists[i], statusNames[i]);
        }
        cout << "\n";
    }
}

/*
    Renames task pointed by 'task' to name inputed by user.
*/
void renameTask(Task* task) {
    string taskName;
    taskName = getToken(1);
    if (taskName.size() == 0) {
        printf("Subtask name must not be empty.\n\n");
        return;
    } else if (task->parent == nullptr) {
        printf("Can't rename main task.\n\n");
        return;
    } else if (findTaskByName(task->parent, taskName) != nullptr) {
        printf("\"%s\" already has subtask named \"%s\".\n\n", task->parent->name.c_str(), taskName.c_str());
        return;
    }
    printf("\"%s\" renamed to \"%s\"\n\n", task->name.c_str(), taskName.c_str());
    task->name = taskName;
}

/*
    Changes code of task pointed by 'task' to code inputed by user.
*/
void changeCode(Task* task) {
    string code;
    code = toUppercase(getToken(1));
    if (code.size() < 1 || code.size() > 5) {
        printf("Subtask code must be 1-5 characters long.\n\n");
        return;
    } else if (task->parent == nullptr) {
        printf("Can't change main task code.\n\n");
        return;
    } else if (codeUsed(rootTask, code)) {
        printf("Code \"%s\" is not available.\n\n", code.c_str());
        return;
    }
    printf("\"%s\" code changed to \"%s\"\n\n", task->code.c_str(), code.c_str());
    task->code = code;
}

/*
    Changes status of a subtask of task pointed by 'task' to status inputed by
    user.
*/
void setSubtaskStatus(Task *task) {
    char *statusName;
    Task *subtask;
    int status;
    subtask = getSubtask(task);
    statusName = toLowercase(getToken(2));
    if (strcmp(statusName, "active") == 0) {
        status = TASK_ACTIVE;
    } else if (strcmp(statusName, "inactive") == 0) {
        status = TASK_INACTIVE;
    } else if (strcmp(statusName, "completed") == 0) {
        status = TASK_COMPLETED;
    } else if (strcmp(statusName, "canceled") == 0) {
        status = TASK_CANCELED;
    } else {
        printf("\"%s\" is not a valid status.\n\n", statusName);
        return;
    }
    if (subtask == nullptr) return;
    if (status == TASK_COMPLETED && !allCompleted(subtask)) {
        printf("You can only complete \"%s\" when all its subtasks are completed.\n\n", subtask->name.c_str());
        return;
    }
    subtask->status = status;
    printf("\"%s\" is set to \"%s\"\n\n", subtask->name.c_str(), statusName);
}

/*
    Ask user for note content and add it in task pointed by 'task' notes.
*/
void addNote(Task *task) {
    string noteText;
    noteText = getToken(1);
    if (noteText.size() == 0) {
        printf("Note text must not be empty.\n\n");
        return;
    }
    Note *note = new Note;
    note->date = getCurrentTime();
    note->text = noteText;
    task->notes.push_back(note);
    printf("Added note\n\n");
}

/*
    Ask user for note ID and remove it from task pointed by 'task'.
*/
void removeNote(Task* task) {
    int id = atoi(getToken(1)) - 1;

    Note *note = ithNote(task->notes, id);

    if (note == nullptr) return;

    printf("Note %d removed.\n\n", id + 1);

    task->notes.remove(note);
}

/*
    Print list of notes of task pointed by 'task'.
*/
void listNotes(Task* task) {
    int i = 1;
    struct tm *stm;
    printTitle("Notes", MAIN_LEVEL);
    if (task->notes.size() == 0) {
        printf("  |   No notes\n");
    }
    for (auto it = task->notes.begin(); it != task->notes.end(); it++) {
        Note *note = *it;
        stm = localtime(&(note->date));
        cout << "    " << getColor(BRIGHT_BLUE) << setw(2) << i << "." << getColor(BRIGHT_WHITE);
        cout << getColor(BRIGHT_CYAN) << " (" << setw(2) << stm->tm_mday << "/" << setw(2) << stm->tm_mon + 1 << "/" << setw(4) << stm->tm_year + 1900 << ") " << getColor(BRIGHT_WHITE);
        cout << note->text << "\n" << endl;
        i++;
    }
    printLine(MAIN_LEVEL);
}

#include <unistd.h>

/*
    Opens a text editor to edit the description of 'task'.
*/
void editDescription(Task* task) {
    fstream file;
    stringstream stream;
    file.open("temp.txt", fstream::out);
    file << task->description;
    file.close();
    system("nvim temp.txt");
    file.open("temp.txt", fstream::in);
    
    stream << file.rdbuf();
    task->description = stream.str();
    int end = task->description.size() - 1;
    if (end >= 0) {
        if (task->description[end] != '\n') {
            task->description += '\n';
        } else {
            while (end > 0 && task->description[end - 1] == '\n') end--;
            task->description = task->description.substr(0, end + 1);
        }
    }
    system("rm temp.txt");
}

void printDescription(Task *task) {
    uint size = task->description.size();
    if (size == 0) return;
    cout << "  ";
    for (uint i = 0; i < size; i++) {
        char c = task->description[i];
        if (c == '#') cout << "\033[94m";
        if (c == '*') cout << colorString("*", BRIGHT_BLUE);
        else cout << c;
        if (i != size - 1 && c == '\n') {
            cout << "  ";
            cout << "\033[0m";
        }
    }
    cout << "\n\n";
}

/*
    Enters in subtask menu of task pointed by 'task'.
*/
void subtasksMenu(Task* task) {
    char *commandName;
    printTitle("Sub-objetivos - " + task->code, MAIN_LEVEL);
    listSubtasks(task);
    cout << "\n";
    printLine(MAIN_LEVEL);
    while (true) {
        commandName = getCommandName();
        if (strcmp(commandName, "add") == 0) {
            if (validArgs(2)) {
                addSubtask(task);
                saveAll();
            }
        } else if (strcmp(commandName, "rem") == 0) {
            if (validArgs(1)) {
                removeSubtask(task);
                saveAll();
            }
        } else if (strcmp(commandName, "select") == 0) {
            if (validArgs(1)) selectSubtask(task);
        } else if (strcmp(commandName, "move") == 0) {
            if (validArgs(0)) {
                moveTask(task);
                saveAll();
            }
        } else if (strcmp(commandName, "sts") == 0) {
            if (validArgs(0)) listSubtasks(task);
        } else if (strcmp(commandName, "cd") == 0) {
            if (validArgs(1)) {
                if (strcmp("..", getToken(1)) == 0) {
                    curMenu = TASK_MENU;
                    return;
                } else {
                    printf("Digite 'cd ..' para voltar\n\n");
                }
            }
        } else if (generalCommands(commandName)) return;
    }
}

void displayObjectiveStats(Task *task) {
    list<Period *> periods;
    getPeriodsFromTask(periods, task);
    periods.sort(periodComp);

    time_t startTime = 0;
    time_t endTime = 0;
    time_t totalTime = 0;
    time_t lastWeekTime = 0;
    time_t currentWeekTime = 0;

    time_t curTime = getCurrentTime();
    time_t objStart = getTime(25, 8, 2019, 0, 0, 0);

    time_t lastWeekEnd = curTime - (curTime - objStart) % SECS_IN_A_WEEK;
    time_t lastWeekStart = lastWeekEnd - SECS_IN_A_WEEK;
    time_t curWeekStart = curTime - (curTime - objStart) % SECS_IN_A_WEEK;
    time_t curWeekEnd = curWeekStart + SECS_IN_A_WEEK;

    for (auto it = periods.begin(); it != periods.end(); it++) {
        Period *period = *it;
        totalTime += period->end - period->start;
        lastWeekTime += periodIntersect(period->start, period->end, lastWeekStart, lastWeekEnd);
        currentWeekTime += periodIntersect(period->start, period->end, curWeekStart, curWeekEnd);
    }

    if (!periods.empty()) {
        startTime = periods.front()->start;
        endTime = periods.back()->end;
    }

    cout << " - " << task->name << "\n\n";
    cout << "  Primeiro período: " << formatDate(startTime, true) << "\n";
    cout << "  Último período: " << formatDate(endTime, true) << "\n";
    cout << "  Tempo total: " << formatDur(totalTime) << "\n";
    cout << "  Tempo na última semana: " << formatDur(lastWeekTime) << "\n";
    cout << "  Tempo na semana atual: " << formatDur(currentWeekTime) << "\n\n";
}

/*
    Enters in task menu of task pointed by 'task'.
*/
void taskMenu(Task* task) {
    char *commandName;
    bool showHead = true;
    while (true) {
        if (showHead) {
            printTitle("Objetivo - " + task->code, MAIN_LEVEL);
            displayObjectiveStats(task);
            printTitle("Sub-objetivos", SECONDARY_LEVEL, false);
            listSubtasks(task);
            printLine(SECONDARY_LEVEL);
            printDescription(task);
            showHead = false;
        }
        printLine(MAIN_LEVEL);
        periodWarning();
        commandName = getCommandName();
        if (strcmp(commandName, "rename") == 0) {
            if (validArgs(1)) {
                renameTask(task);
                saveAll();
            }
        } else if (strcmp(commandName, "code") == 0) {
            if (validArgs(1)) {
                changeCode(task);
                saveAll();
            }
        } else if (strcmp(commandName, "set") == 0) {
            if (validArgs(2)) {
                setSubtaskStatus(task);
                saveAll();
            }
        } else if (strcmp(commandName, "noteadd") == 0) {
            if (validArgs(1)) {
                addNote(task);
            }
        } else if (strcmp(commandName, "noterem") == 0) {
            if (validArgs(1)) {
                removeNote(task);
            }
        } else if (strcmp(commandName, "pds") == 0) {
            if (task == rootTask) {
                printf("Main task doesn't have periods.\n\n");
            } else {
                if (validArgs(0)) {
                    curMenu = PERIODS_MENU;
                    return;
                }
            }
        } else if (strcmp(commandName, "sts") == 0) {
            if (validArgs(0)) {
                curMenu = SUBTASKS_MENU;
                return;
            }
        } else if (strcmp(commandName, "tds") == 0) {
            if (validArgs(0)) {
                curMenu = TODOS_MENU;
                return;
            }
        } else if (strcmp(commandName, "nts") == 0) {
            if (validArgs(0)) {
                listNotes(task);
            }
        } else if (strcmp(commandName, "desc") == 0) {
            if (validArgs(0)) {
                editDescription(task);
                showHead = true;
            }
        // } else if (strcmp(commandName, "ls") == 0) {
        //     if (validArgs(0)) {
        //         listSubtasks(task);
        //         listTodos(task, 0);
        //     }
        } else if (strcmp(commandName, "cd") == 0) {
            if (validArgs(1)) {
                if (strcmp("..", getToken(1)) == 0) {
                    if (task == rootTask) {
                        printf("Can't go to main task parent.\n\n");
                    } else {
                        curTask = task->parent;
                        return;
                    }
                } else {
                    Task *nextTask = getSubtask(task);
                    if (nextTask != nullptr) {
                        curTask = nextTask;
                        return;
                    }
                }
            }
        } else if (generalCommands(commandName)) return;
    }
}
