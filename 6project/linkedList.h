#ifndef LINKEDLIST_HEAD_H
#define LINKEDLIST_HEAD_H
#include "global.h"
typedef struct node
{
    struct node *next;
    Packet userPacket;
} LinkedList;
extern LinkedList *head;
extern LinkedList *createLinkedList();
extern int deleteLinkedList(LinkedList *head, Packet *paraUserPacket);
extern int insertLinkedList(LinkedList *head, Packet *parrUserMessage);
extern int sendAll(LinkedList *head, Packet *pUserPacket, int pchildSocket);
#endif