#include "onlinecheckerboard.h"
#include "ui_onlinecheckerboard.h"

short onlineCheckerBoard::OFF_ONLINE = 0;
short onlineCheckerBoard::ONLINE = 1;

int onlineCheckerBoard::d = 80;
int onlineCheckerBoard::choosePiece = -1;

onlineCheckerBoard::onlineCheckerBoard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::onlineCheckerBoard)
{
    ui->setupUi(this);

    //绑定按钮
    connect(ui->AdaimDefeat,SIGNAL(clicked()),this,SLOT(AdaimDefeat()));
    connect(ui->RegretChess,SIGNAL(clicked()),this,SLOT(RegretChess()));
    connect(ui->Summation,SIGNAL(clicked()),this,SLOT(Summation()));
    connect(ui->GameChatSend,SIGNAL(clicked()),this,SLOT(GameChat()));


    initPiece();
    showPiece();

}

onlineCheckerBoard::~onlineCheckerBoard()
{
    delete ui;

    delete player1;
    delete player2;
    delete timer1;
}


void onlineCheckerBoard::GameChat(){

    QString res = "GameChat ";
    res += player1->account+" ";
    if(player1->color == Piece::RED){
        res += "r ";
    } else {
        res += "b ";
    }
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("hh:mm:ss");

    res += current_date+" ";
    res += ui->GameChatText->text();

    QByteArray buf;
    buf.append(res);

    res = "";
    res += current_date+"   "+player1->account+":\n";
    res += ui->GameChatText->text();
    ui->GameChatBox->addItem(res);

    ui->GameChatText->setText("");
    socket->write(buf);

}

//播放将军的音频
void onlineCheckerBoard::playCheckMusic(){
    if(checkPlayer != nullptr){
        delete checkPlayer;
        checkPlayer = nullptr;
    }

    checkPlayer =new QSoundEffect(this);
    checkPlayer->setSource(QUrl::fromLocalFile(":/resource/check.mp3"));
    checkPlayer->setVolume(200);
    checkPlayer->play();
}


//悔棋
void onlineCheckerBoard::RegretChess(){

    QString num;
    if(isMyTurn == 0){
        if(record.size() >= 1){
            num = "1";
        } else {
            QMessageBox::warning(this,"提示","步数不足，不能悔棋",QMessageBox::Ok);
            return;
        }
    } else {
        if(record.size() >= 2){
            num = "2";
        } else {
            QMessageBox::warning(this,"提示","步数不足，不能悔棋",QMessageBox::Ok);
            return;
        }
    }
    QString res = "RegretChess ";
    if(player1->color == Piece::RED){
        res += "r ";
    } else {
        res += "b ";
    }
    res += num;
    QByteArray buf;
    buf.append(res);

    QMessageBox::StandardButton result = QMessageBox::warning(this,"悔棋","确定悔棋吗？还有"+QString::number(huiqi)+"次机会",QMessageBox::Yes|QMessageBox::No);
    switch (result) {
    case QMessageBox::Yes:
        socket->write(buf);
        huiqi--;
        if(huiqi == 0){
            ui->RegretChess->setEnabled(false);
        }
        break;
    default:
        break;
    }

}

//认输
void onlineCheckerBoard::AdaimDefeat(){

    QString win;
    QString res = "AdmitDefeat ";
    if(player1->color == Piece::RED){
        res += "r";
        win = "黑方胜利";
    } else {
        res += "b";
        win = "红方胜利";
    }
    QByteArray buf;
    buf.append(res);
    QMessageBox::StandardButton result = QMessageBox::warning(this,"认输","确定认输吗？",QMessageBox::Yes|QMessageBox::No);
    switch (result) {
        case QMessageBox::Yes:
            socket->write(buf);
        break;
    default:
        break;
    }

    QMessageBox::information(this,"对弈结束",win,QMessageBox::Ok);
    gameOver();
}

//求和
void onlineCheckerBoard::Summation(){

    QString res = "Summation ";
    if(player1->color == Piece::RED){
        res += "r";
    } else {
        res += "b";
    }
    QByteArray buf;
    buf.append(res);

    QMessageBox::StandardButton result = QMessageBox::warning(this,"求和","确定求和吗？你还有 "+QString::number(qiuhe)+" 次机会",QMessageBox::Yes|QMessageBox::No);
    qiuhe--;
    if(qiuhe == 0)
        ui->Summation->setEnabled(false);
    switch (result) {
    case QMessageBox::Yes:
        socket->write(buf);
        break;
    case QMessageBox::No:
        qDebug()<<"NO";
        break;
    default:
        break;
    }

}

//转化成棋盘上的点
ClickPoint onlineCheckerBoard::toClickPoint(const QPoint& point){
    int x = point.x();
    int y = point.y();
    x -= d/2;
    y -= d/2;

    ClickPoint res;

    while(x > 0){
        x -= d;
        res.x++;
    }

    while (y > 0) {
        y -= d;
        res.y++;
    }

    return res;
}

//切换玩家
void onlineCheckerBoard::changePlayer(){
    if(isMyTurn == 0){
        player2->steptime = StepTime;
    } else {
        player1->steptime = StepTime;
    }

    //跟新UI
    if((nowplayer->color == Piece::BLACK && isMyTurn == 1) || (nowplayer->color == Piece::RED && isMyTurn == 0)){
        Player *np;
        if(nowplayer->color == Piece::BLACK){
            np = player1;
        } else {
            np = player2;
        }

        ui->P1_StepTime->display(toTime(np->steptime));
        ui->P1_GameTime->display(toTime(np->gametime));


        ui->P1_Flag->setVisible(false);
        ui->P2_Flag->setVisible(true);
    } else if ((nowplayer->color == Piece::RED && isMyTurn == 1) || (nowplayer->color == Piece::BLACK && isMyTurn == 0)){
        Player *np;
        if(nowplayer->color == Piece::BLACK){
            np = player2;
        } else {
            np = player1;
        }
        ui->P2_StepTime->display(toTime(np->steptime));
        ui->P2_GameTime->display(toTime(np->gametime));

        ui->P1_Flag->setVisible(true);
        ui->P2_Flag->setVisible(false);
    }

    if(choosePiece != -1)
        piece[choosePiece].setPixmap(pic[piece[choosePiece].color*7+piece[choosePiece].name]);
    choosePiece = -1;

    if(isMyTurn == 0){
        isMyTurn = 1;
    } else {
        isMyTurn = 0;
    }
}

