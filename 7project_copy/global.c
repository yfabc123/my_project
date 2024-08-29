#include "global.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER; // 读写锁
AvlNode* avlTree = NULL;
//计算高度
int getHeight(AvlNode* root)
{
    return !root ? 0 : root->height;
}
//计算平衡因子
int getBf(AvlNode* root)
{
    return !root ? 0 : getHeight(root->left) - getHeight(root->right);
}
//创建节点
AvlNode* create(int key, const char* name)
{
    AvlNode* node = (AvlNode*)malloc(sizeof(AvlNode));
    node->data = (SocketInfo*)malloc(sizeof(SocketInfo));
    strcpy(node->data->fileName, name);
    node->data->offset = 0;
    node->data->sfd = key;
    node->left = NULL;
    node->right = NULL;
    node->key = key;
    node->height = 1;
    return node;
}
//查找
AvlNode* find(AvlNode* root, int key)
{
    AvlNode* ptr = root;
    while (ptr)
    {
        if (ptr->key == key)
            return ptr;
        if (ptr->key > key)
            ptr = ptr->left;
        else if (ptr->key < key)
            ptr = ptr->right;
    }
    return NULL;
}
// ll插入 即右旋
AvlNode* llRotate(AvlNode* root)
{
    // if (!root)
    //     return NULL;
    AvlNode* temp = root->left;
    // if (!temp)
    //     return NULL;
    root->left = temp->right;
    temp->right = root;
    //注意旋转后需要更新高度?
    root->height = MAX(getHeight(root->left), getHeight(root->right)) + 1;
    temp->height = MAX(getHeight(temp->left), getHeight(temp->right)) + 1;
    return temp; //之前的左节点称为新的根节点
}
// rr插入 即左旋
AvlNode* rrRotate(AvlNode* root)
{
    // if (!root)
    //     return NULL;
    AvlNode* temp = root->right;
    // if (!temp)
    //     return NULL;
    root->right = temp->left;
    temp->left = root;
    root->height = MAX(getHeight(root->left), getHeight(root->right)) + 1;
    temp->height = MAX(getHeight(temp->left), getHeight(temp->right)) + 1;
    return temp;
}
// lr插入 先左旋左子树然后右旋
AvlNode* lrRotate(AvlNode* root)
{
    if (!root)
        return NULL;
    root->left = rrRotate(root->left);
    return llRotate(root);
}
// rl插入 先右旋右子树在左旋
AvlNode* rlRotate(AvlNode* root)
{
    if (!root)
        return NULL;
    root->right = llRotate(root->right);
    return rrRotate(root);
}
/*插入节点 由于这个插入肯定是叶子节点然后会再递归过程更新高度*/
AvlNode* insert(AvlNode* root, int key, const char* name)
{
    if (!root)
    {
        AvlNode* node = create(key, name);
        return node;
    }
    if (key > root->key)
        root->right = insert(root->right, key, name);
    else if (key < root->key)
        root->left = insert(root->left, key, name);
    else //相等不能插入
        return root;
    root->height = MAX(getHeight(root->left), getHeight(root->right)) + 1;
    if (getBf(root) == 2)
    {
        if (getBf(root->left) == 1)
            return llRotate(root);
        else
            return lrRotate(root);
    }
    else if (getBf(root) == -2)
    {
        if (getBf(root->right) == 1)
            return rlRotate(root);
        else
            return rrRotate(root);
    }
    return root;
}
AvlNode* del(AvlNode* root, int key)
{
    if (!root)
        return NULL;
    if (root->key > key)
        root->left = del(root->left, key);
    else if (root->key < key)
        root->right = del(root->right, key);
    else
    {
        if (!root->left || !root->right)
        {
            AvlNode* temp = root->left ? root->left : root->right;
            free(root->data);
            root->data = NULL;
            free(root);
            root = NULL;
            return temp;
        }
        else
        {
            AvlNode* curMax = root->left; //获取左子树的最大值
            while (curMax->right)
            {
                curMax = curMax->right;
            }
            root->key = curMax->key;
            root->left = del(root->left, root->key);
        }
    }
    if (!root)
        return NULL;
    //每次回归重新更新高度
    root->height = MAX(getHeight(root->left), getHeight(root->right)) + 1;
    if (getBf(root) == 2)
    {
        if (getBf(root->left) == 1)
            return llRotate(root);
        else
            return lrRotate(root);
    }
    else if (getBf(root) == -2)
    {
        if (getBf(root->right) == 1)
            return rlRotate(root);
        else
            return rrRotate(root);
    }
    return root;
}
