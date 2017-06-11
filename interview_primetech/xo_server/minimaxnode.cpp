#include "minimaxnode.h"
#include "math.h"
#include <QTime>

using namespace XO;

MiniMaxNode::MiniMaxNode(MiniMaxNode* set_parent, int set_position) {
    parent = set_parent;
    position = set_position;
    move_by_x = false;
    count_of_moves = 0;
    if (!isRoot()) {
        parent->childrens.push_back(this);
        count_of_moves = parent->count_of_moves + 1;
        move_by_x=parent->move_by_x^true;
    }
    empirical_value = (move_by_x)?1000:-1000;
}

void MiniMaxNode::setEmpiricalValue(int value) {
    empirical_value = value;
}

void MiniMaxNode::setEmpiricalValueFromChilds() {
    // минимакс устанавливаем
    double (*minimax)(double, double) = (move_by_x)?&fmin:&fmax;
    empirical_value = (move_by_x)?1000:-1000;
    foreach(MiniMaxNode* i, childrens)
        if (minimax(empirical_value, i->empirical_value) != empirical_value)
            empirical_value = i->empirical_value;
}

MiniMaxNode* MiniMaxNode::getNextStrategyChild() {
    // минимакс считываем
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    if (childrens.isEmpty()) return NULL;
    QVector <MiniMaxNode*> may_be_next;
    double (*minimax)(double, double) = (move_by_x)?&fmin:&fmax;
    int minimax_value = childrens.at(0)->empirical_value;
    foreach(MiniMaxNode* i, childrens) {
        if (minimax(minimax_value, i->empirical_value) != minimax_value) {
            minimax_value = i->empirical_value;
            may_be_next.clear();
            may_be_next.push_back(i);
        }
        else if (minimax_value == i->empirical_value)
            may_be_next.push_back(i);
    }

    // если вариантов хода несколько - возвращаем random
    return may_be_next[qrand()%may_be_next.size()];
}

int MiniMaxNode::getPosition() {
    return position;
}

MiniMaxNode* MiniMaxNode::getNextChildByPosition(int pos) {
    foreach(MiniMaxNode* i, childrens)
        if (i->position == pos) return i;
    return NULL;
}

bool MiniMaxNode::isRoot() {
    return (parent == NULL);
}

MiniMaxNode::~MiniMaxNode() {
    foreach(MiniMaxNode* i, childrens)
        delete i;
    childrens.clear();
}

MiniMaxNode* MiniMaxNode::getParent() {
    return parent;
}


bool MiniMaxNode::isTerminalNode(int &xo_who_win) {
    // заполняем таблицу 3х3
    int table[3][3];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            table[i][j] = CLEAR_CELL;
    MiniMaxNode *next = this;
    while (!next->isRoot()) {
        table[next->position/3][next->position%3] = (next->move_by_x)?X_CELL:O_CELL;
        next = next->parent;
    }
    // проверяем конец игры
    for (int i = 0; i < 3; i++)
        if (abs(table[i][0] + table[i][1] + table[i][2]) == 3 || abs(table[0][i] + table[1][i] + table[2][i]) == 3) {
            xo_who_win = (move_by_x)?X_WIN:O_WIN;
            return true;
        }
    if (abs(table[0][0] + table[1][1] + table[2][2]) == 3 || abs(table[2][0] + table[1][1] + table[0][2]) == 3) {
        xo_who_win = (move_by_x)?X_WIN:O_WIN;
        return true;
    }
    if (count_of_moves == 9) {
        xo_who_win = NO_WIN;
        return true;
    }
    return false;
}