void onlineCheckerBoard::showCheck(short res){
    if(res == 1){                           //我将军别人
        if(nowplayer->color == Piece::RED){
            ui->P2_check->setVisible(true);
        } else {
            ui->P1_check->setVisible(true);
        }

        playCheckMusic();
    } else if (res == -1){
        if(nowplayer->color == Piece::RED){
            ui->P1_check->setVisible(true);
        } else {
            ui->P2_check->setVisible(true);
        }

        playCheckMusic();
    } else {
        ui->P2_check->setVisible(false);
        ui->P1_check->setVisible(false);
    }
}

void onlineCheckerBoard::mousePressEvent(QMouseEvent *event){
    int i;
    ClickPoint clickPoint = toClickPoint(event->pos());

    if(isMyTurn == 0){
        return;
    }

    qDebug()<<clickPoint.x<<" "<<clickPoint.y<<endl;

    if(clickPoint.x < 1 || clickPoint.x > 9 || clickPoint.y > 10 || clickPoint.y < 1){
        return ;
    }

    //判断按到哪个棋子
    for(i=0;i<32;i++){
        if(piece[i].xpos == clickPoint.x && piece[i].ypos == clickPoint.y && piece[i].alive == Piece::ALIVE){

            if(choosePiece != -1 && choosePiece != i && piece[i].color != nowplayer->color){                          //如果之前选中 并且 这次选中的是一颗棋子

                if( MovePiece(clickPoint, choosePiece, i) == true){
                    piece[i].setAlive(Piece::DIE);

                    int x = piece[choosePiece].xpos;
                    int y = piece[choosePiece].ypos;
                    piece[choosePiece].xpos = clickPoint.x;
                    piece[choosePiece].ypos = clickPoint.y;

                    short res = isCheck(nowplayer->color);

                    piece[choosePiece].xpos = x;
                    piece[choosePiece].ypos = y;
                    if(res == -1){
                        qDebug()<<"1:走这步会被将军"<<endl;

                        piece[i].setAlive(Piece::ALIVE);

                        return;
                    } else {

                        break;
                    }
                } else {
                    return ;
                }
            }

            if(piece[i].color == nowplayer->color){
                if(choosePiece != -1){
                    piece[choosePiece].setPixmap(pic[piece[choosePiece].color*7+piece[choosePiece].name]);
                }
                //显示选中状态
                QImage imageA = pic[piece[i].color*7+piece[i].name].toImage();
                QImage imageB(":/resource/border1.png");

                QPainter imagepainter(&imageA);  //新建画板
                imagepainter.setCompositionMode(QPainter::CompositionMode_SourceOver);   //设置重叠效果
                imagepainter.drawImage(0,0, imageB);
                imagepainter.end();

                piece[i].setPixmap(QPixmap::fromImage(imageA));
                choosePiece = i;
                return;
            }
        }
    }

    if(choosePiece != -1 && choosePiece != i && MovePiece(clickPoint, choosePiece, i) == true){              //如果之前有选中 移动

        int x = piece[choosePiece].xpos;
        int y = piece[choosePiece].ypos;

        Step step(ClickPoint(piece[choosePiece].xpos,piece[choosePiece].ypos),ClickPoint(clickPoint.x,clickPoint.y),choosePiece,i);

        piece[choosePiece].xpos = clickPoint.x;
        piece[choosePiece].ypos = clickPoint.y;

        short res = isCheck(nowplayer->color);
        if(res == 1){
            if(isCheckMate(nowplayer->color) == true){
                qDebug()<<"将死"<<endl;

                QLabel *checkmate = new QLabel(this);
                checkmate->setPixmap(QPixmap(":/resource/checkmate1.png"));
                checkmate->setVisible(true);
                checkmate->setGeometry(220,230,400,400);

                QString checkMsg;
                if(nowplayer->color == Piece::RED){
                    checkMsg = "红方获胜";
                } else {
                    checkMsg = "黑方获胜";
                }
                QMessageBox::warning(this,"绝杀",checkMsg,QMessageBox::Ok);

                QString myStep = "Step "+QString::number(record.size()+1)+" ";
                myStep += QString::number(step.StartPoint.x)+" "+QString::number(step.StartPoint.y)
                        +" "+QString::number(step.EndPoint.x)+" "+QString::number(step.EndPoint.y)+" ";
                myStep += QString::number(step.start)+" "+QString::number(step.end);
                QByteArray buf;
                buf.append(myStep);
                socket->write(buf);
                socket->flush();



                QString res = "Win ";
                if(player1->color == Piece::RED){
                    res += "r";
                } else {
                    res += "b";
                }

                QByteArray send;
                send.append(res);
                socket->write(send);

                gameOver();
                return;

            } else {
                qDebug()<<"还有机会"<<endl;
            }
        } else if(res == -1){
            qDebug()<<"2:走这步会被将军"<<endl;

            piece[choosePiece].xpos = x;
            piece[choosePiece].ypos = y;
            return;
        }

        if(record.size() != 0){

            int n = record.size();

            if(n >= 3){
                if(record[n-2].start == 32 || record[n-2].start == 33){
                    piece[record[n-3].start].setPixmap(pic[piece[record[n-3].start].color*7+piece[record[n-3].start].name]);
                }
            }

            if(record[n-1].start == 32 || record[n-1].start == 33){

            } else {
                piece[record[n-1].start].setPixmap(pic[piece[record[n-1].start].color*7+piece[record[n-1].start].name]);
            }
        }

        record.push_back(step);
        ui->RecordBox->addItem(Translate(record[record.size()-1].StartPoint,record[record.size()-1].EndPoint,record[record.size()-1].start,record[record.size()-1].end));

        QString myStep = "Step "+QString::number(record.size())+" ";
        myStep += QString::number(record[record.size()-1].StartPoint.x)+" "+QString::number(record[record.size()-1].StartPoint.y)
                +" "+QString::number(record[record.size()-1].EndPoint.x)+" "+QString::number(record[record.size()-1].EndPoint.y)+" ";
        myStep += QString::number(record[record.size()-1].start)+" "+QString::number(record[record.size()-1].end);
        QByteArray buf;
        buf.append(myStep);
        socket->write(buf);

        showCheck(res);
        piece[choosePiece].setPixmap(pic[piece[choosePiece].color*7+piece[choosePiece].name]);
        piece[choosePiece].setGeometry((piece[choosePiece].xpos)*d-d/2,(piece[choosePiece].ypos)*d-d/2,80,80);

        changePlayer();                                          //更换用户
        choosePiece = -1;                                        //变成没有选中的状态
    }

}

