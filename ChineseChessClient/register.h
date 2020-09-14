#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>
#include <QMouseEvent>

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    QTcpSocket *socket;
    bool isOnlyContainNumberAndCharacter(const QString &);
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void initInternet(QTcpSocket *socket){
        this->socket = socket;
        connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,
                SLOT(errorHandler(QAbstractSocket::SocketError)));
    }
    ~Register();

private:
    Ui::Register *ui;
    QPoint p;
private slots:
    void registerAccount();
    void closeWinow();
    void errorHandler(QAbstractSocket::SocketError socketerror);
signals:
    void windowClose();
};

#endif // REGISTER_H
