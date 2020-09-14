#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QDebug>
#include <QTcpSocket>
#include <QList>
#include <QAbstractSocket>

#include <qdebug.h>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <Player.h>
#include <Group.h>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    void initDatabase();
    void showConnectList(void);
    void createTable();
    void Login(QTcpSocket *,QList<QByteArray>&);
    void Register(QTcpSocket *,QList<QByteArray>&);
    void sendOnlineList(QTcpSocket *,QList<QByteArray>&);
    void sendChatRoomMesg(QTcpSocket *,QList<QByteArray>&);
    void Match(QTcpSocket *,QList<QByteArray>&);
    void Step(QTcpSocket *,QByteArray &,QList<QByteArray>&);
    void Win(QTcpSocket *,QList<QByteArray>&);
    void AdmitDefeat(QTcpSocket *,QList<QByteArray>&);
    void RegretChess(QTcpSocket *,QByteArray& send,QList<QByteArray>&);
    void Summation(QTcpSocket *,QList<QByteArray>&);
    void QiuHe(QTcpSocket *,QList<QByteArray>&);
    void FuPan(QTcpSocket *,QList<QByteArray>&);
    void YaoQing(QTcpSocket *,QList<QByteArray>&);
    void SendRecord(QTcpSocket *,QList<QByteArray>&);
    void Invite(QTcpSocket *,QList<QByteArray>&);
    void watchGame(QTcpSocket *,QList<QByteArray>&);
    void GameChat(QTcpSocket *,QByteArray& send,QList<QByteArray>&);
    void Loss(QTcpSocket *,QList<QByteArray>&);
    void showTableInfor();
    bool AddUserInfor(const QString& account,const QString& password,const QString& name,const QString& sex,const QString& rank,const QString& victory
                      ,const QString& defeat,const QString& harmony,const QString& sum);
    bool AddGameInfor(const QString& GameNumber,const QString& BlackAccount,const QString& RedAccount,const QString& date);
    bool AddRecordInfor(const QString& GameNumber,const QString& step,const QString& StartX,
                        const QString& StartY,const QString& EndX,const QString& EndY,const QString& StartNum
                        ,const QString& EndNum);
    bool existUserAccount(const QString& account);
    void  updateUser(QString &accout,QString &rank,QString &w,QString &l,QString &h,QString &s);
    ~Widget();
public:
    QTcpServer *server;
    QList<Player> onlineList;
    QList<QTcpSocket*> list;
    QList<Player> matchList;
    QList<Group> gameList;
    QSqlDatabase *database;
    QSqlQuery *sql_query;
private:
    Ui::Widget *ui;
private slots:
    void nCnt(void);
    void recv(void);
    void discnt(void);
};
#endif // WIDGET_H
