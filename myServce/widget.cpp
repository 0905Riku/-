#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    initDatabase();

    server = new QTcpServer(this);
    connect(server,SIGNAL(newConnection()),this,SLOT(nCnt()));
    server->listen(QHostAddress::AnyIPv4,8080);
    qDebug()<<"等待连接..."<<endl;

}

//初始化 表格
void Widget::initDatabase(){
    database = new QSqlDatabase;
    *database = QSqlDatabase::addDatabase("QSQLITE");
    database->setDatabaseName("MyDataBase.db");
    if (!database->open())
    {
         qDebug() << "Error: Failed to connect database." << database->lastError();
    }
    else
    {
         qDebug() << "Succeed to connect database." ;
    }

    sql_query = new QSqlQuery;
    createTable();
    showTableInfor();

    //existUserAccount("001");
    //AddUserInfor("002","456","riku2","m","0","0","0","0");
    //AddGameInfor("1","001","002"),
    //AddRecordInfor("1","1","1","1","1","2","1","32");
}

//有连接
void Widget::nCnt(void){
    qDebug()<<"有新的连接"<<endl;
    if(server->hasPendingConnections()){
        list.push_back(server->nextPendingConnection());

        connect(list.last(),SIGNAL(readyRead()),this,SLOT(recv()));
        connect(list.last(),SIGNAL(disconnected()),this,SLOT(discnt()));

        ui->connectList->addItem("IP:"+list.last()->peerAddress().toString()+"  Port:"+QString::number(list.last()->peerPort()));
    }
}

//接受到消息
void Widget::recv(void){
    for(int i=0;i<list.size();i++){
        if(list[i]->bytesAvailable()){
            QTcpSocket *socket = list[i];

            qDebug()<<socket->bytesAvailable()<<endl;
            QByteArray buf = socket->readAll();

            QList<QByteArray> splbuf = buf.split(' ');

            if(splbuf[0] == "Login"){
                Login(socket,splbuf);
            } else if (splbuf[0] == "Register"){
                Register(socket,splbuf);
            } else if (splbuf[0] == "OnlineList"){
                sendOnlineList(socket,splbuf);
            } else if (splbuf[0] == "ChatRoom"){
                sendChatRoomMesg(socket,splbuf);
            } else if (splbuf[0] == "Match"){
                Match(socket,splbuf);
            } else if (splbuf[0] == "Step"){
                Step(socket,buf,splbuf);
            } else if (splbuf[0] == "Win"){
                Win(socket,splbuf);
            } else if (splbuf[0] == "AdmitDefeat"){
                AdmitDefeat(socket,splbuf);
            } else if (splbuf[0] == "RegretChess"){
                RegretChess(socket,buf,splbuf);
            } else if (splbuf[0] == "Summation"){
                Summation(socket,splbuf);
            } else if (splbuf[0] == "Loss"){
                Loss(socket,splbuf);
            } else if (splbuf[0] == "QiuHe"){
                QiuHe(socket,splbuf);
            } else if (splbuf[0] == "GameChat"){
                GameChat(socket,buf,splbuf);
            } else if (splbuf[0] == "FuPan"){
                FuPan(socket,splbuf);
            } else if(splbuf[0] == "Record"){
                SendRecord(socket,splbuf);
            } else if(splbuf[0] == "YaoQing"){
                YaoQing(socket,splbuf);
            } else if(splbuf[0] == "Invite"){
                Invite(socket,splbuf);
            } else if (splbuf[0] == "watchGame"){
                watchGame(socket,splbuf);
            }

            ui->msgBox->addItem("IP:"+list[i]->peerAddress().toString()+"  Port:"+QString::number(list[i]->peerPort())+"\n"+QString(buf));

            qDebug()<<buf<<endl;
        }
    }


}

//观战
void Widget::watchGame(QTcpSocket *socket, QList<QByteArray> &buf){

    int n = -1;
    QString res;
    for(int i=0;i<gameList.size();i++){
        if(gameList[i].RedPlayer.account == buf[1] || gameList[i].BlackPlayer.account == buf[1]){
            n = i;
        }
    }
    if(n == -1){

        res = "watchGame no";
        QByteArray buf1;
        buf1.append(res);
        socket->write(buf1);
        return;
    }

    int my = -1;
    for(int i=0;i<onlineList.size();i++){
        if(onlineList[i].address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

            my = i;
            break;
        }
    }

    res = "GuanZhan ";
    res += gameList[n].BlackPlayer.name+" "+gameList[n].BlackPlayer.rank+" "+gameList[n].BlackPlayer.sex+" ";
    res += gameList[n].RedPlayer.name+" "+gameList[n].RedPlayer.rank+" "+gameList[n].RedPlayer.sex;

    QByteArray buf0;
    buf0.append(res);
    socket->write(buf0);
    socket->flush();

    sql_query->exec("select *from record where gameNumber = "+QString::number(gameList[n].number)+";");
    res = "WatchGameRecord ";
    while(sql_query->next()){

        res += sql_query->value(2).toString()+".";
        res += sql_query->value(3).toString()+".";
        res += sql_query->value(4).toString()+".";
        res += sql_query->value(5).toString()+".";
        res += sql_query->value(6).toString()+".";
        res += sql_query->value(7).toString()+",";
    }

    QByteArray buf1;
    buf1.append(res);
    socket->write(buf1);
    gameList[n].watchPlayers.push_back(onlineList[my]);
}

