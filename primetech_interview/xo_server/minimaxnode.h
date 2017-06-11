#ifndef MINIMAXNODE_H
#define MINIMAXNODE_H

#include <QVector>

namespace XO { // игровые константы
    const int X_WIN = 1;
    const int NO_WIN = 0;
    const int O_WIN = -1;

    const int X_CELL = 1;
    const int CLEAR_CELL = 0;
    const int O_CELL = -1;
}

class MiniMaxNode {

public:
                    MiniMaxNode(MiniMaxNode* set_parent, int set_xo_position = -1);
                    ~MiniMaxNode();    
    MiniMaxNode*    getParent();
    bool            isRoot();

    bool            isTerminalNode(int &xo_who_win); // проверяем конец игры
    MiniMaxNode*    getNextChildByPosition(int pos); // указатель на потомка, соотвтетсвующего этому ходу
    MiniMaxNode*    getNextStrategyChild();          // получаем потомка методом минимакс

    int             getPosition(); // position
    void            setEmpiricalValue(int value); // empirical_value
    void            setEmpiricalValueFromChilds(); // устанавливаем empirical_value методом минимакс

private:
    MiniMaxNode*            parent;
    QVector<MiniMaxNode*>   childrens;
    int                     position;
    int                     empirical_value;
    bool                    move_by_x;         // узел соответствует ходу X или O
    int                     count_of_moves;    // глубина
};

#endif // MINIMAXNODE_H
