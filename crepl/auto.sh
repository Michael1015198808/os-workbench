i=0
while true;do
    inotifywait -e modify crepl.c 1>/dev/null 2>/dev/null
    clear
    if make;
    then echo 1;#Compile success
    else false;#Compile Error
    fi
    i=$((i+1))
    if [ $i -eq 10 ]#Make git log per 10 times
    then
        echo 123;
        i=0
    fi
done
