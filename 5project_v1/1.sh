a=`ipcs -q  |grep "666"| awk '{print$2}'`
if [ -n "$a" ] 
then
for i in $a
do
ipcrm -q $i
done
fi

a=`ipcs -s  |grep "666"| awk '{print$2}'`
if [ -n "$a" ] 
then
for i in $a
do
ipcrm -s $i
done
fi

a=`ipcs -m  |grep "666"| awk '{print$2}'`
if [ -n "$a" ] 
then
for i in $a
do
ipcrm -m $i
done
fi
