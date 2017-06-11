#ifndef GAME_H
#define GAME_H

#include "minimaxtree.h"

struct GameReply { //структура ответа сервера "is_error:is_end_of_game:who_win:last_move:"
    bool is_error;
    bool is_end_of_game;
    int who_win;
    int last_move;

    GameReply():is_error(false),is_end_of_game(false),who_win(XO::NO_WIN),last_move(-1) {}
};

class Game{

public:
                Game(MiniMaxTree* set_game_tree);
    GameReply   startByXO(bool server_go_by_x); // реакция на старт игры клиента за X или O
    GameReply   goTo(int case_pos = -1); // ход в case_pos, если case_pos == -1, то выбирается оптимальный ход

    int         getServerPlaysBy();

private:
    MiniMaxNode*    current_position; // указатель на узел/лист дерева - текущую ситуацию
    MiniMaxTree*    game_tree;        // указатель на дерево ходов игры (память не выделяется)

    int             server_plays_by;

};

#endif // GAME_H
