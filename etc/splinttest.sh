#! /bin/sh
[ -d ./src ] && cd src
cd ../src
echo '-*- compilation -*-' > ../splinttest.out
splint -bugslimit 99 -redef -usevarargs +boolint +posixlib +voidabstract -I ../include -I ../temp `ls -1 *.c *.h | egrep -v 'wrap|pick|ick_ec'` >> ../splinttest.out
