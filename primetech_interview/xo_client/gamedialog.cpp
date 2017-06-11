#include "gamedialog.h"

#include <QNetworkInterface>
#include <QMessageBox>
#include <QHostInfo>

GameDialog::GameDialog(QWidget *parent)
    : QDialog(parent) {
    last_client_turn = -1;
    client_plays_by = XO::CLEAR_CELL;

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(changeGameDialog()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readReply()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));

    setWindowTitle(tr("XO Client"));
    main_layout = new QStackedLayout;
    setLayout(main_layout);

    constructConnectDialog();
    constructGameDialog();  

    changeConnectDialog();
}

void GameDialog::disconnect() {
    QMessageBox::critical(this, tr("XO Client Critical Error"),
                             tr("You was disconnected by server."));
    changeConnectDialog(); // возвращаемся к диалогу подключения
}

void GameDialog::constructConnectDialog() {
    QLabel* header = new QLabel(tr("Change connection settings"));
    header->setAlignment(Qt::AlignCenter);
    QFont f;
    f.setBold(true);
    header->setFont(f);
    host_combo = new QComboBox;
    host_combo->setEditable(true);
    // ищем сетевые интерфейсы для комбобокса
    QString name = QHostInfo::localHostName();
    if (!name.isEmpty()) {
        host_combo->addItem(name);
        QString domain = QHostInfo::localDomainName();
        if (!domain.isEmpty())
            host_combo->addItem(name + QChar('.') + domain);
    }
    if (name != QString("localhost"))
        host_combo->addItem(QString("localhost"));
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (!ipAddressesList.at(i).isLoopback())
            host_combo->addItem(ipAddressesList.at(i).toString());
    }
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i).isLoopback())
            host_combo->addItem(ipAddressesList.at(i).toString());
    }
    //

    port_edit = new QLineEdit;
    port_edit->setValidator(new QIntValidator(1, 65535, this));
    connect_button = new QPushButton(tr("&Connect"));
    connect_button->setEnabled(false);    

    QFormLayout *form_layout = new QFormLayout;

    form_layout->addRow(tr("&Server name:"), host_combo);
    form_layout->addRow(tr("S&erver port:"), port_edit);

    QVBoxLayout* connect_layout = new QVBoxLayout;
    connect_layout->addStretch(1);
    connect_layout->addWidget(header);
    connect_layout->addLayout(form_layout);
    connect_layout->addWidget(connect_button);
    connect_layout->addStretch(1);

    QWidget* connect_widget = new QWidget;
    connect_widget->setLayout(connect_layout);

    main_layout->addWidget(connect_widget);

    port_edit->setFocus();

    connect(host_combo, SIGNAL(editTextChanged(QString)), this, SLOT(enableConnectButton()));
    connect(port_edit, SIGNAL(textChanged(QString)), this, SLOT(enableConnectButton()));
    connect(connect_button, SIGNAL(clicked()), this, SLOT(connectToHost()));
}

void GameDialog::constructGameDialog() {
    QLabel* header = new QLabel(tr("Change game options"));
    header->setAlignment(Qt::AlignCenter);
    QFont f;
    f.setBold(true);
    header->setFont(f);

    start_button = new QPushButton(tr("&Start new game"));
    start_button->setEnabled(false);

    QFormLayout* form_layout = new QFormLayout;
    game_by_x = new QRadioButton(tr("Game by &X"));
    game_by_o = new QRadioButton(tr("Game by &O"));
    form_layout->addWidget(game_by_x);
    form_layout->addWidget(game_by_o);
    form_layout->addWidget(start_button);

    game_label = new QLabel(tr("Game"));
    game_label->setAlignment(Qt::AlignCenter);
    game_label->setFont(f);

    QFrame* frame = new QFrame;
    frame->setFrameStyle(QFrame::StyledPanel);

    QGridLayout* grid_layout = new QGridLayout;
    QFont fg;
    fg.setPointSize(30);
    for (int i = 0; i < 9; i++) {
        game_grid[i] = new QPushButton;
        game_grid[i]->setMinimumWidth(100);
        game_grid[i]->setMinimumHeight(100);
        game_grid[i]->setFocusPolicy(Qt::NoFocus);
        game_grid[i]->setFont(fg);
        grid_layout->addWidget(game_grid[i], i%3, i/3);

        connect(game_grid[i], SIGNAL(clicked()), this, SLOT(goTo()));
    }
    frame->setLayout(grid_layout);
    status_bar = new QStatusBar();
    status_bar->showMessage(tr("Connected to host: %1 port: %2").arg(host_combo->currentText()).arg(port_edit->text()));
    status_bar->setStyleSheet("color: rgb(100, 100, 100)");

    QVBoxLayout* game_layout = new QVBoxLayout;
    game_layout->addWidget(header);
    game_layout->addLayout(form_layout);
    game_layout->addSpacing(20);
    game_layout->addWidget(game_label);
    game_layout->addWidget(frame);
    game_layout->addWidget(status_bar);

    QWidget* game_widget = new QWidget;
    game_widget->setLayout(game_layout);

    main_layout->addWidget(game_widget);

    connect(game_by_x, SIGNAL(clicked()), this, SLOT(enableStartButton()));
    connect(game_by_o, SIGNAL(clicked()), this, SLOT(enableStartButton()));        
    connect(start_button, SIGNAL(clicked()), this, SLOT(startGame()));
}

