#ifndef KEYBOARD_HEAD_H
#define KEYBOARD_HEAD_H
#include <iostream>
#include <termios.h>
#include "ChessBoard.hpp"
#include "ChessPlayer.hpp"

using namespace std;
#include "Cursor.hpp"
class KeyBoard
{
public:
    void control(string color)
    {
        char ch;
        bool ok = false; // 除非落子成功 否则死循环移动
        while (!ok)
        {
            ch = getchar();
            if ('i' == ch || 'I' == ch) // 上
            {
                cursor.moveUp();
            }
            else if ('j' == ch || 'J' == ch) // 左
            {
                cursor.moveLeft();
            }
            else if ('k' == ch || 'K' == ch) // 下
            {
                cursor.moveDown();
            }
            else if ('l' == ch || 'L' == ch) // 右
            {
                cursor.moveRight();
            }
            else if (' ' == ch) // 放下棋子
            {
                ChessBoard *chessboard = ChessBoard::getChessBoard();
                ok = chessboard->updateArray(new ChessPiece(color, cursor.getX(), cursor.getY()));
            }
            else if ('q' == ch)
            {
                break;
            }
        }
    }
    KeyBoard()
    {
        // 设置回显问题
        struct termios attr;
        tcgetattr(0, &attr);
        attr.c_lflag = attr.c_lflag & ~(ICANON | ECHO);
        tcsetattr(0, 0, &attr);
    }

    ~KeyBoard()
    {

        struct termios attr;
        tcgetattr(0, &attr);
        attr.c_lflag = attr.c_lflag | (ICANON | ECHO);
        tcsetattr(0, 0, &attr);
    }

private:
    Cursor cursor;
};
#endif