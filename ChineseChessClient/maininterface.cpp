#include "maininterface.h"
#include "ui_maininterface.h"

MainInterface::MainInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainInterface)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->Off_Online_Game,SIGNAL(clicked()),this,SLOT(openOffOnlineGame()));
    connect(ui->Online_Game,SIGNAL(clicked()),this,SLOT(openOnlineGame()));

    connect(ui->close,SIGNAL(clicked()),this,SLOT(CloseWindow()));
    connect(ui->min,SIGNAL(clicked()),this,SLOT(MinWindow()));

    connect(ui->sendMsg,SIGNAL(clicked()),this,SLOT(chat()));
    connect(ui->gameRecord,SIGNAL(clicked()),this,SLOT(fupan()));

    connect(ui->invitation,SIGNAL(clicked()),this,SLOT(yaoQing()));
    connect(ui->watchGame,SIGNAL(clicked()),this,SLOT(watchGame()));

    connect(ui->CloseGameRecord,SIGNAL(clicked()),this,SLOT(closeGameRecord()));
    connect(ui->qingding,SIGNAL(clicked()),this,SLOT(OpenFuPan()));
    connect(ui->exit,SIGNAL(clicked()),this,SLOT(CloseWindow()));

    ui->GameRecordBox->setVisible(false);
    ui->CloseGameRecord->setVisible(false);
    ui->qingding->setVisible(false);
}

void MainInterface::watchGame(){

    QList<QListWidgetItem*> item = ui->onlinePlayer->selectedItems();
    if(item.size() == 0){
        return;
    }

    if(w != nullptr){

        QMessageBox::warning(this,"观战","正在观战中",QMessageBox::Ok);
        return;
    }
    int row = ui->onlinePlayer->row(item[0]);

    if(onlineList[row].account == nowUser.account){
        return;
    }

    QString res = "watchGame "+onlineList[row].account;
    QByteArray buf;
    buf.append(res);
    socket->write(buf);

}

void MainInterface::yaoQing(){

    QList<QListWidgetItem*> item = ui->onlinePlayer->selectedItems();
    if(item.size() == 0){
        return;
    }
    int row = ui->onlinePlayer->row(item[0]);

    if(onlineList[row].account == nowUser.account){
        return;
    }

    if(isOnlineGame != 0){
        QMessageBox::warning(this,"提示","正在一场游戏中",QMessageBox::Ok);
    }

    QString res = "YaoQing "+onlineList[row].account+" "+nowUser.account;
    QByteArray buf;
    buf.append(res);
    socket->write(buf);

}

void MainInterface::OpenFuPan(){

    if(recordList.size() == 0){
        return;
    }
    QList<QListWidgetItem*> item = ui->GameRecordBox->selectedItems();

    if(item.size() == 0){
        return;
    }

    int row = ui->GameRecordBox->row(item[0]);
    ui->GameRecordBox->item(row)->text();

    QString res = "Record ";
    res += recordList[row];
    QByteArray buf;
    buf.append(res);
    socket->write(buf);

}

void MainInterface::closeGameRecord(){
    ui->GameRecordBox->setVisible(false);
    ui->CloseGameRecord->setVisible(false);
    ui->qingding->setVisible(false);

    for(int i=0;i<ui->GameRecordBox->count();i++){
        ui->GameRecordBox->takeItem(i);
    }

    for(int i=0;i<recordList.size();i++){
        recordList.pop_back();
    }
}

void MainInterface::fupan(){
    QString res = "FuPan ";
    res += nowUser.account;

    ui->GameRecordBox->setVisible(true);
    ui->CloseGameRecord->setVisible(true);
    ui->qingding->setVisible(true);

    QByteArray buf;
    buf.append(res);
    socket->write(buf);
}

void MainInterface::closeEvent(QCloseEvent *event){

    emit gameClose();
}

void MainInterface::chat(){

    if(ui->sendText->text() == ""){
        return;
    }

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("MM/dd hh:mm:ss");

    QString res = "ChatRoom "+nowUser.account+" "+current_date+" "+ui->sendText->text();
    QByteArray buf;
    buf.append(res);

    ui->sendText->setText("");
    qDebug()<<buf<<endl;
    socket->write(buf);
}

void MainInterface::openOffOnlineGame(){
    if(c == nullptr){
        c = new CheckerBoard();

        connect(c,SIGNAL(closeSignal()),this,SLOT(offOnlineGameClose()));

        Player *p1 = new Player("玩家1","业余一级",Player::MAN,Piece::BLACK);
        Player *p2 = new Player("玩家2","业余一级",Player::WOMEN,Piece::RED);


        c->initPlayer(CheckerBoard::OFF_ONLINE,p1,p2);
        c->setWindowTitle("单机对弈");
        connect(c,SIGNAL(gameClose()),this,SLOT(GameClose()));
        c->show();
   } else {

        QMessageBox::warning(this,"提示","正在游戏中...",QMessageBox::Ok);
   }
}