void GameDialog::sendRequest(QString str) {
    status_bar->showMessage(tr("Waiting for server reply..."));

    // блокируем игровые кнопки
    for (int i = 0; i < 9; i++)
        game_grid[i]->blockSignals(true);

    socket->write(str.toUtf8());
    qDebug() << "write:" << str.trimmed();
}

void GameDialog::goTo() {
    QPushButton* sender_button = qobject_cast<QPushButton*>(sender());
    QString go_to = "goto:%1:\n";
    for (int i = 0; i < 9; i++) // находим кнопку, которую нажали
        if (game_grid[i] == sender_button) last_client_turn = i;
    sendRequest(go_to.arg(last_client_turn));
}

void GameDialog::startGame() {
    last_client_turn = -1;
    client_plays_by = (game_by_x->isChecked())?XO::X_CELL:XO::O_CELL;
    for (int i = 0; i < 9; i++) {
        game_grid[i]->setEnabled(false);
        game_grid[i]->setDown(true);
        game_grid[i]->setText("");
    }
    game_label->setEnabled(false);

    QString start = "start:%1:\n";
    sendRequest(start.arg((game_by_x->isChecked())?"x":"o"));
}

void GameDialog::readReply() {
    // не будем волноваться о больших строках и нескольких пакетах
    // так как по выбранному протоколы длины запросов достаточно малы
    QString str = socket->readAll().trimmed();
    QRegExp rxp("^([01]):([01]):(-1|[01]):(-1|[0-8]):$");
    GameReply reply;
    qDebug() << "read:" << str;
    if (rxp.indexIn(str) != -1) {
        reply.is_error = rxp.cap(1).toInt();
        reply.is_end_of_game = rxp.cap(2).toInt();
        reply.who_win = rxp.cap(3).toInt();
        reply.last_move = rxp.cap(4).toInt();
        status_bar->clearMessage();
        processReply(reply);
    }
    else {
        QMessageBox::warning(this, tr("XO Client Warning"),
                                  tr("The host sent wrong reply. Please check the "
                                     "host name and port settings."));
        status_bar->showMessage(tr("The host sent wrong reply..."));
    }
    // разблокируем игровые кнопки
    for (int i = 0; i < 9; i++)
        game_grid[i]->blockSignals(false);
}

void GameDialog::processReply(GameReply reply) {
    if (reply.is_error)
        status_bar->showMessage(tr("Wrong request..."));
    else {
        if (reply.is_end_of_game) {
            if (last_client_turn == reply.last_move)
                game_grid[reply.last_move]->setText((client_plays_by == XO::X_CELL)?"X":"O");
            else {
                game_grid[last_client_turn]->setText((client_plays_by == XO::X_CELL)?"X":"O");
                game_grid[reply.last_move]->setText((client_plays_by != XO::X_CELL)?"X":"O");
            }
            if (reply.who_win == client_plays_by)
                QMessageBox::information(this, tr("XO Client Information"),
                                         tr("You win! Congratulations!"));
            else if (reply.who_win == XO::NO_WIN)
                QMessageBox::information(this, tr("XO Client Information"),
                                         tr("Draw game. Try again!"));
            else
                QMessageBox::information(this, tr("XO Client Information"),
                                         tr("You lose. Try again!"));
        }
        else {
            if (last_client_turn == -1) {
                if (reply.last_move != -1)
                    game_grid[reply.last_move]->setText((client_plays_by != XO::X_CELL)?"X":"O");
                for (int i = 0; i < 9; i++) {
                    game_grid[i]->setEnabled(true);
                    game_grid[i]->setDown(false);
                }
                game_label->setEnabled(true);
            } else {
                if (last_client_turn == reply.last_move)
                    game_grid[reply.last_move]->setText((client_plays_by == XO::X_CELL)?"X":"O");
                else {
                    game_grid[last_client_turn]->setText((client_plays_by == XO::X_CELL)?"X":"O");
                    game_grid[reply.last_move]->setText((client_plays_by != XO::X_CELL)?"X":"O");
                }
            }
        }
    }
}

void GameDialog::enableConnectButton() {
    connect_button->setEnabled(!host_combo->currentText().isEmpty() && !port_edit->text().isEmpty());
}

void GameDialog::enableStartButton() {
    start_button->setEnabled(game_by_x->isChecked() || game_by_o->isChecked());
}

void GameDialog::connectToHost() {
    socket->connectToHost(host_combo->currentText(), port_edit->text().toInt());
}

void GameDialog::displayError(QAbstractSocket::SocketError error) {
    QString headError = tr("DiodeClient Critical Error");
    QString bodyError;
    switch (error) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        bodyError = tr("The host was not found. Please check the host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        bodyError = tr("The connection was refused by the peer. Make sure the XO server is running, "
                       "and check that the host name and port settings are correct.");
        break;
    default:
        bodyError = tr("The following error occurred: %1.").arg(socket->errorString());
    }
    QMessageBox::critical(this, headError, bodyError);
    changeConnectDialog();
}

void GameDialog::changeConnectDialog() {
    main_layout->setCurrentIndex(0);
    setFixedHeight(116);
}

void GameDialog::changeGameDialog() {
    for (int i = 0; i < 9; i++) {
        game_grid[i]->setEnabled(false);
        game_grid[i]->setDown(true);
        game_grid[i]->setText("");
    }
    game_label->setEnabled(false);
    main_layout->setCurrentIndex(1);
    setFixedHeight(518);
}
