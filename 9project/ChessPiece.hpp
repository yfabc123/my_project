#ifndef CHESSPIECE_HEAD_H
#define CHESSPIECE_HEAD_H
#include <iostream>
using namespace std;
class ChessPiece
{
public:
    ChessPiece(string color, int x, int y) : color(color), x(x), y(y) {}
    int getX()
    {
        return x;
    }
    int getY()
    {
        return y;
    }
    string getColor()
    {

        return color;
    }

private:
    string color;
    int x;
    int y;
};

#endif