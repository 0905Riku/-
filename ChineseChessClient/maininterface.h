#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include <QWidget>
#include <checkerboard.h>
#include <onlinecheckerboard.h>
#include <QTcpSocket>
#include <QDateTime>
#include <QTableWidgetItem>
#include <RecordGame.h>
#include <watchcheckgame.h>

namespace Ui {
class MainInterface;
}

class MainInterface : public QWidget
{
    Q_OBJECT
public:
    Player nowUser;
public:
    explicit MainInterface(QWidget *parent = nullptr);
    void initInternet(QTcpSocket *socket);
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    ~MainInterface();

private:
    Ui::MainInterface *ui;
    CheckerBoard *c = nullptr;
    QList<Player> onlineList;
    onlineCheckerBoard *oc = nullptr;
    RecordGame *g = nullptr;
    WatchCheckGame *w = nullptr;
    QTcpSocket *socket;
    QPoint p;
    QList<QString> recordList;
    int isMatch = 0;
    int isOnlineGame = 0;
    onlineCheckerBoard *o = nullptr;
private slots:
    void openOffOnlineGame();
    void openOnlineGame();
    void offOnlineGameClose();

    void recv(void);
    void connectedHandler();
    void disconnectedHandler();
    void errorHandler(QAbstractSocket::SocketError socketerror);
    void stateChangedHandler(QAbstractSocket::SocketState socketstate);

    void closeGameRecord();
    void CloseWindow();
    void MinWindow();
    void chat();
    void closeEvent(QCloseEvent *event) override;
    void GameClose();
    void fupan();
    void OpenFuPan();
    void fupanClose();
    void onlineGameCloseHandler();
    void yaoQing();
    void watchGame();
    void watchGameCLose();
signals:
    void gameClose();
};

#endif // MAININTERFACE_H
