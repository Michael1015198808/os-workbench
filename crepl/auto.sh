i=9
clear
while true;do
    inotifywait -e modify crepl.c 1>/dev/null 2>/dev/null
    clear
    if make;
    then
        ./crepl-64 < in;#Compile success
    else false;#Compile Error
    fi
    i=$((i+1))
    if [ $i -eq 10 ];#Don't use now
    then
        i=1;
    fi
done
