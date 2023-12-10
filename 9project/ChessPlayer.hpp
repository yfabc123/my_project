#ifndef _CHESSPALYER_HEAD_H
#define _CHESSPALYER_HEAD_H
#include <iostream>
#include "ChessPiece.hpp"
#include "KeyBoard.hpp"
using namespace std;

class ChessPlayer
{
public:
    ChessPlayer(string name, string color) : name(name), color(color){};
    virtual void placeChessPiece(KeyBoard *keyboard) = 0;

    string getColor()
    {
        return color;
    }
    string getName()
    {
        return name;
    }
    virtual ~ChessPlayer() // 以防万一 基类设为虚析构
    {
    }

private:
    string name;
    string color;
};
#endif