//接受消息
void onlineCheckerBoard::recv(QByteArray &buf){

    QList<QByteArray> splbuf = buf.split(' ');

    if(splbuf[0] == "Step"){

        qDebug()<<player1->name<<endl;
        if(splbuf[6].toInt() == 32 || splbuf[6].toInt() == 33){
            Step step(ClickPoint(splbuf[2].toInt(),splbuf[3].toInt()),ClickPoint(splbuf[4].toInt(),splbuf[5].toInt()),splbuf[6].toInt(),splbuf[7].toInt());
            record.push_back(step);
            ui->RecordBox->addItem(Translate(record[record.size()-1].StartPoint,record[record.size()-1].EndPoint,record[record.size()-1].start,record[record.size()-1].end));

            changePlayer();
            return;
        }

        if(splbuf[7].toInt() != 32){
            piece[splbuf[7].toInt()].setAlive(Piece::DIE);
        }

        QImage imageA = pic[piece[splbuf[6].toInt()].color*7+piece[splbuf[6].toInt()].name].toImage();
        QImage imageB(":/resource/border1.png");

        QPainter imagepainter(&imageA);  //新建画板
        imagepainter.setCompositionMode(QPainter::CompositionMode_SourceOver);   //设置重叠效果
        imagepainter.drawImage(0,0, imageB);
        imagepainter.end();

        piece[splbuf[6].toInt()].xpos = splbuf[4].toInt();
        piece[splbuf[6].toInt()].ypos = splbuf[5].toInt();
        piece[splbuf[6].toInt()].setPixmap(QPixmap::fromImage(imageA));
        piece[splbuf[6].toInt()].setGeometry((piece[splbuf[6].toInt()].xpos)*d-d/2,(piece[splbuf[6].toInt()].ypos)*d-d/2,80,80);


        int res = isCheck(player1->color);
        showCheck(res);
        if(res == -1){
            if(isCheckMate(player2->color) == true){
                qDebug()<<"将死"<<endl;

                QLabel *checkmate = new QLabel(this);
                checkmate->setPixmap(QPixmap(":/resource/checkmate1.png"));
                checkmate->setVisible(true);
                checkmate->setGeometry(220,230,400,400);

            }
        }

        Step step(ClickPoint(splbuf[2].toInt(),splbuf[3].toInt()),ClickPoint(splbuf[4].toInt(),splbuf[5].toInt()),splbuf[6].toInt(),splbuf[7].toInt());
        record.push_back(step);
        ui->RecordBox->addItem(Translate(record[record.size()-1].StartPoint,record[record.size()-1].EndPoint,record[record.size()-1].start,record[record.size()-1].end));

        changePlayer();
    } else if(splbuf[0] == "Loss"){

        if(splbuf[1] == "r"){
            QMessageBox::information(this,"对弈结束","黑方获胜",QMessageBox::Ok);
        } else {
            QMessageBox::information(this,"对弈结束","红方获胜",QMessageBox::Ok);
        }

        gameOver();
    } else if(splbuf[0] == "QiuHe"){

        if(splbuf.size() > 1){
            if(splbuf[1] == "jujue"){
                QMessageBox::information(this,"和局","对手拒绝和局",QMessageBox::Yes);
                return;
            }

            if(splbuf[1] == "daying"){
                QMessageBox::information(this,"对弈结束","对手答应和局",QMessageBox::Yes);
                gameOver();
                return;
            }

        }

        QString res;
        QByteArray buf;
        QMessageBox::StandardButton result = QMessageBox::warning(this,"求和","对手求和,是否接受?",QMessageBox::Yes|QMessageBox::No);
        QString c;
        if(player1->color == Piece::RED){
            c = "r";
        } else {
            c = "b";
        }
        switch (result) {
        case QMessageBox::Yes:
            res = "QiuHe "+c+" yes";
            buf.append(res);
            socket->write(buf);
            QMessageBox::information(this,"对弈结束","和局",QMessageBox::Yes);
            gameOver();
            break;
        case QMessageBox::No:
            res = "QiuHe "+c+" no";
            buf.append(res);
            socket->write(buf);
            break;
        default:
            break;
        }
    } else if (splbuf[0] == "GameChat"){
        QString res = splbuf[3];
        res += "  "+splbuf[1];
        res += ":\n";

        for(int i=4;i<splbuf.size();i++){
            res += splbuf[i]+" ";
        }

        ui->GameChatBox->addItem(res);
    } else if (splbuf[0] == "RegretChess"){

        QString c;
        if(player1->color == Piece::RED){
            c = "r";
        } else {
            c = "b";
        }
        if(splbuf[1] == "request"){

            QMessageBox::StandardButton result = QMessageBox::information(this,"悔棋","对方请求悔棋",QMessageBox::Yes|QMessageBox::No);
            if(result == QMessageBox::Yes){
                QString res = "RegretChess "+c+" yes";
                QByteArray buf;
                buf.append(res);
                socket->write(buf);

                if(isMyTurn == 1){
                    int n = record.size()-1;
                    if(record[n].start != 32 || record[n].start != 33){
                        HQ(n);
                    }

                    if(choosePiece != -1){
                        piece[choosePiece].setPixmap(pic[piece[choosePiece].color*7+piece[choosePiece].name]);
                        choosePiece = -1;
                    }
                    changePlayer();
                } else {
                    int n = record.size()-1;
                    if(record[n].start != 32 || record[n].start != 33){
                        HQ(n);
                    }
                    n -= 1;
                    if(record[n].start != 32 || record[n].start != 33){
                        HQ(n);
                    }
                }

            } else if (result == QMessageBox::No){
                QString res = "RegretChess "+c+" no";
                QByteArray buf;
                buf.append(res);
                socket->write(buf);
            }
            return;
        }

        if(splbuf[2] == "yes"){

            QMessageBox::information(this,"悔棋","对方同意悔棋",QMessageBox::Ok);

            QString fromc;
            if(splbuf[1] == "b"){
                fromc = "r";
            } else {
                fromc = "b";
            }
            if((isMyTurn == 0 && fromc != c) || (isMyTurn == 1 && fromc == c)){

                if(isMyTurn == 0){
                    changePlayer();
                } else {
                    if(choosePiece != -1){
                        piece[choosePiece].setPixmap(pic[piece[choosePiece].color*7+piece[choosePiece].name]);
                        choosePiece = -1;
                    }
                }
                int n = record.size()-1;
                if(record[n].start != 32 || record[n].start != 33){
                    HQ(n);
                }
                n -= 1;
                if(record[n].start != 32 || record[n].start != 33){
                    HQ(n);
                }
            } else if ((isMyTurn == 1 && fromc != c) || (isMyTurn == 0 && fromc == c)){

                if(isMyTurn == 0){
                    changePlayer();
                } else {
                    if(choosePiece != -1){
                        piece[choosePiece].setPixmap(pic[piece[choosePiece].color*7+piece[choosePiece].name]);
                        choosePiece = -1;
                    }
                }
                int n = record.size()-1;
                if(record[n].start != 32 || record[n].start != 33){
                    HQ(n);
                }
            }
        } else {
            QMessageBox::information(this,"悔棋","对方拒绝悔棋",QMessageBox::Ok);
        }

    } else if(splbuf[0] == "Win"){

        QMessageBox::information(this,"胜利","你的对手跑了,胜利",QMessageBox::Ok);
        gameOver();
    }
    qDebug()<<buf<<endl;
}

