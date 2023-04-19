g++ -O3 ./*.cpp -o test

if [ ! -f "./test" ];then #执行可执行文件
   echo "./test 不存在"
else
    ./test
    # rm ./test
fi