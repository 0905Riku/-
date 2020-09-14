#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <maininterface.h>
#include <register.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QWidget
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    void sendMsg(QString& Msg);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void initInternet();
    ~Login();

private:
    Ui::Login *ui;
    QTcpSocket *socket;
    QString serverHost = "127.0.0.1";
    int serverPort = 8080;
    Register *r = nullptr;
    QPoint p;
    QMetaObject::Connection dis1;
    QMetaObject::Connection dis2;
    QMetaObject::Connection dis3;
    QMetaObject::Connection dis4;
    QMetaObject::Connection dis5;
private slots:
    void recv(void);
    void connectedHandler();
    void disconnectedHandler();
    void LoginIn();
    void RegisterAccount();
    void OffOnlineLoginIn();
    void CloseWindow();
    void MinimizeWindow();
    void internetError();
    void errorHandler(QAbstractSocket::SocketError socketerror);
    void stateChangedHandler(QAbstractSocket::SocketState socketstate);
    void windowOpen();
};
#endif // LOGIN_H