void MainInterface::GameClose(){
    isOnlineGame = 0;
    c = nullptr;
}

void MainInterface::offOnlineGameClose(){

    c = nullptr;
}

void MainInterface::openOnlineGame(){

    if(isMatch == 1){
        QMessageBox::warning(this,"提示","正在匹配中...",QMessageBox::Ok);
    } else {
        QString sendMatch;
        sendMatch = "Match";
        QByteArray buf;
        buf.append(sendMatch);
        socket->write(buf);
        isMatch = 1;
    }
}

void MainInterface::initInternet(QTcpSocket *socket){
    if(socket == nullptr){
       ui->Online_Game->setEnabled(false);
       ui->gameRecord->setEnabled(false);
       ui->watchGame->setEnabled(false);
       ui->invitation->setEnabled(false);
       ui->sendMsg->setEnabled(false);
    } else {
       this->socket = socket;

        connect(socket,SIGNAL(connected()),this,SLOT(connectedHandler()));
        connect(socket,SIGNAL(disconnected()),this,SLOT(disconnectedHandler()));
        connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,
                SLOT(errorHandler(QAbstractSocket::SocketError)));
        connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,
                SLOT(stateChangedHandler(QAbstractSocket::SocketState)));
        connect(socket,SIGNAL(readyRead()),this,SLOT(recv()));

        socket->write("OnlineList");
    }
}

MainInterface::~MainInterface()
{
    delete ui;
}

void MainInterface::connectedHandler(){
    qDebug()<<"connectedHandler"<<endl;

    //成功连接
}

void MainInterface::disconnectedHandler(){
    qDebug()<<"disconnectedHandler"<<endl;

    socket->disconnectFromHost();
    //断开连接
}

void MainInterface::errorHandler(QAbstractSocket::SocketError socketerror){
    qDebug()<<"ERROR:"<<socketerror<<endl;

    socket->disconnectFromHost();
    //有错误

    QMessageBox::warning(this,"网络错误","无法连接到服务器",QMessageBox::Ok);
}

void MainInterface::stateChangedHandler(QAbstractSocket::SocketState socketstate){
    qDebug()<<"STATE:"<<socketstate<<endl;

    //状态改变
}

