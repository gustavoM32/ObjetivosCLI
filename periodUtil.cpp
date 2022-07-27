#include <iomanip>
#include <sstream>

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
