#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player{
public:
    QString account;
    QString password;
    QString name;
    QString sex;
    QString rank;
    QString victory;
    QString defeat;
    QString harmony;
    QString sum;
    QString address;
    int state = 1;
public:
    Player(){}
    Player(const QString& account,const QString& password, const QString& name, const QString& sex, const QString& rank,
          const QString& victory,const QString& defeat,const QString& harmony,const QString& sum,const QString& address)
        :account(account),password(password),name(name),sex(sex),rank(rank),victory(victory),defeat(defeat),harmony(harmony),sum(sum),address(address){}
};

#endif // PLAYER_H
