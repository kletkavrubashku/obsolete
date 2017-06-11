#ifndef GAMEDIALOG_H
#define GAMEDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QStackedLayout>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTcpSocket>
#include <QStatusBar>

namespace XO {
    const int X_WIN = 1;
    const int NO_WIN = 0;
    const int O_WIN = -1;

    const int X_CELL = 1;
    const int CLEAR_CELL = 0;
    const int O_CELL = -1;
}

struct GameReply { //структура ответа сервера "is_error:is_end_of_game:who_win:last_move:"
    bool is_error;
    bool is_end_of_game;
    int who_win;         // X_WIN || O_WIN || NO_WIN
    int last_move;       // ответный ход сервера или последний клиента, если ходить некуда

    GameReply():is_error(false),is_end_of_game(false),who_win(XO::NO_WIN),last_move(-1) {}
};

class GameDialog : public QDialog {
    Q_OBJECT

public:
    GameDialog(QWidget *parent = 0);
    //~GameDialog() {} не нужен

private:
    QStackedLayout* main_layout;
    QTcpSocket* socket;

    QPushButton* connect_button;
    QComboBox* host_combo;
    QLineEdit* port_edit;

    QRadioButton* game_by_x;
    QRadioButton* game_by_o;
    QPushButton* start_button;
    QLabel* game_label;
    QPushButton* game_grid[9]; // игровые кнопки
    QStatusBar* status_bar;

    // компоновка визуальных компонентов
    void constructConnectDialog();
    void constructGameDialog(); // компоновка визуальных компонентов

    // отправка запроса и обработка ответа
    void sendRequest(QString str);
    void processReply(GameReply reply);

    // игровые переменные
    int last_client_turn;
    int client_plays_by;    // X_CELL || O_CELL || CLEAR_CELL

private slots:  

    // слоты графического интерфейса
    void changeConnectDialog();
    void changeGameDialog();
    void enableConnectButton();
    void enableStartButton();

    // слоты соединения
    void connectToHost();
    void displayError(QAbstractSocket::SocketError error);
    void disconnect();
    void readReply();

    // игровые слоты
    void startGame();
    void goTo();

};

#endif // GAMEDIALOG_H
