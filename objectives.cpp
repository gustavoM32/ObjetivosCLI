#include <stdio.h>
#include <stdlib.h>

#include "objectives.hpp"
#include "util.hpp"
#include "task.hpp"
#include "io.hpp"
#include "calendar.hpp"

Task* rootTask;
Calendar *calendar;

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
