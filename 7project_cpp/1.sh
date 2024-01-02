

rm /mnt/hgfs/filesharing/serverDir/1.exe /mnt/hgfs/filesharing/serverDir/2.exe 
rm /mnt/hgfs/filesharing/clientDir/3.exe /mnt/hgfs/filesharing/clientDir/4.exe
g++ -g  ServerMain.cpp Server.cpp TcpSocket.cpp Transfer.cpp Global.cpp  -o server -pthread

./server
