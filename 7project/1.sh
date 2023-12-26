rm /mnt/hgfs/filesharing/serverDir/1.exe /mnt/hgfs/filesharing/serverDir/2.exe 
rm /mnt/hgfs/filesharing/clientDir/3.exe /mnt/hgfs/filesharing/clientDir/4.exe
gcc  serverMain.c server.c tcpSocket.c transfer.c global.c  -o server -lpthread

./server
