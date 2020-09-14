#include "register.h"
#include "ui_register.h"

Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    ui->man->setChecked(true);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);
    connect(ui->close,SIGNAL(clicked()),this,SLOT(closeWinow()));
    connect(ui->RegisterBtn,SIGNAL(clicked()),this,SLOT(registerAccount()));
}

void Register::closeWinow(){
    close();
}

void Register::errorHandler(QAbstractSocket::SocketError socketerror){
    close();
}

void Register::registerAccount(){

    if(isOnlyContainNumberAndCharacter(ui->account->text()) == false){
        QMessageBox::warning(this,"错误","账号格式不规范",QMessageBox::Ok);
        return;
    }

    if(isOnlyContainNumberAndCharacter(ui->password1->text()) == false || isOnlyContainNumberAndCharacter(ui->password2->text()) == false){
        QMessageBox::warning(this,"错误","密码格式不规范",QMessageBox::Ok);
        return;
    }

    if(ui->password1->text() != ui->password2->text()){
        QMessageBox::warning(this,"错误","两次密码不一致",QMessageBox::Ok);
        return;
    }

    if(ui->name->text().size() == 0){
        QMessageBox::warning(this,"错误","用户名不能为空",QMessageBox::Ok);
        return;
    }

    if(ui->name->text().size() > 12){
        QMessageBox::warning(this,"错误","用户名过长",QMessageBox::Ok);
        return;
    }

    QString sex;
    if(ui->man->isChecked() == true){
        sex = "m";
    } else {
        sex = "w";
    }

    QString res = "Register ";
    res += ui->account->text()+" ";
    res += ui->password1->text()+" ";
    res += ui->name->text()+" ";
    res += sex;

    QByteArray buf;
    buf.append(res);

    socket->write(buf);

    close();
}

Register::~Register()
{
    delete ui;
}

void Register::closeEvent(QCloseEvent *event){
    emit windowClose();
}

bool Register::isOnlyContainNumberAndCharacter(const QString &str){

    int n = str.size();

    if(n<5 || n>15){
        return false;
    }

    if (str.contains(QRegExp("^[A-Za-z0-9]{0,"+QString::number(n)+"}$"))){
        return true;
    }
    return false;
}


void Register::mouseMoveEvent(QMouseEvent *event){
    if (event->buttons() == Qt::LeftButton){
            move(event->globalPos() - p);
     }
}

void Register::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        //求坐标差值
        //当前点坐标-左上角坐标
        p = event->globalPos() - frameGeometry().topLeft();
    }
}
