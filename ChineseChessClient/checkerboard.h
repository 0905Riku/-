#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

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

namespace Ui {
class CheckerBoard;
}

class CheckerBoard : public QWidget
{
    Q_OBJECT
public:
    static short OFF_ONLINE;
    static short ONLINE;
    static int d;
    static int choosePiece;
public:
    explicit CheckerBoard(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void initPlayer(const bool& gamemode,Player *,Player *);
    ~CheckerBoard();
private:
    Ui::CheckerBoard *ui;
    QList<Step> record;
    short gameMode;
    int GameTime;
    int StepTime;
    QSoundEffect *myMusic;
    Player *player1;
    Player *player2;
    Player *nowplayer;
    QTimer *timer1;
    QSoundEffect *checkPlayer = nullptr;
    Piece piece[32];
    int nowSong = 0;
    bool isOver = false;
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
    void playCheckMusic();
    void gameOver();
    QString toTime(int );
    bool MovePiece(const ClickPoint& ,const short , const short&);
    bool movePieceJiang(const ClickPoint& ,const short , const short&);
    bool movePieceShi(const ClickPoint& ,const short , const short&);
    bool movePieceXiang(const ClickPoint& ,const short , const short&);
    bool movePieceMa(const ClickPoint& ,const short , const short&);
    bool movePieceChe(const ClickPoint& ,const short , const short&);
    bool movePiecePao(const ClickPoint& ,const short , const short&);
    bool movePieceBing(const ClickPoint& ,const short , const short&);

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
signals:
    void closeSignal();
};

#endif // CHECKERBOARD_H