void onlineCheckerBoard::HQ(int n){

    if(record[n].end != 32){
        piece[record[n].end].setAlive(true);
    }

    int i = record[n].start;

    piece[i].xpos = record[n].StartPoint.x;
    piece[i].ypos = record[n].StartPoint.y;

    piece[i].setPixmap(pic[piece[i].color*7+piece[i].name]);
    piece[i].setGeometry((piece[i].xpos)*d-d/2,(piece[i].ypos)*d-d/2,80,80);

    ui->RecordBox->takeItem(n);
    record.removeAt(n);
}

void onlineCheckerBoard::gameOver(){

    isOver = 1;
    timer1->stop();

    ui->RegretChess->setEnabled(false);
    ui->AdaimDefeat->setEnabled(false);
    ui->Summation->setEnabled(false);
    isMyTurn = 0;
}

void onlineCheckerBoard::initPiece(){
    //初始化棋子的属性
    for(int i=0;i<32;i++){
        piece[i].setParent(this);
    }

    piece[0].setValue(Piece::BLACK,1,1,Piece::CHE);
    piece[1].setValue(Piece::BLACK,2,1,Piece::MA);
    piece[2].setValue(Piece::BLACK,3,1,Piece::XIANG);
    piece[3].setValue(Piece::BLACK,4,1,Piece::SHI);
    piece[4].setValue(Piece::BLACK,5,1,Piece::SHUAI);
    piece[5].setValue(Piece::BLACK,6,1,Piece::SHI);
    piece[6].setValue(Piece::BLACK,7,1,Piece::XIANG);
    piece[7].setValue(Piece::BLACK,8,1,Piece::MA);
    piece[8].setValue(Piece::BLACK,9,1,Piece::CHE);
    piece[9].setValue(Piece::BLACK,2,3,Piece::PAO);
    piece[10].setValue(Piece::BLACK,8,3,Piece::PAO);
    piece[11].setValue(Piece::BLACK,1,4,Piece::BING);
    piece[12].setValue(Piece::BLACK,3,4,Piece::BING);
    piece[13].setValue(Piece::BLACK,5,4,Piece::BING);
    piece[14].setValue(Piece::BLACK,7,4,Piece::BING);
    piece[15].setValue(Piece::BLACK,9,4,Piece::BING);

    piece[16].setValue(Piece::RED,1,10,Piece::CHE);
    piece[17].setValue(Piece::RED,2,10,Piece::MA);
    piece[18].setValue(Piece::RED,3,10,Piece::XIANG);
    piece[19].setValue(Piece::RED,4,10,Piece::SHI);
    piece[20].setValue(Piece::RED,5,10,Piece::SHUAI);
    piece[21].setValue(Piece::RED,6,10,Piece::SHI);
    piece[22].setValue(Piece::RED,7,10,Piece::XIANG);
    piece[23].setValue(Piece::RED,8,10,Piece::MA);
    piece[24].setValue(Piece::RED,9,10,Piece::CHE);
    piece[25].setValue(Piece::RED,2,8,Piece::PAO);
    piece[26].setValue(Piece::RED,8,8,Piece::PAO);
    piece[27].setValue(Piece::RED,1,7,Piece::BING);
    piece[28].setValue(Piece::RED,3,7,Piece::BING);
    piece[29].setValue(Piece::RED,5,7,Piece::BING);
    piece[30].setValue(Piece::RED,7,7,Piece::BING);
    piece[31].setValue(Piece::RED,9,7,Piece::BING);
}

void onlineCheckerBoard::showPiece(){
    //初始化棋子图片位置
    for(int i=0;i<32;i++){
        piece[i].setPixmap(pic[piece[i].color*7+piece[i].name]);
        piece[i].setGeometry((piece[i].xpos)*d-d/2,(piece[i].ypos)*d-d/2,80,80);
    }
}


void onlineCheckerBoard::drawCross(int d,int x,int y,QPainter &painter){
    //画十字
    if(x != 1){
        painter.drawLine(QPoint(x*d-5,y*d+5),QPoint(x*d-20,y*d+5));
        painter.drawLine(QPoint(x*d-5,y*d+5),QPoint(x*d-5,y*d+20));
        painter.drawLine(QPoint(x*d-5,y*d-5),QPoint(x*d-20,y*d-5));
        painter.drawLine(QPoint(x*d-5,y*d-5),QPoint(x*d-5,y*d-20));
    }

    if(x != 9){
        painter.drawLine(QPoint(x*d+5,y*d+5),QPoint(x*d+20,y*d+5));
        painter.drawLine(QPoint(x*d+5,y*d+5),QPoint(x*d+5,y*d+20));
        painter.drawLine(QPoint(x*d+5,y*d-5),QPoint(x*d+20,y*d-5));
        painter.drawLine(QPoint(x*d+5,y*d-5),QPoint(x*d+5,y*d-20));
    }
}

