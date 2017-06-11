#include "game.h"

Game::Game(MiniMaxTree* set_game_tree) {
    server_plays_by = XO::CLEAR_CELL;
    game_tree = set_game_tree;
    current_position = game_tree->getRoot();
}

int Game::getServerPlaysBy() {
    return server_plays_by;
}

GameReply Game::goTo(int case_pos) {
    GameReply result;
    if (server_plays_by == XO::CLEAR_CELL) { // не ходим пока не выберем X или O
        result.is_error = true;
        return result;
    }    
    // выбираем потомка
    MiniMaxNode* try_node = (case_pos == -1)?current_position->getNextStrategyChild():current_position->getNextChildByPosition(case_pos);
    if (try_node) {
        current_position = try_node;
        result.last_move = current_position->getPosition();
        result.is_end_of_game = current_position->isTerminalNode(result.who_win);
    } else
        result.is_error = true;    
    return (result.is_error || result.is_end_of_game || case_pos == -1)?result:goTo(); // если обработали запрос клиента шлём ответный ход
}

GameReply Game::startByXO(bool server_go_by_x) {
    current_position = game_tree->getRoot();
    server_plays_by = (server_go_by_x)?XO::X_CELL:XO::O_CELL;
    GameReply result;
    return (server_go_by_x)?goTo():result; // если сервер-X, то отправляем ответный ход сервера
}

