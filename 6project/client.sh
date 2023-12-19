gcc -g clientMain.c client.c dataBase.c mySocket.c  -lmysqlclient  -lpthread -o client
./client