void onlineCheckerBoard::paintEvent(QPaintEvent *event){
     QPainter painter(this);

     painter.drawPixmap(rect(),QPixmap(":/resource/bk.jpg"),QRect());

     int d=80;
     int border = 8;

     //画10条横线
     for(int i=1;i<=10;i++)//从1开始，就不会使得棋盘处在边界
     {
         painter.drawLine(QPoint(d,i*d),QPoint(9*d,i*d));
     }
     //画9条竖线
     for(int i=1;i<=9;i++)
     {
         if(i==1||i==9)
         {
             painter.drawLine(QPoint(i*d,d),QPoint(i*d,10*d));
         }
         else
         {
             painter.drawLine(QPoint(i*d,d),QPoint(i*d,5*d));
             painter.drawLine(QPoint(i*d,6*d),QPoint(i*d,10*d));
         }
     }
     //九宫格
     painter.drawLine(QPoint(4*d,1*d),QPoint(6*d,3*d));
     painter.drawLine(QPoint(6*d,1*d),QPoint(4*d,3*d));
     painter.drawLine(QPoint(4*d,8*d),QPoint(6*d,10*d));
     painter.drawLine(QPoint(6*d,8*d),QPoint(4*d,10*d));

     QPen pen;
     pen.setWidth(6);
     painter.setPen(pen);

     //周围的一圈
     painter.drawLine(QPoint(d-border,d-border),QPoint(d+border+d*8,d-border));
     painter.drawLine(QPoint(d-border,d-border),QPoint(d-border,d+border+d*9));
     painter.drawLine(QPoint(d-border,d+border+d*9),QPoint(d+border+d*8,d+border+d*9));
     painter.drawLine(QPoint(d+border+d*8,d-border),QPoint(d+border+d*8,d+border+d*9));


     pen.setWidth(2);
     painter.setPen(pen);

     //加粗周围的边
     painter.drawLine(QPoint(d,d),QPoint(9*d,d));
     painter.drawLine(QPoint(d,10*d),QPoint(9*d,10*d));
     painter.drawLine(QPoint(d,5*d),QPoint(9*d,5*d));
     painter.drawLine(QPoint(d,6*d),QPoint(9*d,6*d));
     painter.drawLine(QPoint(d,d),QPoint(d,10*d));
     painter.drawLine(QPoint(9*d,d),QPoint(9*d,10*d));

     //画十字
     pen.setWidth(3);
     painter.setPen(pen);
     for(int i=1;i<=9;i+=2){
         drawCross(d,i,4,painter);
     }
     for(int i=1;i<=9;i+=2){
         drawCross(d,i,7,painter);
     }
     drawCross(d,2,3,painter);
     drawCross(d,8,3,painter);
     drawCross(d,2,8,painter);
     drawCross(d,8,8,painter);

}

//秒转化时间格式
QString onlineCheckerBoard::toTime(int time){
    int min = 0;
    while(time >= 60){
        time -= 60;
        min++;
    }

    QString m = "00";
    QString s = "00";
    if(min != 0){
        if(min < 10)
            m = "0"+ QString::number(min);
        else
            m = QString::number(min);
    }
    if(time != 0){
        if(time < 10)
            s = "0"+ QString::number(time);
        else
            s = QString::number(time);
    }
    return m+":"+s;
}

//判断 将军
short onlineCheckerBoard::isCheck(const short color){

    if(color == Piece::RED){
        for(int i=0;i<16;i++){
            ClickPoint point(piece[20].xpos,piece[20].ypos);

            if(piece[i].alive == Piece::ALIVE && MovePiece(point,i,20) == true){
                qDebug()<<i<<" 被将军"<<endl;
                return -1;
            }
        }

        for(int i=16;i<32;i++){
            ClickPoint point(piece[4].xpos,piece[4].ypos);

            if(piece[i].alive == Piece::ALIVE && MovePiece(point,i,4) == true){
                qDebug()<<i<<" 将军"<<endl;
                return 1;
            }
        }
    } else {

        for(int i=16;i<32;i++){
            ClickPoint point(piece[4].xpos,piece[4].ypos);

            if(piece[i].alive == Piece::ALIVE && MovePiece(point,i,4) == true){
                qDebug()<<i<<" 被将军"<<endl;
                return -1;
            }
        }

        for(int i=0;i<16;i++){
            ClickPoint point(piece[20].xpos,piece[20].ypos);

            if(piece[i].alive == Piece::ALIVE && MovePiece(point,i,20) == true){
                qDebug()<<i<<" 将军"<<endl;
                return 1;
            }
        }
    }

    return 0;
}

