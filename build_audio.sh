file=$1;

g++ -g -c -fpermissive -fPIC -lm ./user_files/$1.cpp -o audio.o &&
g++ -g -shared -o libaudio.so audio.o