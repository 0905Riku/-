#ifndef RECORDGAME_H
#define RECORDGAME_H

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
class RecordGame;
}

class RecordGame : public QWidget
{
    Q_OBJECT
public:
    static short OFF_ONLINE;
    static short ONLINE;
    static int d;
    static int choosePiece;
public:
    explicit RecordGame(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void initRecord(QByteArray& );
    void initPlayer(const bool& gamemode,Player *,Player *);
    ~RecordGame();
private:
    Ui::RecordGame *ui;
    QList<Step> record;
    short gameMode;
    int nowTurn = 0;
    int nowStep = -1;
    Player *player1;
    Player *player2;
    QList<Step> recordList;
    Piece piece[32];
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



    QString Translate(const ClickPoint& StartPoint,const ClickPoint& EndPoint,const int& start,const int& end);
    ClickPoint toClickPoint(const QPoint& point);
    void drawCross(int d,int x,int y,QPainter &painter);

    void nextS();
    void lastS();
private slots:
    void nextStep();
    void lastStep();
    void GotoStep();
signals:
    void closeSignal();
};

#endif // RECORDGAME_H
