
不容易 三个月前刚过完c基础语法的我面对俄罗斯方块这个项目整个人都自闭了觉得好难 我肯定写不了这个 于是跳过 接着学其他的知识 如今回过来头重新分析了下 梳理逻辑一个功能
一个功能实现 真"面向过程编程"  终于拿下 除了一些vt100规则 终端关闭回显规则等引用了下 其余也算是用自己思路实现了 不容易!



![俄罗斯方块项目设计思路](https://github.com/yfabc123/my_project/assets/103840107/876efb5d-d67f-4859-b599-7dd36b764fd8)
![image](https://github.com/yfabc123/my_project/assets/103840107/06d48ec5-d4e0-4552-b97c-73f8237a634e)


```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <termio.h>
// 初始化图形坐标
int init_x = 24;
int init_y = 6;

// 右侧提示模块图形信息
int next_x = 46;
int next_y = 8;
int score = 0;  // 初始分数
int level = 1;  // 初始等级
int t = 800000; // 初始下落速度 单位微妙

int next_shape; // 开始前设置为-1
int next_direction;
int next_color;

// 当前图形信息(因为坐标和方向这三个变量需要基于当前信息变化的)
int cur_x;
int cur_y;
int cur_shape;
int cur_direction;
int cur_color;

// 设置毫秒定时器
void alarm_us(int n)
{
    struct itimerval value;
    // 设置定时器启动的初始化值n
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = n;

    // 设置定时器启动后的间隔数
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = n;

    // 1s后启动定时器，同时发出SIGALRM信号
    // 每间隔5S发送一次SIGALRAM信号
    setitimer(ITIMER_REAL, &value, NULL);
}

struct termios tm_old; // 保存正常标准输入的属性

/* index 16离有边界距离 index 17 离下边界距离 */
int element[7][4][18] =
    {
        {
            {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2}, //
            {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2}, //
            {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2}, //[][]
            {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2}, //[][]
        },
        {
            {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0}, //[]  [][][][]
            {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3}, //[]
            {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0}, //[]
            {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3}, //[]
        },
        {
            {0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2}, //  []    []      [][][]     []
            {1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1}, //[][][]  [][]      []     [][]
            {1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2}, //        []                 []
            {0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 1}  //
        },
        {
            {1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2}, //[][]       []
            {0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1}, //  [][]   [][]
            {1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2}, //         []
            {0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1}, //
        },
        {
            {0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2}, //  [][]   []
            {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 1}, //[][]     [][]
            {0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2}, //           []
            {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 1}, //
        },
        {
            {0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2}, //    []   []    [][][]  [][]
            {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 2, 1}, //[][][]   []    []        []
            {1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2}, //         [][]            []
            {1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 1}  //
        },
        {{1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2},  //[]       [][]  [][][]      []
         {1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1},  //[][][]   []        []      []
         {1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2},  //         []              [][]
         {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 2, 1}}, //
};

int store_shape[24][14] = {0}; // 数组存放颜色值 需要把坐标和数组映射起来  y-6, (x-12)/2

// 整体下移一行
void replace(int line) // 需要消除的行
{
    for (int i = line; i >= 1; i--)
    {
        for (int j = 0; j < 14; j++)
        {
            store_shape[i][j] = store_shape[i - 1][j];
        }
    }
}
// 找到可以消的行
void disappear()
{
    for (int i = 23; i >= 0; i--)
    {
        for (int j = 0; j < 14; j++)
        {
            if (store_shape[i][j] == 0)
            {
                break;
            }
            if (j == 13) // 此时i的值就是可以消的行
            {
                replace(i);
                score++;
                if (score == 2)
                {

                    score = 0;
                    level++;
                    t = t - 100000;
                    alarm_us(t);
                }
                printf("\33[%d;%dH分数:%d\33[0m", 18, 45, score);
                printf("\33[%d;%dH等级:%d\33[0m", 22, 45, level);
                i++;
            }
        }
    }
    // 输出数组
    for (int i = 0; i < 24; i++)
    {
        for (int j = 0; j < 14; j++)
        {
            // y-6, (x-12)/2
            if (store_shape[i][j] != 0)
            {
                printf("\033[%d;%dH\033[%dm[]\033[0m", i + 6, 2 * j + 12, store_shape[i][j]);
            }
            else
            {
                printf("\033[%d;%dH\033[%dm  \033[0m", i + 6, 2 * j + 12, store_shape[i][j]);
            }
        }
    }
}
// 判断是否触底 并处理
int bottom_boundary(int x, int y, int shape, int direction, int color)
{
    // 假设图形下移一位后的坐标(数组)有数据的话 那么表示遇到边界 需要停止并将当前颜色信息保存至数组
    int result = 0;
    if ((y + 3 - element[shape][direction][17]) >= 30) // 越界
    {
        result = 1;
    }
    else
    {
        for (int i = 0; i < 16; i++)
        {
            if (element[shape][direction][i] == 1)
            {                                                                    // 底部遇到其他图形   横坐标:2*(i%4)+xx  纵坐标:i/4+yy
                if (store_shape[i / 4 + y - 6][(2 * (i % 4) + x - 12) / 2] != 0) // 发生碰撞
                {
                    result = 1;
                    break;
                }
            }
        }
    }
    if (result)
    {
        // 保存图形颜色信息
        for (int i = 0; i < 16; i++)
        {
            if (element[shape][direction][i] == 1) // 横坐标:2*(i%4)+xx  纵坐标:i/4+yy
            {                                      // 注意这里保存的是y-1 因为参数传的是当前位置的下移位置
                store_shape[i / 4 + y - 1 - 6][(2 * (i % 4) + x - 12) / 2] = color;
            }
        }

        for (int i = 0; i < 16; i++)
        {
            // 如果最上行有数据则游戏结束
            if (store_shape[0][i] != 0)
            {
                printf("game over");
                tcsetattr(0, 0, &tm_old); // 恢复正常终端属性
                printf("\033[?25h");      // 显示光标

                exit(EXIT_FAILURE);
            }
        }
        disappear(); // 触底之后消行处理
    }

    return result;
}
// 判断是否碰到左右边界以及是否和其他图形碰撞
int is_boundary(int x, int y, int shape, int direction, int color)
{
    int result = 0;
    if (x < 12 || (x + 2 * (3 - element[shape][direction][16]) > 38)) // 左右下越界
    {
        result = 1;
    }
    else
    {
        // 和其他图形发生碰撞
        for (int i = 0; i < 16; i++)
        {
            if (element[shape][direction][i] == 1) // 横坐标:2*(i%4)+x  纵坐标:i/4+yy
            {
                if (store_shape[i / 4 + y - 6][(2 * (i % 4) + x - 12) / 2] != 0) // 越界
                {
                    result = 1;
                    break;
                }
            }
        }
    }
    return result;
}

//  指定坐标位置 指定图案形状 指定方向 指定颜色输出图形 需要5个参数
/*
[] 占两个横坐标
 */
void print_mode_shape(int x, int y, int shape, int direction, int color)
{
    // printf("\33[2J");

    for (int i = 0; i < 16; i++)
    {
        if (element[shape][direction][i] == 1) // 横坐标:2*(i%4)+x  纵坐标:i/4+y
        {
            printf("\033[%d;%dH\033[%dm[]\033[0m", i / 4 + y, 2 * (i % 4) + x, color);
        }
    }
    fflush(stdout);
}
/* ui界面设计:
游戏空间
宽度:14个小坐标点 也就是28个光标 或者(由于终端的光标宽度只有高度的一半为了形成正方形更美观一点于是)
高度:24个小坐标点 24个光标
右边提示界面容纳坐标点:7*6

边框: 上边框纵坐标为5 加上容纳游戏的24个坐标 在加上下边框 所有左边范围是 5到30
 */
void print_start_ui()
{
    printf("\33[2J");
    // 打印上下两条横杠 每一行输出24个坐标点也就是48个光标
    for (int i = 0; i < 47; i++)
    {
        // printf("\33[43m \33[%d;%dH\33[0m", 5, 10 + i);  //不知道为啥把颜色控制放前面会有点小问题..
        //  printf("\33[43m \33[%d;%dH\33[0m", 30, 10 + i);
        printf("\33[%d;%dH\33[43m \33[0m", 5, i + 10);
        printf("\33[%d;%dH\33[43m \33[0m", 30, i + 10);
    }
    // 打印左中右3条竖杠 每一列输出26次

    for (int i = 0; i < 26; i++)
    {
        printf("\33[%d;%dH\33[43m  \33[0m", 5 + i, 10);
        printf("\33[%d;%dH\33[43m  \33[0m", 5 + i, 40);
        printf("\33[%d;%dH\33[43m  \33[0m", 5 + i, 56);
    }

    // 打印分数和下次图形提示的分割线
    for (int i = 0; i < 16; i++)
    {
        printf("\33[%d;%dH\33[43m \33[0m", 12, i + 42);
    }
    // // 输出分数和等级
    //  18  45
    printf("\33[%d;%dH分数:%d\33[0m", 18, 45, score);
    // 22  45
    printf("\33[%d;%dH等级:%d\33[0m", 22, 45, level);
    printf("\33[?25l");//隐藏光标
    //printf("\33[u");

    fflush(stdout);
}
void erase(int x, int y, int shape, int direction)
{

    for (int i = 0; i < 16; i++)
    {
        if (element[shape][direction][i] == 1) // 横坐标:2*(i%4)+x  纵坐标:i/4+y
        {
            printf("\033[%d;%dH  \033[0m", i / 4 + y, 2 * (i % 4) + x);
        }
    }
    fflush(stdout);
}

/*
游戏开始时生成图形
 */
void print_start_shape()
{
    cur_shape = random() % 7;
    cur_direction = random() % 4;
    cur_color = random() % 8 + 40;

    cur_x = init_x;
    cur_y = init_y;
    print_mode_shape(init_x, init_y, cur_shape, cur_direction, cur_color);
}
/* 游戏进行时生成新图形 */
void print_game_shape()
{
    cur_shape = next_shape;
    cur_direction = next_direction;
    cur_color = next_color;

    cur_x = init_x;
    cur_y = init_y;
    print_mode_shape(init_x, init_y, next_shape, next_direction, next_color);
}

// 提示板块生成的图形
void print_next_shape()
{
    erase(next_x, next_y, next_shape, next_direction);
    next_shape = random() % 7;
    next_direction = random() % 4;
    next_color = random() % 8 + 40;
    print_mode_shape(next_x, next_y, next_shape, next_direction, next_color);
}

// 下落
void move_down()
{
    if (bottom_boundary(cur_x, cur_y + 1, cur_shape, cur_direction, cur_color))
    {
        print_game_shape();
        print_next_shape();
    }
    else
    {
        erase(cur_x, cur_y, cur_shape, cur_direction);
        cur_y++;
        print_mode_shape(cur_x, cur_y, cur_shape, cur_direction, cur_color);
    }
}
// 左移
void move_left()
{

    if (is_boundary(cur_x - 2, cur_y, cur_shape, cur_direction, cur_color) == 0)
    {
        erase(cur_x, cur_y, cur_shape, cur_direction);
        cur_x = cur_x - 2;
        print_mode_shape(cur_x, cur_y, cur_shape, cur_direction, cur_color);
    }
}
// 右移
void move_right()
{
    if (is_boundary(cur_x + 2, cur_y, cur_shape, cur_direction, cur_color) == 0)
    {
        erase(cur_x, cur_y, cur_shape, cur_direction);
        cur_x = cur_x + 2;
        print_mode_shape(cur_x, cur_y, cur_shape, cur_direction, cur_color);
    }
}

// 旋转可出现两种情况
void move_up()
{
    // 先判断当前是否触底 也就是看下一个是图形是否到底
    if (bottom_boundary(cur_x, cur_y + 1, cur_shape, cur_direction, cur_color))
    {
        print_game_shape();
        print_next_shape();
    }
    // 判断是否碰撞或者越界
    else if (is_boundary(cur_x, cur_y, cur_shape, (cur_direction + 1) % 4, cur_color) == 0)
    {
        erase(cur_x, cur_y, cur_shape, cur_direction);
        cur_direction = (cur_direction + 1) % 4;
        print_mode_shape(cur_x, cur_y, cur_shape, cur_direction, cur_color);
    }
}
void do_alarm()
{
    move_down();
}

// 不回显输入 同时设置为原始模式
int getch()
{
    struct termios tm;
    tcgetattr(0, &tm_old); // 文件描述符0 标准输入
    cfmakeraw(&tm);        // tm是输出参数用来保存将结构体属性设置为原始模式
    tcsetattr(0, 0, &tm);  // tm输入参数 设置当前的标准输入为原始模式即不回显 也不用输入回车程序直接检测到输入的字符
    int ch = getchar();
    tcsetattr(0, 0, &tm_old); // 恢复正常输入属性
    return ch;
}
// 控制图形移动  关闭回显
void key_control()
{
    while (1)
    {
        int ch = getch();
        if (ch == 'q' || ch == 'Q')
        {
            printf("\033[?25h");

            exit(EXIT_SUCCESS);
        }
        else if (ch == 'j')
        {
            move_left();
        }
        else if (ch == 'k')
        {
            move_down();
        }
        else if (ch == 'l')
        {
            move_right();
        }
        else if (ch == 'i')
        {
            move_up();
        }
    }
}

int main()
{
    srand((unsigned)time(NULL));
    print_start_ui();
    print_start_shape();
    print_next_shape();
    signal(SIGALRM, do_alarm);
    alarm_us(t);
    key_control();
    return 0;
}
```