//判断 是否被 将死
bool onlineCheckerBoard::isCheckMate(const short color){
    if(color == Piece::RED){

        for(int i=0;i<16;i++){
            if(piece[i].alive == Piece::ALIVE){ //这个棋子还活着

                for(int x=1;x<=9;x++){
                    for(int y=1;y<=10;y++){
                        if(piece[i].xpos != x || piece[i].ypos != y){
                            ClickPoint c(x,y);
                            if(MovePiece(c,i,20) == true){       //判断是不是可以走
                                int k = 0;
                                for(k=0;k<32;k++){
                                      if(piece[k].alive == Piece::ALIVE && piece[k].xpos == x && piece[k].ypos == y){
                                          if(color == piece[k].color){   //是可以吃的子

                                              if(i == 4 && k != 20){
                                                    break;
                                              }


                                              int x1 = piece[i].xpos;
                                              int y1 = piece[i].ypos;

                                              piece[k].setAlive(Piece::DIE);
                                              piece[i].xpos = piece[k].xpos;
                                              piece[i].ypos = piece[k].ypos;

                                              //int res = isCheck(color);
                                              int res = isCheck(Piece::BLACK);

                                              piece[k].setAlive(Piece::ALIVE);
                                              piece[i].xpos = x1;
                                              piece[i].ypos = y1;

                                              if(res == -1){     //还是被 将军
                                                  break;
                                              } else {

                                                  qDebug()<<"落点有棋子:"<<piece[i].xpos<<" "<<piece[i].ypos<<" >> "<<x<<" "<<y<<endl;
                                                  return false;
                                              }
                                              // 如果落点是棋子 并且是对方的棋子
                                          } else {
                                              break;
                                          }
                                      }
                                 }

                                 if(k != 32){       // 是不能吃的棋子
                                     break;
                                 }

                                 int x1 = piece[i].xpos;
                                 int y1 = piece[i].ypos;

                                 piece[i].xpos = x;
                                 piece[i].ypos = y;

                                 int res = isCheck(color);

                                 piece[i].xpos = x1;
                                 piece[i].ypos = y1;

                                 if(res == 1){     //还是将军
                                     break;
                                 } else {

                                     if(res == -1 && i == 4){
                                         break;
                                     }
                                     qDebug()<<"落点没有棋子 "<<piece[i].xpos<<" "<<piece[i].ypos<<" >> "<<x<<" "<<y<<endl;
                                     return false;
                                 }
                                 // 如果落点不是棋子

                            }
                        }
                    }
                }

            }
        }
    } else {

        for(int i=16;i<32;i++){
            if(piece[i].alive == Piece::ALIVE){ //这个棋子还活着

                for(int x=1;x<=9;x++){
                    for(int y=1;y<=10;y++){
                        if(piece[i].xpos != x || piece[i].ypos != y){
                            ClickPoint c(x,y);
                            if(MovePiece(c,i,4) == true){       //判断是不是可以走
                                int k = 0;
                                for(k=0;k<32;k++){
                                      if(piece[k].alive == Piece::ALIVE && piece[k].xpos == x && piece[k].ypos == y){
                                          if(color == piece[k].color){   //是可以吃的子

                                              if(i == 20 && k != 4){
                                                  break;
                                              }

                                              int x1 = piece[i].xpos;
                                              int y1 = piece[i].ypos;

                                              piece[k].setAlive(Piece::DIE);
                                              piece[i].xpos = piece[k].xpos;
                                              piece[i].ypos = piece[k].ypos;

                                              //int res = isCheck(color);
                                              int res = isCheck(Piece::RED);

                                              piece[k].setAlive(Piece::ALIVE);
                                              piece[i].xpos = x1;
                                              piece[i].ypos = y1;

                                              if(res == -1){     //还是将军
                                                  break;
                                              } else {

                                                  qDebug()<<"落点有棋子:"<<piece[i].xpos<<" "<<piece[i].ypos<<" >> "<<x<<" "<<y<<endl;
                                                  return false;
                                              }
                                              // 如果落点是棋子 并且是对方的棋子
                                          } else {
                                              break;
                                          }
                                      }
                                 }

                                 if(k != 32){
                                     break;
                                 }

                                 int x1 = piece[i].xpos;
                                 int y1 = piece[i].ypos;

                                 piece[i].xpos = x;
                                 piece[i].ypos = y;

                                 int res = isCheck(color);

                                 piece[i].xpos = x1;
                                 piece[i].ypos = y1;

                                 if(res == 1){     //还是将军
                                     break;
                                 } else {

                                     if(res == -1 && i == 20){
                                         break;
                                     }

                                     qDebug()<<"落点没有棋子 "<<piece[i].xpos<<" "<<piece[i].ypos<<" >> "<<x<<" "<<y<<endl;
                                     return false;
                                 }
                                 // 如果落点不是棋子

                            }
                        }
                    }
                }

            }
        }
    }

    return true;
}

//初始化玩家信息
void onlineCheckerBoard::initPlayer(const bool &gameMode, Player *p1, Player *p2){

    qDebug()<<"color:"+QString::number(p1->color)+" gametime " + QString::number(p1->gametime) +"steptime:"+QString::number(p1->steptime)<<endl;
    qDebug()<<"color:"+QString::number(p1->color)+" gametime " + QString::number(p2->gametime) +"steptime:"+QString::number(p2->steptime)<<endl;

    this->gameMode = gameMode;
    GameTime = 900;
    StepTime = 180;

    player1 = p1;
    player2 = p2;

    player1->gametime = 0;
    player2->gametime = 0;

    if(p1->color == Piece::RED){
        QMessageBox::information(this,"对弈开始","你是红方,请先手",QMessageBox::Ok);
    } else {
        QMessageBox::information(this,"对弈开始","你是黑方,对方先手",QMessageBox::Ok);
    }

    timer1 = new QTimer(this);

    connect(timer1,SIGNAL(timeout()),this,SLOT(TimeOut()));
    timer1->start(1000);

    ui->P1_check->setVisible(false);
    ui->P2_check->setVisible(false);


    ui->P1_Flag->setVisible(false);

    if(player1->color == Piece::BLACK){
        ui->P1_Name->setText(player1->name);
        ui->P1_rank->setText(player1->toDuanWei());
        ui->P1_GameTime->display(toTime(player1->gametime));
        ui->P1_StepTime->display(toTime(player1->steptime));
        if(player1->sex == Player::MAN){
            ui->P1_Portrait->setPixmap(QPixmap(":/resource/manPortrait.jpg"));
        } else {
            ui->P1_Portrait->setPixmap(QPixmap(":/resource/womenPortrait.jpg"));
        }

        ui->P2_Name->setText(player2->name);
        ui->P2_rank->setText(player2->toDuanWei());
        ui->P2_GameTime->display(toTime(player2->gametime));
        ui->P2_StepTime->display(toTime(player2->steptime));
        if(player2->sex == Player::MAN){
            ui->P2_Portrait->setPixmap(QPixmap(":/resource/manPortrait.jpg"));
        } else {
            ui->P2_Portrait->setPixmap(QPixmap(":/resource/womenPortrait.jpg"));
        }

        isMyTurn = 0;
    } else {
        ui->P2_Name->setText(player1->name);
        ui->P2_rank->setText(player1->toDuanWei());
        ui->P2_GameTime->display(toTime(player1->gametime));
        ui->P2_StepTime->display(toTime(player1->steptime));
        if(player1->sex == Player::MAN){
            ui->P2_Portrait->setPixmap(QPixmap(":/resource/manPortrait.jpg"));
        } else {
            ui->P2_Portrait->setPixmap(QPixmap(":/resource/womenPortrait.jpg"));
        }

        ui->P1_Name->setText(player2->name);
        ui->P1_rank->setText(player2->toDuanWei());
        ui->P1_GameTime->display(toTime(player2->gametime));
        ui->P1_StepTime->display(toTime(player2->steptime));
        if(player2->sex == Player::MAN){
            ui->P1_Portrait->setPixmap(QPixmap(":/resource/manPortrait.jpg"));
        } else {
            ui->P1_Portrait->setPixmap(QPixmap(":/resource/womenPortrait.jpg"));
        }

        isMyTurn = 1;
    }


    nowplayer = player1;
}

