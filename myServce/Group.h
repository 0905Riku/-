#ifndef GROUP_H
#define GROUP_H

#include <Player.h>
#include <QList>

class Group{

public:
    int number;
    Player BlackPlayer;
    Player RedPlayer;
    QList<Player> watchPlayers;
    QString huiqi;
public:
    Group(){

    }
    Group(Player& BlackPlayer,Player& RedPlayer){
        this->BlackPlayer = BlackPlayer;
        this->RedPlayer = RedPlayer;
    }
};

#endif // GROUP_H
