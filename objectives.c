#include <stdio.h>
#include <stdlib.h>

#include "objectives.h"
#include "util.h"
#include "task.h"
#include "io.h"
#include "calendar.h"

void freeAll() {
	free(todoCalendar);
	freeTask(rootTask);
}

int main() {
    Task* curTask;
    printf("*** Welcome to Control! ***\n\n");
    printf("Type 'help' for a list of commands.\n\n");
    rootTask = createTask("Objetivos", "OBJ");
    todoCalendar = createCalendar();
    loadAll();
    curTask = rootTask;
    while (curTask != NULL) {
        curTask = taskMenu(curTask);
    }
	freeAll();
    return EXIT_SUCCESS;
}