//邀请
void Widget::Invite(QTcpSocket *socket, QList<QByteArray> &buf){

    int n = 0;
    int p = 0;
    for(int i=0;i<onlineList.size();i++){
        if(onlineList[i].account == buf[2]){
            p = i;
            for(int z=0;z<list.size();z++){
                if(onlineList[i].address == list[z]->peerAddress().toString()+":"+QString::number(list[z]->peerPort())){

                    n = z;
                    break;
                }
            }
            break;
        }
    }

    if(buf[1] == "No"){

        QString res = "Invite no";
        QByteArray buf1;
        buf1.append(res);
        list[n]->write(buf1);
    } else {

        //发给邀请的人
        Group a;
        a.BlackPlayer = onlineList[p];

        for(int i=0;i<onlineList.size();i++){

            if(onlineList[i].address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                QString res = "MatchSuccess r "+onlineList[p].name+" "+onlineList[p].rank+onlineList[p].sex+" "+onlineList[p].victory+" "+onlineList[p].defeat+" "+onlineList[p].harmony+" "+onlineList[p].sum;
                QByteArray buf1;
                buf1.append(res);
                socket->write(buf1);

                res = "MatchSuccess b "+onlineList[i].name+" "+onlineList[i].rank+onlineList[i].sex+" "+onlineList[i].victory+" "+onlineList[i].defeat+" "+onlineList[i].harmony+" "+onlineList[i].sum;
                QByteArray buf2;
                buf2.append(res);
                list[n]->write(buf2);

                a.RedPlayer = onlineList[i];
                break;
            }
        }

        sql_query->exec("select count(0) from game");
        sql_query->next();

        a.number = sql_query->value(0).toInt()+1;


        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("MM/dd|hh:mm:ss");
        AddGameInfor(QString::number(a.number),a.BlackPlayer.account,a.RedPlayer.account,current_date);
        gameList.push_back(a);
    }
}

void Widget::SendRecord(QTcpSocket *socket, QList<QByteArray> &buf){

    QString res = "Record ";
    sql_query->exec("select *from game where accountBlack = '"+buf[1]+"' and accountRed = '"+buf[2]+"' and date = '"+buf[3]+"';");
    sql_query->next();
    QString number = sql_query->value(0).toString();
    sql_query->exec("select *from record where gameNumber = "+number+";");
    while(sql_query->next()){

        res += sql_query->value(2).toString()+".";
        res += sql_query->value(3).toString()+".";
        res += sql_query->value(4).toString()+".";
        res += sql_query->value(5).toString()+".";
        res += sql_query->value(6).toString()+".";
        res += sql_query->value(7).toString()+",";
    }

    QByteArray buf2;
    buf2.append(res);
    socket->write(buf2);

}

void Widget::Win(QTcpSocket *socket, QList<QByteArray> &buf){


    int n = 0;
    //找到我在线是哪一个
    for(int i=0;i<onlineList.size();i++){
        if(socket->peerAddress().toString()+":"+QString::number(socket->peerPort()) == onlineList[i].address){
            n = i;

            int rank = onlineList[n].rank.toInt()+10;
            QString r = QString::number(rank);

            int win = onlineList[n].victory.toInt()+1;
            QString w = QString::number(win);

            int sum = onlineList[n].sum.toInt()+1;
            QString s = QString::number(sum);

            updateUser(onlineList[n].account,r,
                       w,onlineList[n].defeat
                       ,onlineList[n].harmony,s);
            break;
        }
    }

    if(buf[1] == "r"){

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].RedPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){
                    if(gameList[i].BlackPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                        int rank = gameList[i].BlackPlayer.rank.toInt()-10;
                        QString r = QString::number(rank);

                        int los = gameList[i].BlackPlayer.defeat.toInt()+1;
                        QString l = QString::number(los);

                        int sum = gameList[i].BlackPlayer.sum.toInt()-1;
                        QString s = QString::number(sum);

                        updateUser(onlineList[n].account,r,
                                   onlineList[n].victory,l
                                   ,onlineList[n].harmony,s);


                        QString res = "Loss b";
                        QByteArray buf;
                        buf.append(res);
                        list[j]->write(buf);
                    }
                }

                for(int j=0;j<gameList[i].watchPlayers.size();j++){
                    for(int m=0;m<list.size();m++){
                        if(gameList[i].watchPlayers[j].address == list[m]->peerAddress().toString()+":"+QString::number(list[m]->peerPort())){

                            QString res = "Loss b";
                            QByteArray buf;
                            buf.append(res);
                            list[m]->write(buf);
                        }
                    }
                }

                gameList.removeAt(i);
                break;
            }
        }
    } else {

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].BlackPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){
                    if(gameList[i].RedPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                        QString res = "Loss r";
                        QByteArray buf;
                        buf.append(res);
                        list[j]->write(buf);

                        int rank = gameList[i].RedPlayer.rank.toInt()-10;
                        QString r = QString::number(rank);

                        int los = gameList[i].RedPlayer.defeat.toInt()+1;
                        QString l = QString::number(los);

                        int sum = gameList[i].RedPlayer.sum.toInt()-1;
                        QString s = QString::number(sum);

                        updateUser(onlineList[n].account,r,
                                   onlineList[n].victory,l
                                   ,onlineList[n].harmony,s);

                    }
                }

                for(int j=0;j<gameList[i].watchPlayers.size();j++){
                    for(int m=0;m<list.size();m++){
                        if(gameList[i].watchPlayers[j].address == list[m]->peerAddress().toString()+":"+QString::number(list[m]->peerPort())){

                            QString res = "Loss r";
                            QByteArray buf;
                            buf.append(res);
                            list[m]->write(buf);
                        }
                    }
                }

                gameList.removeAt(i);
                break;
            } 
        }
    }

}

