#ifndef _WHITECHESSPALYER_HEAD_H
#define _WHITECHESSPALYER_HEAD_H
#include "ChessPlayer.hpp"
#include "ChessBoard.hpp"
#include "KeyBoard.hpp"
#include <iostream>
using namespace std;

class WhiteChessPlayer : public ChessPlayer
{
public:
    WhiteChessPlayer(string name) : ChessPlayer(name, "白"){};
    virtual void placeChessPiece(KeyBoard *keyboard)
    {
        keyboard->control("白");
    }

private:
};
#endif