#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
// 星期的映射表
char *weeks[7] = {"星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"};
// 每月天数 默认是平年
int month_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
typedef struct
{
    int year;
    int month;
    int day;
    int week; // 0到6表示周一到周日
} date;
// 将输入的年月日 创建结构体
date *create_date()
{
    date *today = (date *)malloc(sizeof(date));
    if (today == NULL)
    {
        printf("内存分配失败。\n");
        exit(EXIT_FAILURE);
    }
    printf("请输入年 月 日三个参数:");
    scanf("%d %d %d", &today->year, &today->month, &today->day);
    return today;
}
// 判断平闰年方法
int is_leap(date *para)
{
    return (para->year % 400 == 0) || ((para->year % 4 == 0 && para->year % 100));
}

// 当前日期减一天的计算逻辑
date *sub_day_1(date *today)
{
    today->week = (today->week + 6) % 7;
    today->day--;
    // 情况1 1月1日
    if (today->day == 0 && today->month == 1)
    {
        today->day = 31;
        today->month = 12;
        today->year--;
    }
    // 情况2  3月一日 需要判断今年是闰年还是平年
    else if (today->day == 0 && today->month == 3)
    {
        if (is_leap(today))
        {
            today->day = 29;
        }
        else
        {
            today->day = 28;
        }
        today->month = 2;
    }
    // 情况三 其他月份一日
    else if (today->day == 0)
    {
        today->month--;
        today->day = month_day[today->month - 1];
    }
    return today;
}
// 当前日期加一天的计算逻辑
date *add_day_1(date *today)
{

    today->week = (today->week + 1 + 7) % 7;
    today->day++;
    // 情况1 12月31日
    if (today->month == 12 && today->day == 32)
    {
        today->day = 1;
        today->month = 1;
        today->year++;
    }
    // 情况2  2月28日 需要判断今年是闰年还是平年
    else if (today->day == 29 && today->month == 2)
    {
        if (is_leap(today))
        {
            today->day = 29;
        }
        else
        {
            today->day = 1;
            today->month = 3;
        }
    }
    // 情况三 其他月份最后一天
    else if (today->day > month_day[today->month - 1])
    {
        today->month++;
        today->day = 1;
    }

    return today;
}

// 计算给定年月日的星期
void week(date *today)
{
    date temp = {2023, 10, 6, 4}; // 先找到一个参照日期
    int a = today->year * 10000 + today->month * 100 + today->day;
    int b = temp.year * 10000 + temp.month * 100 + temp.day;
    if (a > b) // 输入日期比模板日期大
    {
        while (temp.year != today->year || temp.month != today->month || temp.day != today->day)
        {

            add_day_1(&temp);
        }
    }
    else // 输入日期比模板日期小
    {
        while (temp.year != today->year || temp.month != today->month || temp.day != today->day)
        {
            sub_day_1(&temp);
        }
    }

    today->week = temp.week;
}

// 计算该天是这一年第几天 逻辑就是不断减一天 减到1月1日停止
void day(date *today)
{
    int i = 1;
    date temp = *today;

    while (temp.day != 1 || temp.month != 1)
    {
        sub_day_1(&temp);
        i++;
    }
    printf("%d年%d月%d日是一年中的第%d天\n", today->year, today->month, today->day, i);
}
// 根据日期创建日历表 只显示当月的 日和星期
void Calendar(date *para)
{
    int cal_max; // 传入日期当月的天数
                 // 判断闰年还是平年
    if (is_leap(para))
    {
        month_day[1] = 29;
    }
    cal_max = month_day[para->month - 1];
    int number = 1; // 往日历表填数的起始天数 后续一次递增

    //   date temp = {2023, 10, 6, 5};                      // 参考日期标准
    date para_coppy = {para->year, para->month, 1, -1}; // 定义一个 月首结构 星期随便赋个值
    week(&para_coppy);                                  // 确认月首是星期几

    //   para_1->week = temp.week; // 获取传入日期当前月第一天的 星期
    // 构建二维数组 日历表
    int table[6][7]; // 月份最多可能有6行

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (i == 0 && j < para_coppy.week)
            {
                table[i][j] = 0;
            }
            else if (number <= cal_max)
            {
                table[i][j] = number;
                number++;
            }
            else
                table[i][j] = 0;
        }
    }
    // 输出日历结果
    printf("\t\t%d年%d月的日历表\n", para->year, para->month);
    for (int i = 0; i < 7; i++)
    {
        printf("%s\t", weeks[i]);
    }
    printf("\n");
    // 打印前面 还有后面的空格

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (table[i][j] == 0)
            {
                printf("\t");
            }
            else
            {
                printf("%d\t", table[i][j]);
            }
        }
        printf("\n");
    }
}

int main()
{
    date *today = create_date();
    if (is_leap(today))
    {
        printf("%d年%d月%d日是闰年\n", today->year, today->month, today->day);
    }
    else
    {
        printf("%d年%d月%d日是平年\n", today->year, today->month, today->day);
    }
    day(today);
    week(today);
    printf("%d年%d月%d日是%s\n", today->year, today->month, today->day, weeks[today->week]);
    Calendar(today);
    free(today);
    return 0;
}