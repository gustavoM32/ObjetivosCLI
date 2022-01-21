#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
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
    string subtaskMotivation;
    subtaskCode = toUppercase(getToken(1));
    subtaskName = getToken(2);
    subtaskMotivation = getToken(3);

    if (subtaskCode.size() < 1 || subtaskCode.size() > 5) {
        printf("Subtask code must be 1-5 characters long.\n\n");
        return;
    }
    
    if (codeUsed(rootTask, subtaskCode)) {
        printf("Code \"%s\" is not available.\n\n", subtaskCode.c_str());
        return;
    }

    if (subtaskName.size() == 0) {
        printf("Subtask name must not be empty.\n\n");
        return;
    }
    
    if (findTaskByName(parent, subtaskName) != nullptr) {
        printf("\"%s\" already has subtask named \"%s\".\n\n", parent->name.c_str(), subtaskName.c_str());
        return;
    }

    if (subtaskMotivation.size() == 0) {
        cout << "Motivação não pode ser vazia\n\n";
        return;
    }

    Task *task = createTask(subtaskName.c_str(), subtaskCode.c_str());
    task->parent = parent;

    Note *note = new Note;
    note->date = getCurrentTime();
    note->text = subtaskMotivation;
    note->motivation = true;
    task->history.push_back(note);

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

    if (calendar->periodSched != nullptr) {
        cout << "Não é possível remover objetivo enquanto um período está rodando\n\n";
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
    string statusNames[4] = {"active", "inactive", "canceled", "completed"};
    list<Task *> lists[4];
    for (auto it = task->subtasks.begin(); it != task->subtasks.end(); it++) {
        Task *subtask = *it;
        int status = subtask->status;
        lists[status].push_back(subtask);
    }
    cout << "    ";
    if ((lists[0].size() + lists[1].size() + lists[2].size() + lists[3].size()) == 0) {
        cout << colorString("Não há sub-objetivos.\n", "CYAN");
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
    Changes color of task pointed by 'task' to code inputed by user.
*/
void changeColor(Task* task) {
    string color;
    color = toUppercase(getToken(1));
    if (colors.find(color) == colors.end()) {
        cout << "Cor inválida, digite 'colors' para ver cores disponíveis.\n\n";
        return;
    }

    cout << "\"" << task->code << "\" color changed to \"" << color << "\"\n\n";
    task->color = color;
}

void listColors() {
    for (auto color : colors) {
        cout << colorString("  * ", "BLUE") << colorString(color.first, color.first) << "\n";
    }
    cout << "\n";
}

/*
    Changes status of a subtask of task pointed by 'task' to status inputed by
    user.
*/
void setSubtaskStatus(Task *task) {
    string statusName;
    Task *subtask;
    int status;
    subtask = getSubtask(task);
    statusName = toLowercase(getToken(2));
    if (statusName == "active") {
        status = TASK_ACTIVE;
    } else if (statusName == "inactive") {
        status = TASK_INACTIVE;
    } else if (statusName == "canceled") {
        status = TASK_CANCELED;
    } else if (statusName == "completed") {
        status = TASK_COMPLETED;
    } else {
        printf("\"%s\" is not a valid status.\n\n", statusName.c_str());
        return;
    }
    if (subtask == nullptr) return;
    if (status == TASK_COMPLETED && !allCompleted(subtask)) {
        printf("You can only complete \"%s\" when all its subtasks are completed.\n\n", subtask->name.c_str());
        return;
    }
    subtask->status = status;
    printf("\"%s\" is set to \"%s\"\n\n", subtask->name.c_str(), statusName.c_str());
}

/*
    Ask user for history point content and add it in task pointed by 'task' history.
*/
void addNote(Task *task, bool motivation = false) {
    string noteText;
    noteText = getToken(getNComms() - 1);
    if (noteText.size() == 0) {
        printf("Note text must not be empty.\n\n");
        return;
    }
    Note *note = new Note;
    note->date = getCurrentTime();
    note->text = noteText;
    note->motivation = motivation;
    task->history.push_back(note);
    printf("Added note\n\n");
}

/*
    Ask user for note ID and remove it from task pointed by 'task'.
*/
void removeNote(Task* task) {
    int id = stoi(getToken(getNComms() - 1)) - 1;

    Note *note = ithNote(task->history, id);

    if (note == nullptr) return;

    printf("Note %d removed.\n\n", id + 1);

    task->history.remove(note);
}

/*
    Ask user for note ID and toggle it between motivation or not.
*/
void toggleNote(Task* task) {
    int id = stoi(getToken(getNComms() - 1)) - 1;

    Note *note = ithNote(task->history, id);

    if (note == nullptr) return;

    note->motivation = !note->motivation;

    if (note->motivation) cout << "Note " << id + 1 << " is now a motivation.\n";
    else cout << "Note " << id + 1 << " is now normal.\n";
}
/*
    Print history of task pointed by 'task'.
*/
void printHistory(Task* task) {
    int i = 1;
    printTitle("Histórico", MAIN_LEVEL);
    if (task->history.size() == 0) {
        printf("  Sem histórico\n\n");
        return;
    }
    for (auto it = task->history.begin(); it != task->history.end(); it++) {
        Note *note = *it;
        cout << setfill(' ') << "    " << getColor("BRIGHT_BLUE") << setw(2) << i << ". " << getColor("BRIGHT_WHITE");
        cout << colorString("(" + formatDate(note->date, true) + ") ", "BRIGHT_CYAN");
        if (note->motivation) cout << "(Motivação) ";
        cout << note->text << "\n";
        i++;
    }
    cout << "\n";
    printLine(MAIN_LEVEL);
}

/*
    Opens a text editor to edit the plan of 'task'.
*/
void editPlan(Task* task) {
    fstream file;
    stringstream stream;
    file.open("temp.md", fstream::out);
    file << task->plan;
    file.close();
    system("nvim temp.md");
    file.open("temp.md", fstream::in);
    
    stream << file.rdbuf();
    task->plan = stream.str();
    int end = task->plan.size() - 1;
    if (end >= 0) {
        if (task->plan[end] != '\n') {
            task->plan += '\n';
        } else {
            while (end > 0 && task->plan[end - 1] == '\n') end--;
            task->plan = task->plan.substr(0, end + 1);
        }
    }
    system("rm temp.md");
}

void printPlan(Task *task) {
    uint size = task->plan.size();
    if (size == 0) return;
    cout << "  ";
    int lineLen = 2;
    string palavra;
    for (uint i = 0; i < size; i++) {
        char c = task->plan[i];

        if (c == '#') cout << getColor("BRIGHT_BLUE");

        if (c == '*') {
            cout << colorString("*", "BRIGHT_BLUE");
            lineLen++;
        } else if (c == '\n') {
            if (lineLen + palavra.size() > LINE_LEN) {
                cout << "\n" << palavra << "\n";
            } else {
                cout << palavra << "\n";
            }
            lineLen = 0;
            palavra.clear();
            if (i != size - 1) {
                cout << "  ";
                lineLen = 2;
                cout << getColor("BRIGHT_WHITE");
            }
        } else if (c == ' ') {
            if (lineLen + palavra.size() > LINE_LEN) {
                cout << "\n" << palavra << " ";
                lineLen = palavra.size() + 1;
            } else {
                cout << palavra << " ";
                lineLen += palavra.size() + 1;
            }
            palavra.clear();
        } else {
            palavra += c;
        }
    }
    cout << "\n";
}

/*
    Enters in subtask menu of task pointed by 'task'.
*/
void subtasksMenu(Task* task) {
    string commandName;
    printTitle("Sub-objetivos - " + task->code, MAIN_LEVEL);
    listSubtasks(task);
    cout << "\n";
    printLine(MAIN_LEVEL);
    while (true) {
        commandName = getCommandName();
        if (commandName == "add") {
            if (validArgs(3)) {
                addSubtask(task);
                saveAll();
            }
        } else if (commandName == "rem") {
            if (validArgs(1)) {
                removeSubtask(task);
                saveAll();
            }
        } else if (commandName == "select") {
            if (validArgs(1)) selectSubtask(task);
        } else if (commandName == "move") {
            if (validArgs(0)) {
                moveTask(task);
                saveAll();
            }
        } else if (commandName == "sts") {
            if (validArgs(0)) listSubtasks(task);
        } else if (commandName == "cd") {
            if (validArgs(1)) {
                if (getToken(1) == "..") {
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

    cout << colorString("  - " + task->name, getTaskColor(task)) << "\n\n";
    cout << colorString("  Motivação: ", "BRIGHT_BLUE") << getMotivation(task) << "\n\n";
    printRecentHistory(task);
    cout << "  " << colorString("Atividade: ", "BRIGHT_BLUE");

    if (!periods.empty()) {
        startTime = periods.front()->start;
        endTime = periods.back()->end;
        cout << colorString(formatDate(startTime, true) + " - " + formatDate(endTime, true), "BRIGHT_CYAN");
    } else {
        cout << colorString("Não iniciado", "BRIGHT_CYAN");
    }
    cout << "        ";
    cout << colorString("Tempo total: ", "BRIGHT_BLUE") << colorString(formatDur(totalTime), "BRIGHT_CYAN") << "        ";
    cout << colorString("Última semana: ", "BRIGHT_BLUE") << colorString(formatDur(lastWeekTime), "BRIGHT_CYAN") << "        ";
    cout << colorString("Semana atual: ", "BRIGHT_BLUE") << colorString(formatDur(currentWeekTime), "BRIGHT_CYAN") << "\n\n";
}

void showTaskInfo(Task *task) {
    printTitle("Objetivo - " + task->code, MAIN_LEVEL);
    displayObjectiveStats(task);
    printTitle("Sub-objetivos", SECONDARY_LEVEL, false);
    listSubtasks(task);
    printLine(SECONDARY_LEVEL);
    printPlan(task);
}

/*
    Enters in task menu of task pointed by 'task'.
*/
void taskMenu(Task* task) {
    string commandName;
    bool showHead = true;
    while (true) {
        if (showHead) {
            showTaskInfo(task);
            showHead = false;
        }
        printLine(MAIN_LEVEL);
        periodWarning();
        commandName = getCommandName();
        if (commandName == "rename") {
            if (validArgs(1)) {
                renameTask(task);
                saveAll();
                showHead = true;
            }
        } else if (commandName == "code") {
            if (validArgs(1)) {
                changeCode(task);
                saveAll();
                showHead = true;
            }
        } else if (commandName == "color") {
            if (validArgs(1)) {
                changeColor(task);
                saveAll();
                showHead = true;
            }
        } else if (commandName == "colors") {
            if (validArgs(0)) {
                listColors();
            }
        } else if (commandName == "set") {
            if (validArgs(2)) {
                setSubtaskStatus(task);
                saveAll();
                showHead = true;
            }
        } else if (commandName == "history") {
            if (getNComms() == 1) {
                if (validArgs(0)) {
                   printHistory(task);
                }
            } else if (getToken(1) == "add") {
                if (validArgs(2)) {
                    addNote(task);
                    saveAll();
                    showHead = true;
                }
            } else if (getToken(1) == "remove") {
                if (validArgs(2)) {
                    removeNote(task);
                    showHead = true;
                }
            } else if (getToken(1) == "toggle") {
                if (validArgs(2)) {
                    toggleNote(task);
                    showHead = true;
                }
            } else cout << "Opção inválida\n\n";
        } else if (commandName == "motivation") {
            if (validArgs(1)) {
                addNote(task, true);
                showHead = true;
            }
        } else if (commandName == "pds") {
            if (task == rootTask) {
                printf("Main task doesn't have periods.\n\n");
            } else {
                if (validArgs(0)) {
                    curMenu = PERIODS_MENU;
                    return;
                }
            }
        } else if (commandName == "sts") {
            if (validArgs(0)) {
                curMenu = SUBTASKS_MENU;
                return;
            }
        } else if (commandName == "tds") {
            if (validArgs(0)) {
                curMenu = TODOS_MENU;
                return;
            }
        } else if (commandName == "plan") {
            if (validArgs(0)) {
                editPlan(task);
                saveAll();
                showHead = true;
            }
        } else if (commandName == "ls") {
            if (validArgs(0)) {
                showTaskInfo(task);
            }
        } else if (commandName == "cd") {
            if (validArgs(1)) {
                if (getToken(1) == "..") {
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