void Widget::AdmitDefeat(QTcpSocket *socket, QList<QByteArray> &buf){

    int n = 0;
    //找到我在线是哪一个
    for(int i=0;i<onlineList.size();i++){
        if(socket->peerAddress().toString()+":"+QString::number(socket->peerPort()) == onlineList[i].address){
            n = i;
            int rank = onlineList[i].rank.toInt()-10;
            QString r = QString::number(rank);

            int los = onlineList[i].defeat.toInt()+1;
            QString l = QString::number(los);

            int sum = onlineList[i].sum.toInt()+1;
            QString s = QString::number(sum);

            updateUser(onlineList[n].account,r,
                       onlineList[n].victory,l
                       ,onlineList[n].harmony,s);
            break;
        }


    }

    if(buf[1] == "r"){

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].RedPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){
                    if(gameList[i].BlackPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                        int rank = gameList[i].BlackPlayer.rank.toInt()+10;
                        QString r = QString::number(rank);

                        int win = gameList[i].BlackPlayer.victory.toInt()+1;
                        QString w = QString::number(win);

                        int sum = gameList[i].BlackPlayer.sum.toInt()+1;
                        QString s = QString::number(sum);

                        updateUser(gameList[i].BlackPlayer.account,r,
                                   w,gameList[i].BlackPlayer.defeat
                                   ,gameList[i].BlackPlayer.harmony,s);

                        QString res = "Loss r";
                        QByteArray buf;
                        buf.append(res);
                        list[j]->write(buf);
                    }
                }



                for(int j=0;j<gameList[i].watchPlayers.size();j++){
                    for(int m=0;m<list.size();m++){
                        if(gameList[i].watchPlayers[j].address == list[m]->peerAddress().toString()+":"+QString::number(list[m]->peerPort())){

                            QString res = "Loss r";
                            QByteArray buf;
                            buf.append(res);
                            list[m]->write(buf);
                        }
                    }
                }
                gameList.removeAt(i);
                break;
            }
        }
    } else {

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].BlackPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){
                    if(gameList[i].RedPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){


                        int rank = gameList[i].RedPlayer.rank.toInt()+10;
                        QString r = QString::number(rank);

                        int win = gameList[i].RedPlayer.victory.toInt()+1;
                        QString w = QString::number(win);

                        int sum = gameList[i].RedPlayer.sum.toInt()+1;
                        QString s = QString::number(sum);

                        updateUser(gameList[i].RedPlayer.account,r,
                                   w,gameList[i].RedPlayer.defeat
                                   ,gameList[i].RedPlayer.harmony,s);

                        QString res = "Loss b";
                        QByteArray buf;
                        buf.append(res);
                        list[j]->write(buf);
                    }
                }

                for(int j=0;j<gameList[i].watchPlayers.size();j++){
                    for(int m=0;m<list.size();m++){
                        if(gameList[i].watchPlayers[j].address == list[m]->peerAddress().toString()+":"+QString::number(list[m]->peerPort())){

                            QString res = "Loss b";
                            QByteArray buf;
                            buf.append(res);
                            list[m]->write(buf);
                        }
                    }
                }

                gameList.removeAt(i);
            }

        }

    }


}

//发送走的记录
void Widget::Step(QTcpSocket *socket,QByteArray& msg, QList<QByteArray> &buf){

    int i = 0;
    for(i=0;i<gameList.size();i++){

        if(buf[6].toInt() < 16 || buf[6].toInt() == 33){

            if(gameList[i].BlackPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){
                    if(list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort()) == gameList[i].RedPlayer.address){

                        list[j]->write(msg);
                    }
                }


                for(int j=0;j<gameList[i].watchPlayers.size();j++){

                    for(int n=0;n<list.size();n++){
                        if(gameList[i].watchPlayers[j].address == list[n]->peerAddress().toString()+":"+QString::number(list[n]->peerPort())){
                            list[n]->write(msg);
                            list[n]->flush();
                        }
                    }
                }

                break;
            }

        } else if(buf[6].toInt() > 16 && buf[6].toInt() <= 32){

            qDebug()<<gameList[i].RedPlayer.address<<endl;
            qDebug()<<socket->peerAddress().toString()+":"+QString::number(socket->peerPort())<<endl;
            if(gameList[i].RedPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){
                    if(list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort()) == gameList[i].BlackPlayer.address){

                        list[j]->write(msg);
                    }
                }

                for(int j=0;j<gameList[i].watchPlayers.size();j++){

                    for(int n=0;n<list.size();n++){
                        if(gameList[i].watchPlayers[j].address == list[n]->peerAddress().toString()+":"+QString::number(list[n]->peerPort())){
                            list[n]->write(msg);
                            list[n]->flush();
                        }
                    }
                }

                break;
            }
        }
    }


    if(i == gameList.size()){
        qDebug()<<"没有找到"<<endl;
        return;
    }
    AddRecordInfor(QString::number(gameList[i].number),buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);

}