void MainInterface::recv(void){
   QByteArray buf = socket->readAll();

   if(o != nullptr){
       o->recv(buf);
   }

   if(w != nullptr){
       w->recv(buf);
   }

   QList<QByteArray> splbuf = buf.split(' ');

   if(splbuf[0] == "OnlineList"){
       QList<QByteArray> array = splbuf[1].split(',');
       for(int i=0;i<array.size();i++){

           if(array[i] == ""){
               break;
           }
           QList<QByteArray> player = array[i].split('.');

           Player p(player[0],player[1],player[2],player[4],player[5],player[6],player[7],player[3],1,0,0);

           QString res;
           res = "账号："+p.account+" 用户名："+p.name+" 段位："+p.toDuanWei()+" 总场数："+p.sum;

           onlineList.push_back(p);

           if(p.account == nowUser.account){
                nowUser.name = p.name;
                nowUser.sex = p.sex;
                nowUser.rank = p.rank;
                nowUser.w = p.w;
                nowUser.l = p.l;
                nowUser.h = p.h;
                nowUser.sum = p.sum;
                nowUser.gametime = 900;
                nowUser.steptime = 180;

                QString userInformation;
                userInformation = nowUser.name+"  "+nowUser.toDuanWei()+"  "+nowUser.w;
                ui->user_infor->addItem("用户名："+nowUser.name);
                ui->user_infor->addItem("段位："+nowUser.toDuanWei());
                ui->user_infor->addItem("积分："+nowUser.rank);
                ui->user_infor->addItem("胜场： "+nowUser.w+"     败场： "+nowUser.l+"     和场: "+nowUser.h);
           }
           ui->onlinePlayer->addItem(res);
       }
   } else if(splbuf[0] == "ChatRoomMsg"){

       QString chatRoomMessage;
       chatRoomMessage += splbuf[1]+" "+splbuf[2]+" "+splbuf[3]+":\n";
       for(int x = 4;x<splbuf.size();x++){
           chatRoomMessage += splbuf[x]+" ";
       }

       ui->MsgBox->addItem(chatRoomMessage);
   } else if(splbuf[0] == "OffOnline"){

       int n = splbuf[1].toInt();
       ui->onlinePlayer->takeItem(n);
   } else if (splbuf[0] == "Online"){

       QList<QByteArray> player = splbuf[1].split('.');

       Player p(player[0],player[1],player[2],player[4],player[5],player[6],player[7],player[3],1,0,0);
       QString res;
       res = "账号："+p.account+" 用户名："+p.name+" 段位："+p.toDuanWei()+" 总场数："+p.sum;
       ui->onlinePlayer->addItem(res);
       onlineList.push_back(p);
   } else if (splbuf[0] == "WaitMatch"){
       QMessageBox::warning(this,"提示","正在匹配中...",QMessageBox::Ok);
   } else if (splbuf[0] == "MatchSuccess"){

       isOnlineGame = 1;
       QString color = splbuf[1];
       short c;
       if(color == "r"){
           c = Piece::RED;
       } else {
           c = Piece::BLACK;
       }

       Player *opponent = new Player(splbuf[2],splbuf[4],splbuf[3],abs(c-1));
       nowUser.color = c;

       this->showMinimized();

       o = new onlineCheckerBoard();
       o->initPlayer(CheckerBoard::ONLINE,&nowUser,opponent);

       connect(o,SIGNAL(onlineGameClose()),this,SLOT(onlineGameCloseHandler()));
       o->initInternet(socket);
       o->show();
       isMatch = 0;

   } else if (splbuf[0] == "GameRecord"){

       if(splbuf[1] == ""){

           ui->GameRecordBox->addItem("没有 比赛记录");
       } else {

           QList<QByteArray> gc = splbuf[1].split(',');

           for(int i=0;i<gc.size();i++){

               if(gc[i] == ""){
                   break;
               }

               QList<QByteArray> r = gc[i].split('.');
               recordList.push_back(r[0]+" "+r[1]+" "+r[2]);
               QString res = "黑方："+r[0]+"  红方："+r[1]+"  日期："+r[2];
               ui->GameRecordBox->addItem(res);
           }

       }
   } else if (splbuf[0] == "Record"){

       if(g == nullptr){
           QList<QListWidgetItem*> item = ui->GameRecordBox->selectedItems();
           int row = ui->GameRecordBox->row(item[0]);
           QList<QString> r =  recordList[row].split(' ');


           g = new RecordGame();
           Player *p1 = new Player(r[0],"",Player::MAN,Piece::BLACK);
           Player *p2 = new Player(r[1],"",Player::WOMEN,Piece::RED);

           g->initRecord(splbuf[1]);
           g->initPlayer(CheckerBoard::OFF_ONLINE,p1,p2);
           connect(g,SIGNAL(closeSignal()),this,SLOT(fupanClose()));
           g->show();
       }


   } else if(splbuf[0] == "YaoQing"){

       if(splbuf[1] == "busy"){
           QMessageBox::information(this,"邀请","对方正在比赛",QMessageBox::Ok);
           return;
       }

       QMessageBox::StandardButton res = QMessageBox::information(this,"邀请",splbuf[1]+"  邀请你对弈",QMessageBox::Yes|QMessageBox::No);
       if(res == QMessageBox::Yes){

           QString buf = "Invite Yes "+splbuf[1];
           QByteArray buf1;
           buf1.append(buf);
           socket->write(buf1);

       } else {

           QString buf = "Invite No "+splbuf[1];
           QByteArray buf1;
           buf1.append(buf);
           socket->write(buf1);
       }
   } else if (splbuf[0] == "Invite"){

       if(splbuf[1] == "no"){
           QMessageBox::information(this,"邀请","对方拒绝了你的邀请",QMessageBox::Ok);
       }


   } else if (splbuf[0] == "watchGame"){

       if(splbuf[1] == "no"){
           QMessageBox::information(this,"观战","对方没有在游戏中...",QMessageBox::Ok);
       }


   } else if(splbuf[0] == "GuanZhan"){

       if(w != nullptr){
           QMessageBox::information(this,"观战","正在观战中..",QMessageBox::Ok);
           return;
       }

       w = new WatchCheckGame();
       Player *p1 = new Player(QString(splbuf[1]),QString(splbuf[2]),QString(splbuf[3]),Piece::BLACK);
       Player *p2 = new Player(QString(splbuf[4]),QString(splbuf[5]),QString(splbuf[6]),Piece::RED);
       connect(w,SIGNAL(closeSignal()),this,SLOT(watchGameCLose()));
       w->initPlayer(CheckerBoard::OFF_ONLINE,p1,p2);
       w->show();

   } else if(splbuf[0] == "WatchGameRecord"){
       w->initRecord(splbuf[1]);
   }

   qDebug()<<"2来着服务器："<<buf<<endl;
}

void MainInterface::watchGameCLose(){

    w = nullptr;
}

void MainInterface::onlineGameCloseHandler(){

    o = nullptr;
    isOnlineGame = 0;
}

void MainInterface::fupanClose(){
    isOnlineGame = 0;
    g = nullptr;
}

void MainInterface::MinWindow(){
    this->showMinimized();
}

void MainInterface::CloseWindow(){
    close();
}

void MainInterface::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton){
        move(event->globalPos() - p);
    }
}

void MainInterface::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        //求坐标差值
        //当前点坐标-左上角坐标
        p = event->globalPos() - frameGeometry().topLeft();
    }
}
