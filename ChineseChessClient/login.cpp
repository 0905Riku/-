#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAcceptDrops(true);

    connect(ui->close,SIGNAL(clicked()),this,SLOT(CloseWindow()));
    connect(ui->min,SIGNAL(clicked()),this,SLOT(MinimizeWindow()));
    connect(ui->OffOnlineBtn,SIGNAL(clicked()),this,SLOT(OffOnlineLoginIn()));

    initInternet();
}

void Login::sendMsg(QString& Msg){
    QByteArray buf;
    buf.append(Msg);

    socket->write(buf);
}

void Login::initInternet(){

    socket = new QTcpSocket();
    socket->connectToHost(serverHost,serverPort);

    dis1 = connect(socket,SIGNAL(connected()),this,SLOT(connectedHandler()));
    dis2 = connect(socket,SIGNAL(disconnected()),this,SLOT(disconnectedHandler()));
    dis3 = connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(errorHandler(QAbstractSocket::SocketError)));
    dis4 = connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,
            SLOT(stateChangedHandler(QAbstractSocket::SocketState)));
}

void Login::connectedHandler(){
    qDebug()<<"connectedHandler"<<endl;

    connect(ui->LoginBtn,SIGNAL(clicked()),this,SLOT(LoginIn()));
    connect(ui->RegisterBtn,SIGNAL(clicked()),this,SLOT(RegisterAccount()));

    dis5 = connect(socket,SIGNAL(readyRead()),this,SLOT(recv()));
    //成功连接
}

void Login::disconnectedHandler(){
    qDebug()<<"disconnectedHandler"<<endl;

    socket->disconnectFromHost();
    //断开连接
}

void Login::errorHandler(QAbstractSocket::SocketError socketerror){
    qDebug()<<"ERROR:"<<socketerror<<endl;

    socket->disconnectFromHost();
    //有错误


    connect(ui->LoginBtn,SIGNAL(clicked()),this,SLOT(internetError()));
    connect(ui->RegisterBtn,SIGNAL(clicked()),this,SLOT(internetError()));
    QMessageBox::warning(this,"网络错误","无法连接到服务器",QMessageBox::Ok);
}

void Login::stateChangedHandler(QAbstractSocket::SocketState socketstate){
    qDebug()<<"STATE:"<<socketstate<<endl;

    //状态改变
}

void Login::recv(void){
   QByteArray buf = socket->readAll();

   QList<QByteArray> splbuf = buf.split(' ');
   if(splbuf[0] == "Error"){
       QMessageBox::warning(this,"错误",splbuf[1],QMessageBox::Ok);
   } else if(splbuf[0] == "Login" && splbuf[1] == "1"){
       disconnect(dis1);
       disconnect(dis2);
       disconnect(dis3);
       disconnect(dis4);
       disconnect(dis5);

       MainInterface *m = new MainInterface;
       m->initInternet(socket);
       m->nowUser.account = ui->account->text();
       m->show();
       close();
   } else if(splbuf[0] == "Register" && splbuf[1] == "1"){
       QMessageBox::information(this,"恭喜","注册账号成功",QMessageBox::Ok);
   }

   qDebug()<<"来着服务器："<<buf<<endl;
}

void Login::LoginIn(){
    QString buf;
    buf = "Login ";
    buf += ui->account->text();
    buf = buf + " " +ui->password->text();

    sendMsg(buf);
}

void Login::RegisterAccount(){

    r = new Register;
    connect(r,SIGNAL(windowClose()),this,SLOT(windowOpen()));
    r->initInternet(socket);
    r->show();
    this->showMinimized();
}

void Login::OffOnlineLoginIn(){
    MainInterface *m = new MainInterface();
    m->initInternet(nullptr);
    m->show();

    close();
}

void Login::CloseWindow(){
    close();
}

void Login::MinimizeWindow(){
    this->showMinimized();
}

void Login::internetError(){
    socket->connectToHost(serverHost,serverPort);
}

Login::~Login()
{
    delete ui;
}

void Login::windowOpen(){
    this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
}

void Login::mouseMoveEvent(QMouseEvent *event){
    if (event->buttons() == Qt::LeftButton){
           move(event->globalPos() - p);
    }
}

void Login::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton){
            //求坐标差值
            //当前点坐标-左上角坐标
            p = event->globalPos() - frameGeometry().topLeft();
        }
}