void Widget::RegretChess(QTcpSocket *socket,QByteArray& send, QList<QByteArray> &buf){

    int j=0;
    int n=0;
    if(buf[1] == "r"){

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].RedPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(j=0;j<list.size();j++){
                    if(gameList[i].BlackPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){
                        break;
                    }
                }

                n = i;
                break;
            }
        }
    } else {

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].BlackPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(j=0;j<list.size();j++){
                    if(gameList[i].RedPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                        break;
                    }
                }
                n = i;
                break;

            }
        }
    }


    if(buf[2] == "yes"){

        qDebug()<<socket->peerAddress().toString()+":"+QString::number(socket->peerPort())<<endl;
        qDebug()<<list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())<<endl;


        QStringList sbuf = gameList[n].huiqi.split(' ');

        int num = sbuf[2].toInt();

        qDebug()<<"GameList n:"<<n<<endl;

        sql_query->exec("select count(0) from record where gameNumber = "+QString::number(gameList[n].number)+";");
        sql_query->next();

        int sum = sql_query->value(0).toInt();

        qDebug()<<"Record sum:"<<sum<<endl;

        while(num != 0){
            sql_query->exec("delete from record where step = "+QString::number(sum)+" and gameNumber = "+QString::number(gameList[n].number)+";");
            sum--;
            num--;
        }

        list[j]->write(send);


        send += " "+sbuf[2];
        for(int e=0;e<gameList[n].watchPlayers.size();e++){
            for(int w=0;w<list.size();w++){

                if(list[w]->peerAddress().toString()+":"+QString::number(list[w]->peerPort()) == gameList[n].watchPlayers[e].address){


                    list[w]->write(send);
                    break;
                }
            }
        }

        return;
    } else if(buf[2] == "no"){

        qDebug()<<socket->peerAddress().toString()+":"+QString::number(socket->peerPort())<<endl;
        qDebug()<<list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())<<endl;

        list[j]->write(send);
        return;
    }


    gameList[n].huiqi = send;
    qDebug()<<"HuiQi:"+gameList[n].huiqi<<endl;
    QString res = "RegretChess request ";
    res += buf[2];
    QByteArray bf;
    bf.append(res);
    list[j]->write(bf);

}

void Widget::QiuHe(QTcpSocket *socket, QList<QByteArray> &buf){


    int n = 0;
    //找到我在线是哪一个
    for(int i=0;i<onlineList.size();i++){
        if(socket->peerAddress().toString()+":"+QString::number(socket->peerPort()) == onlineList[i].address){
            n = i;
            if(buf[2] == "yes"){
                int rank = onlineList[i].rank.toInt()-10;
                QString r = QString::number(rank);

                int los = onlineList[i].harmony.toInt()+1;
                QString l = QString::number(los);

                int sum = onlineList[i].sum.toInt()+1;
                QString s = QString::number(sum);

                updateUser(onlineList[n].account,r,
                           onlineList[n].victory,onlineList[n].defeat
                           ,l,s);
            }

            break;
        }


    }

    if(buf[1] == "r"){

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].RedPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){
                    if(gameList[i].BlackPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                        if(buf[2] == "yes"){
                            int rank = gameList[i].BlackPlayer.rank.toInt()+10;
                            QString r = QString::number(rank);

                            int win = gameList[i].BlackPlayer.harmony.toInt()+1;
                            QString w = QString::number(win);

                            int sum = gameList[i].BlackPlayer.sum.toInt()+1;
                            QString s = QString::number(sum);

                            updateUser(gameList[i].BlackPlayer.account,r,
                                       gameList[i].BlackPlayer.victory,gameList[i].BlackPlayer.defeat
                                       ,w,s);

                            QString res = "QiuHe daying";
                            QByteArray buf;
                            buf.append(res);
                            list[j]->write(buf);

                        }

                        if(buf[2] == "no"){
                            QString res = "QiuHe jujue";
                            QByteArray buf;
                            buf.append(res);
                            list[j]->write(buf);
                        }
                    }
                }

                if(buf[2] == "yes"){
                    for(int j=0;j<gameList[i].watchPlayers.size();j++){
                        for(int m=0;m<list.size();m++){
                            if(gameList[i].watchPlayers[j].address == list[m]->peerAddress().toString()+":"+QString::number(list[m]->peerPort())){

                                QString res = "Loss r qiuhe";
                                QByteArray buf;
                                buf.append(res);
                                list[m]->write(buf);
                            }
                        }
                    }

                    gameList.removeAt(i);
                }
                break;
            }
        }
    } else {

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].BlackPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){
                    if(gameList[i].RedPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){


                        if(buf[2] == "yes"){
                            int rank = gameList[i].BlackPlayer.rank.toInt()+10;
                            QString r = QString::number(rank);

                            int win = gameList[i].BlackPlayer.harmony.toInt()+1;
                            QString w = QString::number(win);

                            int sum = gameList[i].BlackPlayer.sum.toInt()+1;
                            QString s = QString::number(sum);

                            updateUser(gameList[i].BlackPlayer.account,r,
                                       gameList[i].BlackPlayer.victory,gameList[i].BlackPlayer.defeat
                                       ,w,s);
                            QString res = "QiuHe daying";
                            QByteArray buf;
                            buf.append(res);
                            list[j]->write(buf);
                        }

                        if(buf[2] == "no"){
                            QString res = "QiuHe jujue";
                            QByteArray buf;
                            buf.append(res);
                            list[j]->write(buf);
                        }
                    }
                }

                if(buf[2] == "yes"){
                    for(int j=0;j<gameList[i].watchPlayers.size();j++){
                        for(int m=0;m<list.size();m++){
                            if(gameList[i].watchPlayers[j].address == list[m]->peerAddress().toString()+":"+QString::number(list[m]->peerPort())){

                                QString res = "Loss b qiuhe";
                                QByteArray buf;
                                buf.append(res);
                                list[m]->write(buf);
                            }
                        }
                    }
                    gameList.removeAt(i);

                }
                break;
            }
        }

    }
}

