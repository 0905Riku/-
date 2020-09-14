#ifndef PIECE_H
#define PIECE_H

#include <QLabel>
#include <QPixmap>

class Piece:public QLabel{
public:
    static short RED;
    static short BLACK;
    static short SHUAI;
    static short SHI;
    static short XIANG;
    static short MA;
    static short CHE;
    static short PAO;
    static short BING;

    static bool ALIVE;
    static bool DIE;
public:
    short color;
    short xpos;
    short ypos;
    short name;
    bool alive;
public:
    void setValue(const short& color,const short& xpos,const short& ypos,const short& name);
    void setAlive(const bool&);
};



#endif // PIECE_H
