#ifndef WINLOG_HEAD_H
#define WINLOG_HEAD_H
#include "ChessPlayer.hpp"
class WinLog
{
public:
    void showLog(ChessPlayer *chesspalayer)
    {
        fprintf(stderr, "\033[13;60H\033[31m胜方棋色:%s\033[0m", chesspalayer->getColor().c_str());
        fprintf(stderr, "\033[14;60H\033[31m胜方姓名:%s\033[0m", chesspalayer->getName().c_str());
        fprintf(stderr, "\033[15;60H\033[31m游戏开发者:菜鸟阿凡\033[0m");
        fprintf(stderr, "\033[30;0H\033[0m");
    }

private:
};
#endif