void Widget::Summation(QTcpSocket *socket, QList<QByteArray> &buf){
    int n = 0;
    //找到我在线是哪一个
    for(int i=0;i<onlineList.size();i++){
        if(socket->peerAddress().toString()+":"+QString::number(socket->peerPort()) == onlineList[i].address){
            n = i;

            break;
        }


    }

    if(buf[1] == "r"){

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].RedPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){
                    if(gameList[i].BlackPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                        QString res = "QiuHe";
                        QByteArray buf;
                        buf.append(res);
                        list[j]->write(buf);
                    }
                }

                break;
            }
        }
    } else {

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].BlackPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){
                    if(gameList[i].RedPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                        QString res = "QiuHe";
                        QByteArray buf;
                        buf.append(res);
                        list[j]->write(buf);
                    }
                }
            }
        }
    }


}

void Widget::GameChat(QTcpSocket *socket,QByteArray& send ,QList<QByteArray> &buf){

    if(buf[2] == "r"){

        for(int i=0;i<gameList.size();i++){
            if(gameList[i].RedPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

                for(int j=0;j<list.size();j++){

                    qDebug()<<gameList[i].BlackPlayer.address<<endl;
                    qDebug()<<list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())<<endl;
                    if(gameList[i].BlackPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                         list[j]->write(send);
                    }


                }
                return;
            }
        }
    } else {

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].BlackPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){
                for(int j=0;j<list.size();j++){
                    qDebug()<<gameList[i].RedPlayer.address<<endl;
                    qDebug()<<list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())<<endl;
                    if(gameList[i].RedPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                            list[j]->write(send);
                        }
                    }

                }
                return;
        }
    }
}

void Widget::Loss(QTcpSocket *socket, QList<QByteArray> &buf){


    if(buf[1] == "r"){

        for(int i=0;i<gameList.size();i++){
            if(gameList[i].RedPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){

               for(int j=0;j<list.size();j++){
                    if(gameList[i].BlackPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                         QString res = "Win b";
                         QByteArray arr;
                         arr.append(res);
                         list[j]->write(arr);
                    }


                }

                for(int j=0;j<gameList[i].watchPlayers.size();j++){
                    for(int z=0;z<list.size();z++){
                         if(gameList[i].watchPlayers[j].address == list[z]->peerAddress().toString()+":"+QString::number(list[z]->peerPort())){

                              QString res = "Win b";
                              QByteArray arr;
                              arr.append(res);
                              list[z]->write(arr);
                         }
                     }
                }

                gameList.removeAt(i);
                return;
            }
        }
    } else {

        for(int i=0;i<gameList.size();i++){

            if(gameList[i].BlackPlayer.address == socket->peerAddress().toString()+":"+QString::number(socket->peerPort())){
                for(int j=0;j<list.size();j++){
                    qDebug()<<gameList[i].RedPlayer.address<<endl;
                    qDebug()<<list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())<<endl;
                    if(gameList[i].RedPlayer.address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                        QString res = "Win r";
                        QByteArray arr;
                        arr.append(res);
                        list[j]->write(arr);
                     }
                  }

                for(int j=0;j<gameList[i].watchPlayers.size();j++){
                    for(int z=0;z<list.size();z++){
                         if(gameList[i].watchPlayers[j].address == list[z]->peerAddress().toString()+":"+QString::number(list[z]->peerPort())){

                              QString res = "Win r";
                              QByteArray arr;
                              arr.append(res);
                              list[z]->write(arr);
                         }
                     }
                }
                gameList.removeAt(i);
                return;
              }
        }
    }
}

void Widget::FuPan(QTcpSocket *socket,QList<QByteArray>&buf){

    QString res = "GameRecord ";

    sql_query->exec("select *from game where accountBlack = '"+buf[1]+"' or accountRed = '"+buf[1]+"'");
    while(sql_query->next()){

        res += sql_query->value(1).toString()+".";
        res += sql_query->value(2).toString()+".";
        res += sql_query->value(3).toString()+",";
    }

    QByteArray buf2;
    buf2.append(res);
    socket->write(buf2);


}

