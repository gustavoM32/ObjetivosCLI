#include <stdio.h>
#include "help.hpp"
#include "objectives.hpp"

void printHelp(int page) {
    const char* lineStart = "  |";
    const char* nameStart = "       ";
    const char* itemStart = "    *";
    printf("  +--------------------------------------------------------------------+\n");
    switch (page) {
        case TASK_MENU:
            printf("%s%s Task Menu commands\n", lineStart, nameStart);
            printf("%s\n", lineStart);
            printf("%s%s rename \"new name\" - rename task\n", lineStart, itemStart);
            printf("%s%s code <new code> - change task code\n", lineStart, itemStart);
            printf("%s%s set <task code> <status> - set subtask status\n", lineStart, itemStart);
            printf("%s%s active, inactive, completed\n", lineStart, itemStart);
            printf("%s%s time - show total time spent in task\n", lineStart, itemStart);
            printf("%s%s weektime - show time spent in task this week\n", lineStart, itemStart);
            printf("%s%s noteadd \"note text\" - add note\n", lineStart, itemStart);
            printf("%s%s noterem <note id> - remove note\n", lineStart, itemStart);
            printf("%s%s pds - manage periods\n", lineStart, itemStart);
            printf("%s%s sts - manage subtasks\n", lineStart, itemStart);
            printf("%s%s tds - manage to-do list\n", lineStart, itemStart);
            printf("%s%s nts - show note list\n", lineStart, itemStart);
            printf("%s%s cal - show calendar\n", lineStart, itemStart);
            printf("%s%s week - print week summary\n", lineStart, itemStart);
            printf("%s%s alltds - show all to-dos with no date\n", lineStart, itemStart);
            printf("%s%s ls - list subtasks and to-dos\n", lineStart, itemStart);
            printf("%s%s cd <task code> - open the subtask menu\n", lineStart, itemStart);
            printf("%s%s cd .. - go back to parent task\n", lineStart, itemStart);
            printf("%s%s goto ... [<parent code>] <task code> - open task menu\n", lineStart, itemStart);
            printf("%s%s help - show this menu\n", lineStart, itemStart);
            printf("%s%s save - save all data\n", lineStart, itemStart);
            printf("%s%s exit - exit program\n", lineStart, itemStart);
            break;
        case SUBTASKS_MENU:
            printf("%s%s Subtask Menu commands\n", lineStart, nameStart);
            printf("%s\n", lineStart);
            printf("%s%s add <task code> \"task name\" - add new subtask\n", lineStart, itemStart);
            printf("%s%s rem <task code> - remove a subtask\n", lineStart, itemStart);
            printf("%s%s select <task code> - select a subtask\n", lineStart, itemStart);
            printf("%s%s move - move selected subtask to current subtask list\n", lineStart, itemStart);
            printf("%s%s sts - list subtasks\n", lineStart, itemStart);
            printf("%s%s help - show this menu\n", lineStart, itemStart);
            printf("%s%s cd .. - go back to task menu\n", lineStart, itemStart);
            break;
        case TODOS_MENU:
            printf("%s%s To-do Menu commands\n", lineStart, nameStart);
            printf("%s\n", lineStart);
            printf("%s%s add \"name\" - add a to-do\n", lineStart, itemStart);
            printf("%s%s addesp <start-end> \"name\" - add to-dos with numbers\n", lineStart, itemStart);
            printf("%s%s rem <id> - remove a to-do\n", lineStart, itemStart);
            printf("%s%s tds - list to-dos\n", lineStart, itemStart);
            printf("%s%s set <id> <dd/mm> - change to-do planned date\n", lineStart, itemStart);
            printf("%s%s setesp <start-end> <dd/mm> - change to-do planned date\n", lineStart, itemStart);
            printf("%s%s start - start a period\n", lineStart, itemStart);
            printf("%s%s stop - stop period\n", lineStart, itemStart);
            printf("%s%s cancel - cancel period\n", lineStart, itemStart);
            printf("%s%s time - time spent in current period\n", lineStart, itemStart);
            printf("%s%s help - show this menu\n", lineStart, itemStart);
            printf("%s%s cd .. - go back to task menu\n", lineStart, itemStart);
            break;
        case PERIODS_MENU:
            printf("%s%s Period Menu commands\n", lineStart, nameStart);
            printf("%s\n", lineStart);
            printf("%s%s pds - list periods\n", lineStart, itemStart);
            printf("%s%s set <id> <start/end> <time> <hh:mm:ss> or\n", lineStart, itemStart);
            printf("%s%s set <id> <start/end> <date> <dd/mm/yy> - edit period\n", lineStart, itemStart);
            printf("%s%s add <dd/mm/yy> <hh:mm:ss> <dd/mm/yy> <hh:mm:ss> - add period\n", lineStart, itemStart);
            printf("%s%s rem <id> - remove period\n", lineStart, itemStart);
            printf("%s%s help - show this menu\n", lineStart, itemStart);
            printf("%s%s cd .. - go back to task menu\n", lineStart, itemStart);
            break;
        // case WEEK_MENU:
        //     printf("%s%s Week Menu commands\n", lineStart, nameStart);
        //     printf("%s\n", lineStart);
        //     printf("%s%s add \"day\" \"month\" \"task name\" - add week task\n", lineStart, itemStart);
        //     printf("%s%s rem \"day\" \"month\" \"id\" - remove week task\n", lineStart, itemStart);
        //     printf("%s%s help - show this menu\n", lineStart, itemStart);
        //     printf("%s%s cd .. - go back to previous menu\n", lineStart, itemStart);
        //     break;
    }
    printf("%s\n", lineStart);
    printf("  +--------------------------------------------------------------------+\n\n");
}
