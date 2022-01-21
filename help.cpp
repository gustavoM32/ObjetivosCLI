#include <iostream>
#include <vector>
#include "help.hpp"
#include "objectives.hpp"
#include "util.hpp"

using namespace std;

void printHelp(int page) {
    vector<string> helpItems;

    printTitle("Ajuda", SECONDARY_LEVEL);
    if (page == TASK_MENU) {
        cout << colorString("    # Comandos do menu do objetivo\n\n", "CYAN");
        helpItems.push_back("rename \"nome\" - renomeia objetivo");
        helpItems.push_back("code <código> - muda código do objetivo");
        helpItems.push_back("color <cor> - muda cor do objetivo");
        helpItems.push_back("colors - lista cores disponíveis");
        helpItems.push_back("set <código> <estado> - muda estado do objetivo (active, inactive, completed, canceled)");
        helpItems.push_back("history add \"texto\" - adiciona anotação");
        helpItems.push_back("history remove <id> - remove anotação");
        helpItems.push_back("history toggle <id> - alterna entre anotação ser motivação ou não");
        helpItems.push_back("motivation \"texto\" - adiciona anotação");
        helpItems.push_back("pds - abre menu de períodos");
        helpItems.push_back("sts - abre menu de sub-objetivos");
        helpItems.push_back("tds - abre menu de to-dos");
        helpItems.push_back("history - mostra histórico");
        helpItems.push_back("plan - edita plano do objetivo");
        helpItems.push_back("ls - lista to-dos");
        helpItems.push_back("cd <código> - abre o menu de um sub-objetivo");
        helpItems.push_back("cd .. - vai para o objetivo pai");
    } else if (page == SUBTASKS_MENU) {
        cout << colorString("    # Comandos do menu de sub-objetivos\n\n", "CYAN");
        helpItems.push_back("add <código> \"nome\" \"motivação\" - adiciona sub-objetivo");
        helpItems.push_back("rem <código> - remove um sub-objetivo");
        helpItems.push_back("select <código> - seleciona um sub-objetivo");
        helpItems.push_back("move - torna o objetivo selecionado filho do objetivo atual");
        helpItems.push_back("sts - lista sub-objetivos");
        helpItems.push_back("cd .. - volta para o menu do objetivo");
    } else if (page == TODOS_MENU) {
        cout << colorString("    # Comandos do menu de to-dos\n\n", "CYAN");
        helpItems.push_back("add \"name\" - adiciona um to-do");
        // helpItems.push_back("addesp <start-end> \"name\" - add to-dos with numbers");
        helpItems.push_back("rem <id> - remove um to-do");
        helpItems.push_back("edit name <id> \"nome\" - edita o nome de um to-do");
        helpItems.push_back("move inside|before <idOrigem> <idDestino> - move um to-do");
        helpItems.push_back("sched <id> [<estimativa> [<data> [<hora>]]] - agenda um to-do");
        helpItems.push_back("tds [all] - lista to-dos");
        helpItems.push_back("set <id> [<esp> <idPai> <inicio-fim>] - prioriza um to-do");
        helpItems.push_back("clear <id> [<esp> <idPai> <inicio-fim>] - desprioriza um to-do");
        helpItems.push_back("complete <id> [<esp> <idPai> <inicio-fim>] - completa um to-do");
        helpItems.push_back("hide <id> [<esp> <idPai> <inicio-fim>] - completa e esconde um to-do");
        helpItems.push_back("cd .. - volta para o menu do objetivo");
    } else if (page == PERIODS_MENU) {
        cout << colorString("    # Comandos do menu de períodos\n\n", "CYAN");
        helpItems.push_back("pds [all] - lista períodos");
        helpItems.push_back("edit start|end <hora> <id> <hh:mm:ss> ou");
        helpItems.push_back("edit start|end <data> <id> <dd/mm/yy> - edita período");
        helpItems.push_back("rem <id> - remove período");
        helpItems.push_back("reduce <minutos> - remove tempo do último período");
        helpItems.push_back("cd .. - volta para o menu do objetivo");
    } else if (page == CALENDAR_MENU) {
        cout << colorString("    # Comandos do calendário\n\n", "CYAN");
        helpItems.push_back("time <id> [hora] - edita horário de um agendamento");
        helpItems.push_back("date <id> [data] - edita data de um agendamento");
        helpItems.push_back("est <id> estimativa - estima tempo para um agendamento");
        helpItems.push_back("post <id> <dias> [keep] - adia um agendamento em dias");
        helpItems.push_back("delay <id> <horas> - adia um agendamento em horas");
        helpItems.push_back("clear <id> - remove um agendamento");
        helpItems.push_back("complete <id> - completa o to-do relacionado a um agendamento");
        helpItems.push_back("start <id> - inicia um período");
        helpItems.push_back("stop - para período");
        helpItems.push_back("cancel - cancela período");
        helpItems.push_back("status - mostra tempo gasto no período atual");
        helpItems.push_back("habit <id> - adiciona atividade em um hábito");
        helpItems.push_back("habits - mostra todos os hábitos");
        helpItems.push_back("cal - mostra calendário");
        helpItems.push_back("cd .. - go back to previous menu");
    }

    helpItems.push_back("goto <código> - vai para objetivo");
    helpItems.push_back("cal - vai para o calendário");
    helpItems.push_back("week [all] - mostra resumo semanal");
    helpItems.push_back("tree [inactive|incomplete|all] - mostra árvore de objetivos");
    helpItems.push_back("help - mostra esse menu");
    helpItems.push_back("save - salva todos os dados");
    helpItems.push_back("exit - sai do programa");
    
    for (auto it = helpItems.begin(); it != helpItems.end(); it++) {
        cout << colorString("    * ", "BRIGHT_BLUE") << *it << endl;
    }
    cout << "\n";
    printLine(SECONDARY_LEVEL);
}
