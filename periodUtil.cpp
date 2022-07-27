#include <iomanip>
#include <iostream>
#include <sstream>
#include <list>

#include "io.hpp"
#include "objectives.hpp"
#include "periodUtil.hpp"
#include "util.hpp"

using namespace std;

string periodToString(Period *p, bool showGoal) {
    string formatedStartTime = formatTime(p->start);
    string formatedEndTime = formatTime(p->end);
    string formatedDur = formatDur(p->end - p->start);
    string periodName;

    if (showGoal) periodName = getTodoFullName(p->todo);
    else periodName = getTodoPath(p->todo);

    stringstream ss;
    ss << "[" << formatedStartTime << " ... " << formatedEndTime << "]   " << setw(8) << formatedDur << "   " << periodName;
    return ss.str();
}

void printAllPeriods() {
    list<Period *> periods;
    getPeriodsFromTask(periods, rootTask);
    periods.sort(periodComp);

    int numPeriods;
    int totalPeriods = periods.size();

    if (getNComms() == 2) {
        string arg = getToken(1);
        if (arg == "all") numPeriods = totalPeriods;
        else {
            numPeriods = stoi(arg);
            if (numPeriods < 1) {
                cout << "É necessário imprimir um número positivo de períodos\n\n";
                return;
            }
            numPeriods = min(numPeriods, totalPeriods);
        }
    } else numPeriods = 20;

    printTitle("Períodos de todos objetivos", SECONDARY_LEVEL);

    cout << "  Mostrando " << numPeriods << " mais recentes de um total de " << totalPeriods << "\n\n";

    while (int(periods.size()) > numPeriods) {
        periods.pop_front();
    }

    if (periods.size() == 0) {
        printf("    Não há períodos.\n");
    }

    int i = totalPeriods - numPeriods;

    for (Period *p : periods) {
        cout << "    " << getColor("CYAN") << setw(3) << ++i << "." << getColor("BRIGHT_WHITE") << " " << periodToString(p, true) << "\n";
    }
    cout << "\n";
}
