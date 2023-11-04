#ifndef _BLACKCHESSPALYER_HEAD_H
#define _BLACKCHESSPALYER_HEAD_H
#include "ChessPlayer.hpp"
#include "ChessPiece.hpp"
#include "ChessBoard.hpp"
#include "KeyBoard.hpp"
#include <iostream>
using namespace std;

class BlackChessPlayer : public ChessPlayer
{
public:
    BlackChessPlayer(string name ) : ChessPlayer(name, "黑"){};
    virtual void placeChessPiece(KeyBoard *keyboard)
    {
        keyboard->control("黑");
    }

private:
};
#endif