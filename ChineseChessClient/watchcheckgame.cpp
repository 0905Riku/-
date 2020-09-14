#include "watchcheckgame.h"
#include "ui_watchcheckgame.h"

short WatchCheckGame::OFF_ONLINE = 0;
short WatchCheckGame::ONLINE = 1;

int WatchCheckGame::d = 80;
int WatchCheckGame::choosePiece = -1;

WatchCheckGame::WatchCheckGame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WatchCheckGame)
{
    ui->setupUi(this);

    ui->AdaimDefeat->setEnabled(false);
    ui->RegretChess->setEnabled(false);
    ui->Summation->setEnabled(false);


    initPiece();
    showPiece();

    ui->P1_Flag->setVisible(false);
    ui->P2_Flag->setVisible(true);
}

WatchCheckGame::~WatchCheckGame()
{
    delete ui;

    delete player1;
    delete player2;
}

/*
void WatchCheckGame::GotoStep(){

    QList<QListWidgetItem*> item = ui->RecordBox->selectedItems();
    int row = ui->RecordBox->row(item[0]);

    if(row == nowStep){
        return;
    }

    int n = row-nowStep;

    if(n<0){
        for(int i=0;i>n;i--){
            lastS();
        }
    } else {
        for(int i=0;i<n;i++){
            nextS();
        }
    }

}
*/

//关闭窗口
void WatchCheckGame::closeEvent(QCloseEvent *event){
    emit closeSignal();
}

void WatchCheckGame::recv(QByteArray &buf){

    QList<QByteArray> splbuf = buf.split(' ');

    if(splbuf[0] == "Step"){

        Step step(ClickPoint(splbuf[2].toInt(),splbuf[3].toInt()),ClickPoint(splbuf[4].toInt(),splbuf[5].toInt()),splbuf[6].toInt(),splbuf[7].toInt());
        record.push_back(step);
        ui->RecordBox->addItem(Translate(record[record.size()-1].StartPoint,record[record.size()-1].EndPoint,record[record.size()-1].start,record[record.size()-1].end));

        nextS();
    } else if (splbuf[0] == "RegretChess"){

        if(splbuf[2] != "yes"){
            return;
        }

        int num = splbuf[3].toInt();
        for(int i=0;i<num;i++){
            lastS();
            record.removeAt(record.size()-1);
        }

        if(splbuf[1] == "r"){
            QMessageBox::information(this,"悔棋","黑方悔棋",QMessageBox::Ok);
        } else {
            QMessageBox::information(this,"悔棋","红方悔棋",QMessageBox::Ok);
        }


    } else if (splbuf[0] == "Loss"){

        if(splbuf.size() == 3){
            if(splbuf[2] == "qiuhe"){
                QMessageBox::information(this,"游戏结束","双方和局",QMessageBox::Ok);
                return;
            }
        }

        if(splbuf[1] == "r"){
            QMessageBox::information(this,"游戏结束","黑方获胜",QMessageBox::Ok);
        } else {
            QMessageBox::information(this,"游戏结束","红方获胜",QMessageBox::Ok);
        }
    }

    qDebug()<<"观战室：!!!!"<<buf<<endl;
}

void WatchCheckGame::initRecord(QByteArray& str){

    if(str == ""){
        return;
    } else {

        QList<QByteArray> array = str.split(',');
        if(array.size() == 0){
            return;
        }
        for(int i=0;i<array.size();i++){
            if(array[i] == ""){
                break;
            }
            QList<QByteArray> s = array[i].split('.');

            Step step(ClickPoint(s[0].toInt(),s[1].toInt()),ClickPoint(s[2].toInt(),s[3].toInt()),s[4].toInt(),s[5].toInt());

            record.push_back(step);
            ui->RecordBox->addItem(Translate(record.last().StartPoint,record.last().EndPoint,record.last().start,record.last().end));
        }

        for(int i=0;i<array.size()-1;i++){
            nextS();
        }
    }

}

void  WatchCheckGame::nextS(){
    if((nowStep+1) >= record.count()){
        return;
    }

    if(nowStep != -1){
        piece[record[nowStep].start].setPixmap(pic[piece[record[nowStep].start].color*7+piece[record[nowStep].start].name]);
    }

    nowStep++;

    int n = nowStep;
    if(record[n].start == 32 || record[n].start == 33){
        changePlayer();
        return;
    }

    if(record[n].end != 32){
        piece[record[n].end].setAlive(Piece::DIE);
    }

    QImage imageA = pic[piece[record[n].start].color*7+piece[record[n].start].name].toImage();
    QImage imageB(":/resource/border1.png");

    QPainter imagepainter(&imageA);  //新建画板
    imagepainter.setCompositionMode(QPainter::CompositionMode_SourceOver);   //设置重叠效果
    imagepainter.drawImage(0,0, imageB);
    imagepainter.end();

    piece[record[n].start].setPixmap(QPixmap::fromImage(imageA));
    piece[record[n].start].xpos = record[n].EndPoint.x;
    piece[record[n].start].ypos = record[n].EndPoint.y;
    piece[record[n].start].setGeometry((piece[record[n].start].xpos)*d-d/2,(piece[record[n].start].ypos)*d-d/2,80,80);

    ui->RecordBox->item(n)->setSelected(true);
    changePlayer();
}

