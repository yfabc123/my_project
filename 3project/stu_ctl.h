#ifndef _STUCTL_HEAD_H_
#define _STUCTL_HEAD_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student
{
    int id;
    char name[16];
    char sex[16];
    int age;
    double score;
} student_t;

typedef struct linkList
{
    struct linkList *next;
    student_t node;
} linkList_t;

extern linkList_t *head;
extern void login();
extern void create();
extern void add();
extern void show();
extern void find();
extern void change();

extern void clearInputBuf(); // 有输入的地方都调用下
extern void del();
#endif