#include "server.h"
#include "../shm.h"
#include "threadPool.h"
#include "hashTable.h"
#include "../sem.h"
#include "../mq.h"
void runServer()
{
    createShm(2, sizeof(User));
    ThreadPool *tPool = createTpool(2, 2);
    createMq();
    createPubHash();
    createSubHash();
    addTask(tPool);
}