void  WatchCheckGame::lastS(){
    if(nowStep < 0){
        return;
    }

    if(nowStep != record.size()-1){
        piece[record[nowStep+1].start].setPixmap(pic[piece[record[nowStep+1].start].color*7+piece[record[nowStep+1].start].name]);
    }

    int n = nowStep;
    if(record[n].start == 32 || record[n].start == 33){
        changePlayer();
        return;
    }

    if(record[n].end != 32){
        piece[record[n].end].setAlive(Piece::ALIVE);
    }

    QImage imageA = pic[piece[record[n].start].color*7+piece[record[n].start].name].toImage();
    QImage imageB(":/resource/border1.png");

    QPainter imagepainter(&imageA);  //新建画板
    imagepainter.setCompositionMode(QPainter::CompositionMode_SourceOver);   //设置重叠效果
    imagepainter.drawImage(0,0, imageB);
    imagepainter.end();

    piece[record[n].start].setPixmap(QPixmap::fromImage(imageA));
    piece[record[n].start].xpos = record[n].StartPoint.x;
    piece[record[n].start].ypos = record[n].StartPoint.y;
    piece[record[n].start].setGeometry((piece[record[n].start].xpos)*d-d/2,(piece[record[n].start].ypos)*d-d/2,80,80);

    if(nowStep == 0){
        ui->RecordBox->item(0)->setSelected(false);
    }else {
        ui->RecordBox->item(n-1)->setSelected(true);
    }

    changePlayer();
    nowStep--;
}

//转化成棋盘上的点
ClickPoint WatchCheckGame::toClickPoint(const QPoint& point){
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
void WatchCheckGame::changePlayer(){

    if(nowTurn == 0){

        ui->P1_Flag->setVisible(true);
        ui->P2_Flag->setVisible(false);
        nowTurn = 1;
    } else {

        ui->P2_Flag->setVisible(true);
        ui->P1_Flag->setVisible(false);
        nowTurn = 0;
    }
}

void WatchCheckGame::initPiece(){
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

void WatchCheckGame::showPiece(){
    //初始化棋子图片位置
    for(int i=0;i<32;i++){
        piece[i].setPixmap(pic[piece[i].color*7+piece[i].name]);
        piece[i].setGeometry((piece[i].xpos)*d-d/2,(piece[i].ypos)*d-d/2,80,80);
    }
}


void WatchCheckGame::drawCross(int d,int x,int y,QPainter &painter){
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

void WatchCheckGame::paintEvent(QPaintEvent *event){
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


//初始化玩家信息
void WatchCheckGame::initPlayer(const bool &gameMode, Player *p1, Player *p2){

    this->gameMode = gameMode;

    player1 = p1;
    player2 = p2;

    player1->gametime = 0;
    player2->gametime = 0;

    ui->P1_check->setVisible(false);
    ui->P2_check->setVisible(false);

    ui->P1_Flag->setVisible(false);
    ui->P1_Name->setText(player1->name);
    ui->P1_rank->setText(player1->rank);
    if(player1->sex == Player::MAN){
        ui->P1_Portrait->setPixmap(QPixmap(":/resource/manPortrait.jpg"));
    } else {
        ui->P1_Portrait->setPixmap(QPixmap(":/resource/womenPortrait.jpg"));
    }

    ui->P2_Name->setText(player2->name);
    ui->P2_rank->setText(player2->rank);
    if(player2->sex == Player::MAN){
        ui->P2_Portrait->setPixmap(QPixmap(":/resource/manPortrait.jpg"));
    } else {
        ui->P2_Portrait->setPixmap(QPixmap(":/resource/womenPortrait.jpg"));
    }

}

QString WatchCheckGame ::Translate(const ClickPoint& StartPoint,const ClickPoint& EndPoint,const int& start,const int& end){
    QString temp;
    int sx = StartPoint.x;
    int sy = StartPoint.y;

    int ex = EndPoint.x;
    int ey = EndPoint.y;

    if(start == 32){
        return "红方： 超时未下";
    }
    if(start == 33){
        return "黑方： 超时未下";
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
