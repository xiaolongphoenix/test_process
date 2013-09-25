MYROOT=../
find $MYROOT -name "*.xml" -mtime +3 | xargs rm -rf
find $MYROOT -name "*.data" -mtime +3 | xargs rm -rf
find $MYROOT/log/ -name "*" -mtime +1 | xargs rm -rf
find $MYROOT/output -name "*.txt" -mtime +5 | xargs rm -rf
find $MYROOT/input.bak -name "*" -mtime +1 | xargs rm -rf