//匹配
void Widget::Match(QTcpSocket *socket, QList<QByteArray> &buf){

    int n = 0;
    //找到我在线是哪一个
    for(int i=0;i<onlineList.size();i++){
        if(socket->peerAddress().toString()+":"+QString::number(socket->peerPort()) == onlineList[i].address){
            n = i;
            break;
        }
    }

    for(int i=0;i<matchList.size();i++){
        if(abs(onlineList[n].rank.toInt() - matchList[i].rank.toInt()) < 50 && onlineList[n].account != matchList[i].account){
            //匹配成功

             qDebug()<<"匹配到"<<endl;
            Player rplayer = matchList[i];
            Player bplayer = onlineList[n];

            onlineList[n].state = 2;

            QString bMsg = "MatchSuccess b ";
            bMsg += rplayer.name+" ";
            bMsg += rplayer.sex+" ";
            bMsg += rplayer.rank+" ";
            bMsg += rplayer.victory+" ";
            bMsg += rplayer.defeat+" ";
            bMsg += rplayer.harmony+" ";
            bMsg += rplayer.sum;


            QByteArray buf;
            buf.append(bMsg);
            socket->write(buf);


            for(int m=0;m<list.size();m++){
                if(list[m]->peerAddress().toString()+":"+QString::number(list[m]->peerPort()) == rplayer.address){

                    QString rMsg = "MatchSuccess r ";
                    rMsg += bplayer.name+" ";
                    rMsg += bplayer.sex+" ";
                    rMsg += bplayer.rank+" ";
                    rMsg += bplayer.victory+" ";
                    rMsg += bplayer.defeat+" ";
                    rMsg += bplayer.harmony+" ";
                    rMsg += bplayer.sum;

                    QByteArray buf2;
                    buf2.append(rMsg);
                    list[m]->write(buf2);
                }
            }

            sql_query->exec("select count(0) from game");
            sql_query->next();

            Group game(bplayer,rplayer);
            game.number = sql_query->value(0).toInt()+1;


            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("MM/dd|hh:mm:ss");
            AddGameInfor(QString::number(game.number),bplayer.account,rplayer.account,current_date);

            gameList.push_back(game);
            matchList.removeAt(i);

            return;
        }
    }

    {

         //没有符合要求的,加入列表
        matchList.push_back(onlineList[n]);
        onlineList[n].state = 2;

        qDebug()<<"加入匹配："+n<<endl;
        QString res = "WaitMatch";
        QByteArray send;
        send.append(res);
        socket->write(send);
     }
}

//发送聊天室信息
void Widget::sendChatRoomMesg(QTcpSocket *socket, QList<QByteArray> &buf){

    for(int i=0;i<onlineList.size();i++){

        for(int j=0;j<list.size();j++){
            if(list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort()) == onlineList[i].address){

                QString res = "ChatRoomMsg ";
                res += buf[2] + " " + buf[3] + " " + buf[1];

                for(int x = 0;x<buf.size();x++){
                    if(x >= 4)
                        res += " "+buf[x];
                }

                QByteArray sendBuf;
                sendBuf.append(res);
                list[j]->write(sendBuf);
            }
        }
    }
}



//发送在线人员列表
void Widget::sendOnlineList(QTcpSocket *socket, QList<QByteArray> &buf){
    QString res = "OnlineList ";
    for(int i=0;i<onlineList.size();i++){
        res += onlineList[i].account + ".";
        res += onlineList[i].name + ".";
        res += onlineList[i].rank + ".";
        res += onlineList[i].sex + ".";
        res += onlineList[i].victory+".";
        res += onlineList[i].defeat+".";
        res += onlineList[i].harmony+".";
        res += onlineList[i].sum + ",";
    }

    QByteArray send;
    send.append(res);
    socket->write(send);
}

//登录函数
void Widget::Login(QTcpSocket *socket, QList<QByteArray> &buf){


    if(onlineList.size() != 0){
        for(int i=0;i<onlineList.size();i++){
            if(onlineList[i].account == buf[1]){
                socket->write(QByteArray("Error 用户已登录"));
                return;
            }
        }
    }

    qDebug()<<buf[1]<<" "<<buf[2]<<endl;

    if(!sql_query->exec("select count(0) from user where account = '"+buf[1]+"';"))
    {
        //没有找到
        qDebug()<<"登陆函数 找账号 错误"<<endl;
    }
    else
    {
        sql_query->next();
        if(sql_query->value(0).toInt() == 0){
            socket->write(QByteArray("Error 账号不存在"));
            return;
        }
    }

    if(!sql_query->exec("select * from user where account = '"+buf[1]+"';"))
    {
        //没有找到
        qDebug()<<"登陆函数 找账号 错误"<<endl;
        return;
    }
    else
    {
        sql_query->next();
        qDebug()<<sql_query->value(1).toString()<<endl;
        if(sql_query->value(1).toString() != buf[2]){
            socket->write(QByteArray("Error 密码错误"));
            return;
        } else {
            socket->write(QByteArray("Login 1"));

            Player player(sql_query->value(0).toString(),sql_query->value(1).toString(),sql_query->value(2).toString(),
                          sql_query->value(3).toString(),sql_query->value(4).toString(),sql_query->value(5).toString(),
                          sql_query->value(6).toString(),sql_query->value(7).toString(),sql_query->value(8).toString(),socket->peerAddress().toString()+":"+QString::number(socket->peerPort()));

            for(int x=0;x<onlineList.size();x++){
                for(int j=0;j<list.size();j++){
                    if(list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort()) == onlineList[x].address){

                        QString res = "Online ";
                        res += player.account + ".";
                        res += player.name + ".";
                        res += player.rank + ".";
                        res += player.sex + ".";
                        res += player.victory+".";
                        res += player.defeat+".";
                        res += player.harmony+".";
                        res += player.sum;

                        QByteArray send;
                        send.append(res);
                        list[j]->write(send);
                    }
                }
            }
            onlineList.push_back(player);

            QString res;
            res = res + "account:"+player.account +" ";
            res = res + "password:"+player.password +" ";
            res = res + "name:"+player.name +" ";
            res = res + "sex:"+player.sex +" ";
            res = res + "rank:"+player.rank +" ";
            res = res + "v:"+player.victory +" ";
            res = res + "l:"+player.defeat +" ";
            res = res + "h:"+player.harmony +" ";
            res = res + "s:"+player.sum +" ";
            res = res + "address:" +player.address+" ";
            res = res + "state:" + QString::number(player.state);

            ui->onlineListWidget->addItem(res);
            return;
        }
    }
}

