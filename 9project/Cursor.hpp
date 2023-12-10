#ifndef CURSOR_HEAD_H
#define CURSOR_HEAD_H
#define centerX 29
#define certerY 15
class Cursor
{
public:
    Cursor()
    {
        x = centerX;
        y = certerY;
        show();
    }
    int getX()
    {
        return x;
    }
    int getY()
    {
        return y;
    }
    void show()
    {
        fprintf(stderr, "\033[%d;%dH\033[0m", y, x);
    }
    void moveUp()
    {
        if (y > 1)
        {
            this->y -= 2;
            show();
        }
        else
            return;
    }
    void moveDown()
    {
        if (y < 29)
        {
            this->y += 2;
            show();
        }
        else
            return;
    }
    void moveRight()
    {
        if (x < 57)
        {
            this->x += 4;
            show();
        }
        else
            return;
    }
    void moveLeft()
    {
        if (x > 1)
        {
            this->x -= 4;
            show();
        }
        else
            return;
    }

private:
    int x;
    int y;
};
#endif