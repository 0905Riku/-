#include <Piece.h>

short Piece::RED = 1;
short Piece::BLACK = 0;
short Piece::SHUAI = 1;
short Piece::SHI = 2;
short Piece::XIANG = 3;
short Piece::MA = 4;
short Piece::CHE = 5;
short Piece::PAO = 6;
short Piece::BING = 7;

bool Piece::ALIVE = true;
bool Piece::DIE = false;

void Piece::setValue(const short& color,const short& xpos,const short& ypos,const short& name){
    this->color = color;
    this->xpos = xpos;
    this->ypos = ypos;
    this->name = name;
    this->alive = true;
}

void Piece::setAlive(const bool& state){
    if(state == false){
        this->alive = false;
        this->setVisible(false);
    }else{
        this->alive = true;
        this->setVisible(true);
    }
}