//注册
void Widget::Register(QTcpSocket *socket, QList<QByteArray> &buf){

    if(existUserAccount(buf[1]) == true){
        socket->write("Error 此账户已注册");
        return ;
    }

    if(AddUserInfor(buf[1],buf[2],buf[3],buf[4],"0","0","0","0","0") == true){
        socket->write("Register 1");
    }

}

//断开连接
void Widget::discnt(void){
    for(int i=0;i<list.size();i++){
        if(list[i]->state() != QAbstractSocket::ConnectedState){
            for(int z=0;z<onlineList.size();z++){
                QString add = list[i]->peerAddress().toString();
                add += ":" +QString::number(list[i]->peerPort());


                if(onlineList[z].address == add){
                    qDebug()<<"OffOnline "+add<<endl;

                    for(int z=0;z<matchList.size();z++){
                        if(matchList[z].address == add){
                            matchList.removeAt(z);
                            break;
                        }
                    }

                    onlineList.removeAt(z);
                    ui->onlineListWidget->takeItem(z);

                    for(int j=0;j<onlineList.size();j++){
                        for(int x=0;x<list.size();x++){
                            if(list[x]->peerAddress().toString()+":"+QString::number(list[x]->peerPort()) == onlineList[j].address){

                                for(int m=0;m<gameList.size();m++){
                                    if(gameList[m].RedPlayer.address == onlineList[j].address){
                                        QList<QByteArray> a;
                                        a.push_back("Loss");
                                        a.push_back("r");
                                        Loss(list[x],a);
                                    }
                                    if(gameList[m].BlackPlayer.address == onlineList[j].address){
                                        QList<QByteArray> a;
                                        a.push_back("Loss");
                                        a.push_back("b");
                                        Loss(list[x],a);
                                    }

                                    gameList.removeAt(m);
                                }

                                QString res = "OffOnline "+QString::number(z);
                                QByteArray sendBuf;
                                sendBuf.append(res);
                                list[x]->write(sendBuf);
                            }
                        }
                    }
                }
            }

            list[i]->deleteLater();
            list.removeAt(i);
            delete ui->connectList->takeItem(i);
        }
    }

    qDebug()<<"list size:"<<list.size()<<endl;
}

void Widget::showConnectList(void){

}

Widget::~Widget()
{
    delete ui;
}

//判断 这个用户是否已经存在
bool Widget::existUserAccount(const QString &account){

    if(!sql_query->exec("select count(0) from user where account = '"+account+"';"))
    {
        qDebug() << "seach account is same fail"<< sql_query->lastError();
        return true;
    }
    else
    {
        sql_query->next();
        if(sql_query->value(0).toInt() != 0){
            qDebug() << "this accout is exist";
            return true;
        } else {
            qDebug() << "this accout is exist";
            return false;
        }
    }
}

//添加 用户表格的一行数据
bool Widget::AddUserInfor(const QString& account,const QString& password,const QString& name,const QString& sex,const QString& rank,const QString& victory
                  ,const QString& defeat,const QString& harmony,const QString& sum){
    if(!sql_query->exec("insert into user values('"+account+"','"+password+"','"+name+"','"+sex+"',"+rank+","+victory+","+defeat+","+harmony+","+sum+");"))
    {
        qDebug() << "insert into Game fail"<< sql_query->lastError();
        return false;
    }
    else
    {
        qDebug() << "insert into Game success"<< sql_query->lastError();

        QString res;

        res = res + "account:"+account +" ";
        res = res + "password:"+password +" ";
        res = res + "name:"+name +" ";
        res = res + "sex:"+sex +" ";
        res = res + "rank:"+rank +" ";
        res = res + "v:"+victory +" ";
        res = res + "l:"+defeat +" ";
        res = res + "h:"+harmony +" ";
        res = res + "s:"+sum +" ";

        ui->userTable->addItem(res);
        return true;
    }



}

//添加 游戏表格的一行数据
bool Widget::AddGameInfor(const QString& GameNumber,const QString& BlackAccount,const QString& RedAccount,const QString& date){
    if(!sql_query->exec("insert into game values("+GameNumber+",'"+BlackAccount+"','"+RedAccount+"','"+date+"');"))
    {
        qDebug() << "insert into Game fail"<< sql_query->lastError();
        return false;
    }
    else
    {
        qDebug() << "insert into Game success"<< sql_query->lastError();

        QString res;
        res = res + "gameNumber:"+GameNumber +" ";
        res = res + "BlackAccount:"+BlackAccount +" ";
        res = res + "RedAccount:"+RedAccount +" ";
        res = res + "date:"+date +" ";

        ui->gameTable->addItem(res);
        return true;
    }
}

//添加 记录表格的一行数据
bool Widget::AddRecordInfor(const QString& GameNumber,const QString& step,const QString& StartX,
                    const QString& StartY,const QString& EndX,const QString& EndY,const QString& StartNum
                    ,const QString& EndNum){
    if(!sql_query->exec("insert into record values("+GameNumber+","+step+","+StartX+","+StartY+","+EndX+","+EndY+","+StartNum
                        +","+EndNum+");"))
    {
        qDebug() << "insert into record fail"<< sql_query->lastError();
        return false;
    }
    else
    {
        qDebug() << "insert into record success"<< sql_query->lastError();

        QString res;
        res = res + "gameNumber:"+GameNumber +" ";
        res = res + "step:"+step +" ";
        res = res + "StartPoint:("+StartX+","+StartY+") ";
        res = res + "EndPoint:("+EndX+","+EndY+") ";
        res = res + "StartPiece:"+StartNum +" ";
        res = res + "EndPiece:"+EndNum +" ";

        ui->recordTable->addItem(res);
    }

}

