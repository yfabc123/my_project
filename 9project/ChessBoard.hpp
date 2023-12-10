#ifndef CHESSBOARD_HEAD_H_
#define CHESSBOARD_HEAD_H_
#include "ChessPiece.hpp"
#include "Cursor.hpp"
#include "Judge.hpp"
#define horizontalCount 15
#define verticalCount 15

// 棋盘的长度是57字符 高度是29字符

class ChessBoard
{
public:
    static ChessBoard *getChessBoard()
    {
        if (chessboard == nullptr)
        {

            chessboard = new ChessBoard();
        }

        return chessboard;
    }
    void showBoard() // 输出棋盘信息
    {
        FILE *file = fopen("ChessBoard.txt", "r");
        if (!file)
        {
            perror("fopen:");
            return;
        }
        fprintf(stderr, "\033[2J");          // 放到1 1位置上
        fprintf(stderr, "\033[1;1H\033[0m"); // 左上角对其 (1,1) 因为(0,0)和(1,1)重叠 按0处理有问题
        char buf[1024] = {0};
        while (fgets(buf, sizeof(buf), file))
        {
            fprintf(stderr, "%s", buf);
        }
    }
    // 放置棋子 也就是更新棋盘数组 另外x y坐标和 数组有个映射关系
    bool updateArray(ChessPiece *chesspiece)
    {
        int row = (chesspiece->getY() - 1) / 2;
        int column = (chesspiece->getX() - 1) / 4;

        if (nullptr == arr[row][column])
        {
            arr[row][column] = chesspiece;
            showPiece(chesspiece->getX(), chesspiece->getY(), chesspiece->getColor());
            this->x = chesspiece->getX();
            this->y = chesspiece->getY();
            this->color = chesspiece->getColor();
            return true; // 放棋成功
        }
        else
        {
            return false; // 放棋失败该位置有棋子
        }
    }
    // 输出棋子位置的函数
    void showPiece(int x, int y, string color)
    {
        if ("白" == color)
        {
            fprintf(stderr, "\033[%d;%dH\033[47m[0]\033[0m", y, x - 1);
        }
        else
            fprintf(stderr, "\033[%d;%dH\033[40m[0]\033[0m", y, x - 1);
    }

    int getX()
    {
        return this->x;
    }
    int getY()
    {
        return this->y;
    }
    bool isWin()
    {
        return judge.isWin(this->x, this->y, this->color, this->arr);
    }

    ~ChessBoard()
    {
        for (int i = 0; i < 15; i++)
        {
            for (int j = 0; j < 15; j++)
            {
                if (arr[i][j])
                {
                    delete arr[i][j];
                }
            }
        }
    }

private:
    ChessBoard()
    {
        fprintf(stderr, "debug");
        for (int i = 0; i < 15; i++)
        {
            for (int j = 0; j < 15; j++)
            {
                arr[i][j] = nullptr;
            }
        }
    }
    /* 光标的坐标 */
    int x;
    int y;
    string color;
    Judge judge;
    ChessPiece *arr[horizontalCount][verticalCount]; // 一个存放棋子指针的二维数组
    static ChessBoard *chessboard;
};
ChessBoard *ChessBoard::chessboard = nullptr;

#endif