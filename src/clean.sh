MYROOT=/home/s/news_process_main/
find $MYROOT -name "*.xml" -mtime +7 | xargs rm -rf
find $MYROOT -name "*.data" -mtime +7 | xargs rm -rf
find $MYROOT/log/ -name "*" -mtime +3 | xargs rm -rf
find $MYROOT/output -name "*.txt" -mtime +7 | xargs rm -rf
find $MYROOT/input.bak -name "*" -mtime +1 | xargs rm -rf
find $MYROOT/input -name "*.xml" -mmin +120 | xargs rm -rf