//创建 3个表格
void Widget::createTable(){

    //sql_query->exec("drop table game");
    //sql_query->exec("drop table record");

    if(!sql_query->exec("create table user(account char(10) primary key, password char(20) not null,"
                        "name char(20) not null,sex char(1) not null,rank int not null,victory int not null,"
                        "defeat int not null,harmony int not null,sum int not null);"))
    {
        qDebug() << "Error: Fail to create table user."<< sql_query->lastError();
    }
    else
    {
        qDebug() << "user Table created!";
    }

    if(!sql_query->exec("create table game(gameNumber int primary key, accountBlack char(10) not null, accountRed char(10) not null, date char(12) not null);"))
    {
        qDebug() << "Error: Fail to create table. game"<< sql_query->lastError();
    }
    else
    {
        qDebug() << "game Table created!";
    }

    if(!sql_query->exec("create table record(gameNumber int not null, step int not null, startX int not null, startY int not null,"
                        "endX int not null,endY int not null,startNum int not null, endNum int not null);"))
    {
        qDebug() << "Error: Fail to create table. record"<< sql_query->lastError();
    }
    else
    {
        qDebug() << "record Table created!";
    }

}

void Widget::showTableInfor(){
    //把三个表格的信息添加到 ListWidget
    sql_query->exec("select * from user");
        if(!sql_query->exec())
        {
            qDebug()<<sql_query->lastError();
        }
        else
        {
            while(sql_query->next())
            {
                QString res;
                res = res + "account:"+sql_query->value(0).toString() +" ";
                res = res + "password:"+sql_query->value(1).toString() +" ";
                res = res + "name:"+sql_query->value(2).toString() +" ";
                res = res + "sex:"+sql_query->value(3).toString() +" ";
                res = res + "rank:"+sql_query->value(4).toString() +" ";
                res = res + "v:"+sql_query->value(5).toString() +" ";
                res = res + "l:"+sql_query->value(6).toString() +" ";
                res = res + "h:"+sql_query->value(7).toString() +" ";
                res = res + "s:"+sql_query->value(8).toString() +" ";

                ui->userTable->addItem(res);
            }
        }

      sql_query->exec("select * from game");
            if(!sql_query->exec())
            {
                qDebug()<<sql_query->lastError();
            }
            else
            {
                while(sql_query->next())
                {
                    QString res;
                    res = res + "gameNumber:"+sql_query->value(0).toString() +" ";
                    res = res + "BlackAccount:"+sql_query->value(1).toString() +" ";
                    res = res + "RedAccount:"+sql_query->value(2).toString() +" ";
                    res = res + "date:" +sql_query->value(3).toString();
                    ui->gameTable->addItem(res);
                }
            }

      sql_query->exec("select * from record");
             if(!sql_query->exec())
             {
                  qDebug()<<sql_query->lastError();
             }
             else
             {
                  while(sql_query->next())
                  {
                      QString res;
                      res = res + "gameNumber:"+sql_query->value(0).toString() +" ";
                      res = res + "step:"+sql_query->value(1).toString() +" ";
                      res = res + "StartPoint:("+sql_query->value(2).toString()+","+sql_query->value(3).toString()+") ";
                      res = res + "EndPoint:("+sql_query->value(4).toString()+","+sql_query->value(5).toString()+") ";
                      res = res + "StartPiece:"+sql_query->value(6).toString() +" ";
                      res = res + "EndPiece:"+sql_query->value(7).toString() +" ";

                      ui->recordTable->addItem(res);
                   }
             }
}


void  Widget::updateUser(QString &accout,QString &rank,QString &w,QString &l,QString &h,QString &s){

      sql_query->exec("update user set rank = "+rank+",victory = "+w+",defeat = "+l+",harmony = "+h+",sum= "+s+" where account = '"+accout+"';");

      qDebug()<<"update user set rank = "+rank+",victory = "+w+",defeat = "+l+",harmony = "+h+",sum= "+s+" where accout = '"+accout+"';"<<endl;
}

void Widget::YaoQing(QTcpSocket *socket, QList<QByteArray> &buf){

    QString res;
    for(int i=0;i<matchList.size();i++){
        if(matchList[i].account == buf[1]){

            res = "YaoQing busy";
            QByteArray buf1;
            buf1.append(res);
            socket->write(buf1);
            return;
        }
    }

    for(int i=0;gameList.size();i++){

        if(gameList[i].RedPlayer.account == buf[1] || gameList[i].BlackPlayer.account == buf[1]){

            res = "YaoQing busy";
            QByteArray buf1;
            buf1.append(res);
            socket->write(buf1);
            return;
        }
    }

    for(int i=0;i<onlineList.size();i++){

        if(onlineList[i].account == buf[1]){
            for(int j=0;j<list.size();j++){

                if(onlineList[i].address == list[j]->peerAddress().toString()+":"+QString::number(list[j]->peerPort())){

                    res = "YaoQing "+buf[2];
                    QByteArray buf1;
                    buf1.append(res);
                    list[j]->write(buf1);
                }
            }
        }


    }
}
