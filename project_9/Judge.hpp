#ifndef JUDGE_HEAD_H
#define JUDGE_HEAD_H
#include "ChessBoard.hpp"
#include "ChessPiece.hpp"
class Judge
{
public:
    bool isWin(int x, int y, string color, ChessPiece *arr[15][15])
    {

        if (verticalWin(x, y, color, arr))
        {

            return true;
        }
        else if (levelWin(x, y, color, arr))
        {
            return true;
        }
        else if (obliqueUpwardWin(x, y, color, arr))
        {
            return true;
        }
        else if (obliqueDownWin(x, y, color, arr))
        {
            return true;
        }

        return false;
    }

    // 判断垂直方向是否存在五棋连色
    bool verticalWin(int x, int y, string color, ChessPiece *arr[15][15])
    {
        int column = (x - 1) / 4;
        int row = (y - 1) / 2;
        int num = 0;
        for (int i = 1; i < 5; i++)
        {

            if (row - i < 0 || !arr[row - i][column] || arr[row - i][column]->getColor() != color)
            {

                break;
            }

            num++;
        }
        if (num >= 4)
        {
            return true;
        }
        for (int i = 1; i < 5; i++)
        {
            if (row + i > 14 || !arr[row + i][column] || arr[row + i][column]->getColor() != color)
            {
                break;
            }
            num++;
        }
        if (num >= 4)
        {
            return true;
        }
        return false;
    }
    // 水平方向是否存在五棋连色

    bool levelWin(int x, int y, string color, ChessPiece *arr[15][15])
    {
        int column = (x - 1) / 4;
        int row = (y - 1) / 2;
        int num = 0;
        for (int i = 1; i < 5; i++)
        {
            if (column - i < 0 || !arr[row][column - i] || arr[row][column - i]->getColor() != color)
            {
                break;
            }
            num++;
        }
        if (num >= 4)
        {
            return true;
        }
        for (int i = 1; i < 5; i++)
        {
            if (column + i > 14 || !arr[row][column + i] || arr[row][column + i]->getColor() != color)
            {
                break;
            }
            num++;
        }
        if (num >= 4)
        {
            return true;
        }
        return false;
    }
    // 斜上方向是否存在五棋连色

    bool obliqueUpwardWin(int x, int y, string color, ChessPiece *arr[15][15])
    {
        int column = (x - 1) / 4;
        int row = (y - 1) / 2;
        int num = 0;
        for (int i = 1; i < 5; i++)
        {
            if (row - i < 0 || column + i > 14 || !arr[row - i][column + i] || arr[row - i][column + i]->getColor() != color)
            {
                break;
            }
            num++;
        }
        if (num >= 4)
        {
            return true;
        }
        for (int i = 1; i < 5; i++)
        {
            if (row + i > 14 || column - i < 0 || !arr[row + i][column - i] || arr[row + i][column - i]->getColor() != color)
            {
                break;
            }
            num++;
        }
        if (num >= 4)
        {
            return true;
        }
        return false;
    }
    // 斜下方向是否存在五棋连色

    bool obliqueDownWin(int x, int y, string color, ChessPiece *arr[15][15])
    {
        int column = (x - 1) / 4;
        int row = (y - 1) / 2;
        int num = 0;
        for (int i = 1; i < 5; i++)
        {
            if (row - i < 0 || column - i < 0 || !arr[row - i][column - i] || arr[row - i][column - i]->getColor() != color)
            {
                break;
            }
            num++;
        }
        if (num >= 4)
        {
            return true;
        }
        for (int i = 1; i < 5; i++)
        {
            if (row + i > 14 || column + i > 0 || !arr[row + i][column + i] || arr[row + i][column + i]->getColor() != color)
            {
                break;
            }
            num++;
        }
        if (num >= 4)
        {
            return true;
        }
        return false;
    }

private:
};
#endif