//计时器
void onlineCheckerBoard::TimeOut(){

    Player *np;

    if(isMyTurn == 1){
        np = player1;
    } else {
        np = player2;
    }

    np->steptime--;
    np->gametime++;

    if(np->gametime == GameTime){
        //todo 游戏结束 超时间者 负


        QString timeOver = "Loss ";
        if(player1->color == Piece::RED){
            timeOver += "r";
        } else {
            timeOver += "b";
        }

        QByteArray buf;
        buf.append(timeOver);
        socket->write(buf);

    }
    if(np->steptime == 0){

        if(np == player1){
            QString res = "Step ";
            res += QString::number(record.size()+1);
            Step step(ClickPoint(0,0),ClickPoint(0,0),0,0);
            if(player1->color == Piece::RED){
                res += " 0 0 0 0 32 0";
                step = Step(ClickPoint(0,0),ClickPoint(0,0),32,0);
            } else {
                res += " 0 0 0 0 33 0";
                step = Step(ClickPoint(0,0),ClickPoint(0,0),33,0);
            }

            record.push_back(step);
            ui->RecordBox->addItem(Translate(record[record.size()-1].StartPoint,record[record.size()-1].EndPoint,record[record.size()-1].start,record[record.size()-1].end));
            QByteArray send;
            send.append(res);
            socket->write(send);

            changePlayer();
        }
    }
    if(np->color == Piece::BLACK){
        ui->P1_StepTime->display(toTime(np->steptime));
        ui->P1_GameTime->display(toTime(np->gametime));
    } else {
        ui->P2_StepTime->display(toTime(np->steptime));
        ui->P2_GameTime->display(toTime(np->gametime));
    }
}

//判断选中的棋子能不能移动到这个点
bool onlineCheckerBoard::MovePiece(const ClickPoint& clickPoint,const short choosepiece,const short& killPiece){

    if(piece[choosepiece].name == Piece::SHUAI){

        return  movePieceJiang(clickPoint,choosepiece,killPiece);     //移动 将
    } else if (piece[choosepiece].name == Piece::SHI){

        return  movePieceShi(clickPoint,choosepiece,killPiece);
    } else if (piece[choosepiece].name == Piece::XIANG){

        return  movePieceXiang(clickPoint,choosepiece,killPiece);
    } else if (piece[choosepiece].name == Piece::MA){

        return  movePieceMa(clickPoint,choosepiece,killPiece);
    } else if (piece[choosepiece].name == Piece::CHE){

        return  movePieceChe(clickPoint,choosepiece,killPiece);
    } else if (piece[choosepiece].name == Piece::PAO){

        return  movePiecePao(clickPoint,choosepiece,killPiece);
    } else if (piece[choosepiece].name == Piece::BING){

        return  movePieceBing(clickPoint,choosepiece,killPiece);
    }

    return true;
}

int onlineCheckerBoard::min(int x, int y){
    return x<y?x:y;
}

int onlineCheckerBoard::max(int x, int y){
    return x<y?y:x;
}

bool onlineCheckerBoard::movePieceJiang(const ClickPoint& clickPoint,const short choosepiece,const short& killPiece){
    if(clickPoint.x == piece[choosepiece].xpos && killPiece != 32){                                                        //是否对将
        if(piece[killPiece].name == Piece::SHUAI){
            for(int i=0;i<32;i++){
                if(piece[i].xpos == clickPoint.x && piece[i].ypos < max(clickPoint.y,piece[choosepiece].ypos) && piece[i].ypos > min(clickPoint.y,piece[choosepiece].ypos) && piece[i].alive == Piece::ALIVE){
                       return false;
                }
            }
            return true;
        }

    }

    if(abs(clickPoint.x-piece[choosepiece].xpos)+abs(clickPoint.y-piece[choosepiece].ypos) == 1){   //目标点在将一格的地方
        if(clickPoint.x >= 4 && clickPoint.x <= 6){
            if(piece[choosepiece].color == Piece::RED){
                if(clickPoint.y >= 8 && clickPoint.y <= 10)
                    return true;
                else
                    return false;
            } else {
                if(clickPoint.y >= 1 && clickPoint.y <= 3)
                    return true;
                else
                    return false;
            }
        } else {
            return false;
        }
    }
    return false;
}

bool onlineCheckerBoard::movePieceShi(const ClickPoint& clickPoint,const short choosepiece,const short& killPiece){                           //士 可走

    if(abs(clickPoint.x-piece[choosepiece].xpos) == 1 && abs(clickPoint.y-piece[choosepiece].ypos) == 1 ){                              //士 可走
       if(piece[choosepiece].color == Piece::RED){
           if(clickPoint.x >= 4 && clickPoint.x <= 6 && clickPoint.y >= 8 && clickPoint.y <= 10){
               return true;
           }
       } else {
           if(clickPoint.x >= 4 && clickPoint.x <= 6 && clickPoint.y >= 1 && clickPoint.y <= 3){
               return true;
           }
       }
    }

    return false;
}

bool onlineCheckerBoard::movePieceXiang(const ClickPoint& clickPoint,const short choosepiece,const short& killPiece){                             //象 可走

    if(abs(clickPoint.x-piece[choosepiece].xpos) == 2 && abs(clickPoint.y-piece[choosepiece].ypos) == 2){                                  //判断是象 可走的点
       for(int i=0;i<32;i++){
            if(piece[i].xpos == (clickPoint.x+piece[choosepiece].xpos)/2 && piece[i].ypos == (clickPoint.y+piece[choosepiece].ypos)/2 && piece[i].alive == Piece::ALIVE){     //两点 中间没有棋子
                return false;
            }
       }
       if(piece[choosepiece].color == Piece::RED){
           if(clickPoint.y >= 6){
               return true;
           }
       } else {
           if(clickPoint.y <= 5){
               return true;
           }
       }
    }

    return false;
}

bool onlineCheckerBoard::movePieceMa(const ClickPoint& clickPoint,const short choosepiece,const short& killPiece){

        if(abs(clickPoint.x-piece[choosepiece].xpos) == 2 && abs(clickPoint.y-piece[choosepiece].ypos) == 1){                   //判断是不是在竖着的可走范围内
            for(int i=0;i<32;i++){
                if(piece[i].xpos == (clickPoint.x+piece[choosepiece].xpos)/2 && piece[i].ypos == piece[choosepiece].ypos && piece[i].alive == Piece::ALIVE){
                    return false;
                }
            }
            return true;
        }

        if(abs(clickPoint.y-piece[choosepiece].ypos) == 2 && abs(clickPoint.x-piece[choosepiece].xpos) == 1 ){
            for(int i=0;i<32;i++){
                if(piece[i].ypos == (clickPoint.y+piece[choosepiece].ypos)/2 && piece[i].xpos == piece[choosepiece].xpos && piece[i].alive == Piece::ALIVE){
                    return false;
                }
            }
            return true;
        }
        return false;
}

