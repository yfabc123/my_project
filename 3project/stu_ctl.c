#include "stu_ctl.h"
// 定义
linkList_t *head = NULL;
void login()
{

    char account[32];
    char passwd[32];
    printf("===============登陆===============\n");
    printf("账号:");
    fgets(account, 32, stdin);
    account[strlen(account) - 1] = '\0';
    printf("密码:");
    fgets(passwd, 32, stdin);
    passwd[strlen(passwd) - 1] = '\0';
    while (strcmp("root", account) != 0 || strcmp("root", passwd) != 0)
    {
        printf("账号密码不匹配请重新输入:\n");
        printf("账号:");
        fgets(account, 32, stdin);
        account[strlen(account) - 1] = '\0';
        printf("密码:");
        fgets(passwd, 32, stdin);
        passwd[strlen(passwd) - 1] = '\0';
    }
    printf("登陆成功!\n");
    printf(" ┏━━━━━━━━━━━━━━━━━━━━━━━┓ \n");
    printf(" ┃      学生管理系统     ┃ \n");
    printf(" ┣━━━━━━━━━━━━━━━━━━━━━━━┫ \n");
    printf(" ┃      1. 增加学生      ┃ \n");
    printf(" ┃      2. 查找学生      ┃ \n");
    printf(" ┃      3. 输出学生      ┃ \n");
    printf(" ┃      4. 修改学生      ┃ \n");
    printf(" ┃      5. 删除学生      ┃ \n");
    printf(" ┃      6. 退出系统      ┃ \n");
    printf(" ┗━━━━━━━━━━━━━━━━━━━━━━━┛\n");
    int choice;
    while (1)
    {
        printf("请选择业务类型:");
        int result = getchar();
        clearInputBuf(); // 不调用的话 缓存中存了一个'\n';
        switch (result)
        {
        case '1':
            if (!head)
            {
                create();
            }
            add();
            break;
        case '2':
            find();
            break;
        case '3':
            show();
            break;
        case '4':
            change();
            break;
        case '5':
            del();
            break;
        case '6':

            return;
        default:

            break;
        }
    }
}

// 创建虚拟头节点
void create()
{
    head = (linkList_t *)malloc(sizeof(linkList_t));
    head->next = NULL;
    head->node;
}
// 插入数据
void add()
{
    linkList_t *temp = (linkList_t *)malloc(sizeof(linkList_t));
    memset(temp, 0, sizeof(linkList_t));
    printf("请输入学生的信息[学号 姓名 性别 年龄 分数]:");
    char buf[1024] = {0};
    fgets(buf, sizeof(buf), stdin);
    int num = sscanf(buf, "%d %s %s %d %lf", &temp->node.id, temp->node.name,
                     temp->node.sex, &temp->node.age, &temp->node.score);
    if (num != 5)
    {
        printf("有参数输入不对,本次数据未插入\n");
        memset(temp, 0, sizeof(linkList_t));
        return;
    }
    temp->next = head->next;
    head->next = temp;

    printf("插入成功!\n");
}
//
void find()
{
    if (!head)
    {
        printf("暂无数据\n");
        return;
    }
    printf("请输入要查找学生学号:");
    int id;
    scanf("%d", &id);
    clearInputBuf();
    linkList_t *itr = head->next;
    while (itr && itr->node.id != id)
    {
        itr = itr->next;
    }
    if (!itr)
    {
        printf("没有你要查找的学生\n");
        return;
    }
    else
    {
        printf("----------------------------------------------------\n");
        printf("|%s\t|%s\t|%s\t|%s\t|%s\t|\n", "学号", "姓名", "性别", "年龄", "分数");
        printf("|%d\t|%s\t|%s\t|%d\t|%.1lf\t|\n", itr->node.id, itr->node.name, itr->node.sex, itr->node.age, itr->node.score);

        printf("----------------------------------------------------\n");
    }
}

// 输出
void show()
{
    if (!head)
    {
        printf("暂无数据\n");
        return;
    }
    printf("--------------------学生信息表--------------------------------\n");
    printf("|%s\t|%s\t|%s\t|%s\t|%s\t|\n", "学号", "姓名", "性别", "年龄", "分数");
    linkList_t *itr = head;
    while (itr->next)
    {
        itr = itr->next;
        printf("|%d\t|%s\t|%s\t|%d\t|%.1lf\t|\n", itr->node.id, itr->node.name, itr->node.sex, itr->node.age, itr->node.score);
    }
}
// 修改
void change()
{
    if (!head)
    {
        printf("暂无数据\n");
        return;
    }
    int id;
    int type;
    int newInt;
    double newScore;
    char newStr[32];
    printf("请输入要修改的学生学号:");
    scanf("%d", &id);
    clearInputBuf();
    linkList_t *itr = head->next;
    while (itr && itr->node.id != id)
    {
        itr = itr->next;
    }
    if (!itr)
    {
        printf("没有你要查找的学生\n");
        return;
    }
    printf(" ┏━━━━━━━━━━━━━━━━━━━━━━━┓ \n");
    printf(" ┃        学生信息       ┃ \n");
    printf(" ┣━━━━━━━━━━━━━━━━━━━━━━━┫ \n");
    printf(" ┃      1. 修改学号      ┃ \n");
    printf(" ┃      2. 修改姓名      ┃ \n");
    printf(" ┃      3. 修改性别      ┃ \n");
    printf(" ┃      4. 修改年龄      ┃ \n");
    printf(" ┃      5. 修改分数      ┃ \n");
    printf(" ┃      6. 退出          ┃ \n");
    printf(" ┗━━━━━━━━━━━━━━━━━━━━━━━┛\n");
    printf("请输入你要修改的学生信息编号:");
    scanf("%d", &type);

    clearInputBuf();
    if (type == 6)
    {
        return;
    }
    printf("请输入你要修改的新的数据:");
    switch (type)
    {
    case 1:
        scanf("%d", &newInt);
        clearInputBuf();
        itr->node.id = newInt;
        printf("修改成功\n");
        break;
    case 2:
        scanf("%s", newStr);
        clearInputBuf();
        strcpy(itr->node.name, newStr);
        printf("修改成功\n");

        break;
    case 3:
        scanf("%d", &newInt);
        clearInputBuf();
        strcpy(itr->node.sex, newStr);
        printf("修改成功\n");

        break;
    case 4:
        scanf("%d", &newInt);
        clearInputBuf();
        itr->node.age = newInt;
        printf("修改成功\n");
        break;
    case 5:
        scanf("%le", &newScore);
        clearInputBuf();
        itr->node.score = newScore;
        printf("修改成功\n");

        break;
    case 6:
        break;
    default:
        break;
    }
}
// 删除
void del()
{
    if (!head)
    {
        printf("暂无数据\n");
        return;
    }
    int id;
    printf("请输入要删除的学生学号:");
    scanf("%d", &id);
    clearInputBuf();
    linkList_t *itr = head;
    while (itr->next && itr->next->node.id != id)
    {
        itr = itr->next;
    }
    if (!itr->next)
    {
        printf("没有你要删除的学生\n");
        return;
    }

    linkList_t *temp = itr->next;
    itr->next = itr->next->next;
    temp->next = NULL;
    free(temp);
    printf("删除成功\n");
}
void clearInputBuf()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}
