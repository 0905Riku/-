#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player{
public:
    static QString MAN;
    static QString WOMEN;
public:
    QString account;
    QString name;
    QString rank;
    QString w;
    QString l;
    QString h;
    QString sum;
    QString sex;
    short color;
    int gametime;
    int steptime;
public:
    Player(){
       gametime = 900;
       steptime = 180;
    }
    Player(const QString& name,const QString& rank, const QString& sex, const short& color,const int gametime = 900,const int steptime = 180)
        :name(name),rank(rank),sex(sex),color(color),gametime(gametime),steptime(steptime){}
    Player(const QString& account,const QString& name,const QString& rank,const QString& w,const QString& l,const QString& h, const QString& sum,const QString& sex, const short& color,const int gametime = 900,const int steptime = 180)
        :account(account),name(name),rank(rank),w(w),l(l),h(h),sum(sum),sex(sex),color(color),gametime(gametime),steptime(steptime){}
    QString toDuanWei(){
        if(rank.toInt() < 0){
            return "业余小白";
        } else if(rank.toInt() < 100){
            return "业余一级";
        } else if (rank.toInt() < 200) {
            return "业余二级";
        } else if (rank.toInt() < 300) {
            return "业余三级";
        } else if (rank.toInt() < 400) {
            return "业余四级";
        } else if (rank.toInt() < 500) {
            return "业余五级";
        } else if (rank.toInt() < 600) {
            return "业余六级";
        } else if (rank.toInt() < 700) {
            return "业余七级";
        }
    }
};

#endif // PLAYER_H
