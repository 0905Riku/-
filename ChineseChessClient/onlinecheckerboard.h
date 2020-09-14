#ifndef ONLINECHECKERBOARD_H
#define ONLINECHECKERBOARD_H

#include <QWidget>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QPoint>
#include "Piece.h"
#include <QBrush>
#include <QLabel>
#include <QMouseEvent>
#include <Player.h>
#include <QTimer>
#include <QMessageBox>
#include <QList>
#include <ClickPoint.h>
#include <Step.h>
#include <QUrl>
#include <QSoundEffect>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QTcpSocket>
#include <QDateTime>

namespace Ui {
class onlineCheckerBoard;
}

class onlineCheckerBoard : public QWidget
{
    Q_OBJECT
public:
    static short OFF_ONLINE;
    static short ONLINE;
    static int d;
    static int choosePiece;
public:
    explicit onlineCheckerBoard(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void recv(QByteArray &buf);
    void initInternet(QTcpSocket *socket){
        this->socket = socket;
    }
    void initPlayer(const bool& gamemode,Player *,Player *);
    ~onlineCheckerBoard();
private:
    Ui::onlineCheckerBoard *ui;
    QList<Step> record;
    short gameMode;
    int GameTime;
    int isOver = 0;
    int StepTime;
    int isMyTurn = 1;
    int qiuhe = 3;
    int huiqi = 3;
    QSoundEffect *myMusic;
    Player *player1;
    Player *player2;
    Player *nowplayer;
    QTimer *timer1;
    QTcpSocket *socket;
    QSoundEffect *checkPlayer = nullptr;
    Piece piece[32];
    int nowSong = 0;
    QPixmap pic[15] = {QPixmap(":/Piece/bche.png")
                       ,QPixmap(":/Piece/bshuai.png")
                       ,QPixmap(":/Piece/bshi.png")
                       ,QPixmap(":/Piece/bxiang.png")
                       ,QPixmap(":/Piece/bma.png")
                       ,QPixmap(":/Piece/bche.png")
                       ,QPixmap(":/Piece/bpao.png")
                       ,QPixmap(":/Piece/bbing.png")
                       ,QPixmap(":/Piece/rshuai.png")
                       ,QPixmap(":/Piece/rshi.png")
                       ,QPixmap(":/Piece/rxiang.png")
                       ,QPixmap(":/Piece/rma.png")
                       ,QPixmap(":/Piece/rche.png")
                       ,QPixmap(":/Piece/rpao.png")
                       ,QPixmap(":/Piece/rbing.png")};
private:
    void initPiece();
    void showPiece();
    void changePlayer();
    void gameOver();
    void playCheckMusic();
    QString toTime(int );
    bool MovePiece(const ClickPoint& ,const short , const short&);
    bool movePieceJiang(const ClickPoint& ,const short , const short&);
    bool movePieceShi(const ClickPoint& ,const short , const short&);
    bool movePieceXiang(const ClickPoint& ,const short , const short&);
    bool movePieceMa(const ClickPoint& ,const short , const short&);
    bool movePieceChe(const ClickPoint& ,const short , const short&);
    bool movePiecePao(const ClickPoint& ,const short , const short&);
    bool movePieceBing(const ClickPoint& ,const short , const short&);

    void HQ(int n);
    short isCheck(const short color);
    bool isCheckMate(const short color);
    void showCheck(short res);
    QString Translate(const ClickPoint& StartPoint,const ClickPoint& EndPoint,const int& start,const int& end);
    ClickPoint toClickPoint(const QPoint& point);
    void drawCross(int d,int x,int y,QPainter &painter);

    int min(int x, int y);
    int max(int x, int y);
private slots:
    void TimeOut();
    void RegretChess();
    void AdaimDefeat();
    void Summation();
    void GameChat();
signals:
    void onlineGameClose();
};


#endif // ONLINECHECKERBOARD_H