bool onlineCheckerBoard::movePieceChe(const ClickPoint& clickPoint,const short choosepiece,const short& killPiece){

    if(clickPoint.x == piece[choosepiece].xpos){                            // 在同一列
        for(int i=0;i<32;i++){
            if(piece[i].ypos < max(piece[choosepiece].ypos,clickPoint.y) && piece[i].ypos > min(piece[choosepiece].ypos,clickPoint.y) && piece[i].xpos == clickPoint.x && piece[i].alive == Piece::ALIVE){
                                                                            //两点之间没有其他棋子
                return false;
            }
        }
        return true;
    }

    if(clickPoint.y == piece[choosepiece].ypos){                            // 在同一行
        for(int i=0;i<32;i++){
            if(piece[i].xpos < max(piece[choosepiece].xpos,clickPoint.x) && piece[i].xpos > min(piece[choosepiece].xpos,clickPoint.x) && piece[i].ypos == clickPoint.y && piece[i].alive == Piece::ALIVE){
                                                                            //两点之间没有其他棋子
                return false;
            }
        }
        return true;
    }

    return false;
}

bool onlineCheckerBoard::movePiecePao(const ClickPoint& clickPoint,const short choosepiece,const short& killPiece){

    if(clickPoint.x == piece[choosepiece].xpos){                            // 在同一列
        int sum = 0;
        bool flag = false;

        for(int i=0;i<32;i++){
            if(piece[i].color != piece[choosepiece].color && piece[i].xpos == clickPoint.x && piece[i].ypos == clickPoint.y){
                flag = true;
            }
            if(piece[i].ypos < max(piece[choosepiece].ypos,clickPoint.y) && piece[i].ypos > min(piece[choosepiece].ypos,clickPoint.y)
                    && piece[i].xpos == clickPoint.x && piece[i].alive == Piece::ALIVE){
                sum++;
                if(sum > 1){
                    return false;
                }
            }
        }

        if(flag == true && sum == 1){
            return true;
        } else if (flag == false && sum == 0){
            return true;
        } else {
            return false;
        }
    }

    if(clickPoint.y == piece[choosepiece].ypos){                            // 在同一行
        int sum = 0;
        bool flag = false;
        for(int i=0;i<32;i++){
            if(piece[i].color != piece[choosepiece].color && piece[i].xpos == clickPoint.x && piece[i].ypos == clickPoint.y){
                flag = true;
            }
            if(piece[i].xpos < max(piece[choosepiece].xpos,clickPoint.x) && piece[i].xpos > min(piece[choosepiece].xpos,clickPoint.x) && piece[i].ypos == clickPoint.y
                    && piece[i].alive == Piece::ALIVE){
                sum++;
                if(sum > 1){
                    return false;
                }                                                             //两点之间没有其他棋子
            }
        }

        if(flag == true && sum == 1){
            return true;
        } else if (flag == false && sum == 0){
            return true;
        } else {
            return false;
        }
    }

    return false;
}

bool onlineCheckerBoard::movePieceBing(const ClickPoint& clickPoint,const short choosepiece,const short& killPiece){

    if(piece[choosepiece].color == Piece::RED){

        if(piece[choosepiece].ypos >= 6){

            if(clickPoint.x == piece[choosepiece].xpos && clickPoint.y-piece[choosepiece].ypos == -1){
                return true;
            }
        } else {

            if(abs(clickPoint.x-piece[choosepiece].xpos)+abs(clickPoint.y-piece[choosepiece].ypos) == 1 && clickPoint.y-piece[choosepiece].ypos != 1){
                return true;
            }
        }
    } else {

        if(piece[choosepiece].ypos <= 5){

            if(clickPoint.x == piece[choosepiece].xpos && clickPoint.y-piece[choosepiece].ypos == 1){
                return true;
            }
        } else {

            if(abs(clickPoint.x-piece[choosepiece].xpos)+abs(clickPoint.y-piece[choosepiece].ypos) == 1 && clickPoint.y-piece[choosepiece].ypos != -1){
                return true;
            }
        }
    }

    return false;
}

void onlineCheckerBoard::closeEvent(QCloseEvent *event){

    if(isOver == 0){
        QString res = "Loss ";
        if(player1->color == Piece::RED){
            res += "r";
        } else {
            res += "b";
        }
        QByteArray buf;
        buf.append(res);
        socket->write(buf);
    }


    emit onlineGameClose();
}

QString onlineCheckerBoard :: Translate(const ClickPoint& StartPoint,const ClickPoint& EndPoint,const int& start,const int& end){
    QString temp;
    int sx = StartPoint.x;
    int sy = StartPoint.y;

    int ex = EndPoint.x;
    int ey = EndPoint.y;

    if(start == 32){
        temp = "红方： 超时未下";
        return temp;
    }

    if(start == 33){
        temp = "黑方： 超时未下";
        return temp;
    }

    if(piece[start].color == Piece::RED){
        sx = 10 - sx;
        ex = 10 - ex;
        sy = 11 - sy;
        ey = 11 - ey;
        temp = temp + "红方： ";
    }
    else{
        temp = temp + "黑方： ";
    }

    if(piece[start].name == Piece::SHUAI){
        temp = temp + "士";
    }
    if(piece[start].name == Piece::MA){
        temp = temp + "马";
    }
    if(piece[start].name == Piece::PAO){
        temp = temp + "炮";
    }
    if(piece[start].name == Piece::SHI){
        temp = temp + "士";
    }
    if(piece[start].name == Piece::CHE){
        temp = temp + "车";
    }
    if(piece[start].name == Piece::XIANG){
        temp = temp + "象";
    }
    if(piece[start].name == Piece::BING){
        temp = temp + "兵";
    }

    temp.push_back((char)(sx+'0'));


    if(piece[start].name != Piece::MA && piece[start].name != Piece::SHI && piece[start].name != Piece::XIANG){
        if(ey - sy > 0){
            temp = temp + "进";
            temp.push_back((char)(ey - sy+'0'));
        }
        else if(ey - sy < 0){
            temp = temp + "退";
            temp.push_back((char)(sy - ey+'0'));
        }
        else if(ey - sy == 0){
            temp = temp + "平";
            temp.push_back((char)(ex+'0'));
        }
    }
    else{
        if(ey - sy > 0){
            temp = temp + "进";
            temp.push_back((char)(ex+'0'));
        }
        else if(ey - sy < 0){
            temp = temp + "退";
            temp.push_back((char)(ex+'0'));
        }
    }
    return temp;
}
