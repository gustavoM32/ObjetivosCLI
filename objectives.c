#include <stdio.h>
#include <stdlib.h>

#include "objectives.h"
#include "util.h"
#include "task.h"
#include "io.h"
#include "calendar.h"

void freeAll() {
	freeTask(rootTask);
    free(calendar);
}

int main() {
    Task* curTask;
    calendar = createCalendar();
    printf("*** Welcome to Control! ***\n\n");
    printf("Type 'help' for a list of commands.\n\n");
    
    rootTask = createTask("Objetivos", "OBJ");
    loadAll();
    curTask = rootTask;
    while (curTask != NULL) {
        curTask = taskMenu(curTask);
    }
	freeAll();
    return EXIT_SUCCESS;
}
