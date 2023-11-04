#ifndef LOCALGAME_HEAD_H
#define LOCALGAME_HEAD_H
#include "ChessBoard.hpp"
#include "ChessPiece.hpp"
#include "BlackChessPlayer.hpp"
#include "WhiteChessPlayer.hpp"
#include "KeyBoard.hpp"
#include "Cursor.hpp"
#include "WinLog.hpp"
class LocalGame
{
public:
    LocalGame(string BlackChessPlayerName, string WhiteChessPlayerName)
    {
        fprintf(stderr, "\033[2J");
        chessboard = ChessBoard::getChessBoard();
        chessboard->showBoard();
        b = new BlackChessPlayer(BlackChessPlayerName);
        w = new WhiteChessPlayer(WhiteChessPlayerName);
        keyboard = new KeyBoard();
        winlog = new WinLog();
    }
    void runGame()
    {
        bool ok = false;
        while (!ok)
        {
            b->placeChessPiece(keyboard);
            if (chessboard->isWin())
            {
                ok = true;
                winlog->showLog(b);
                break;
            }
            w->placeChessPiece(keyboard);
            if (chessboard->isWin())
            {
                ok = true;
                winlog->showLog(w);

                break;
            }
        }
    }
    ~LocalGame()
    {
        delete chessboard; // 需要手动实现析构因为还有个指针成员变量
        delete keyboard;
        delete b;
        delete w;
        delete winlog;
    }

private:
    ChessBoard *chessboard;
    ChessPlayer *b;
    ChessPlayer *w;
    KeyBoard *keyboard;
    WinLog *winlog;
};
#endif