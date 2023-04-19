if [ ! -d "build" ];then
    mkdir build
else
    echo "build文件夹已经存在"
fi
if [ ! -d "./build/resources" ];then  #拷贝resources
    cp ./resources/$file  ./build/resources/$file -r
else
    echo "./build/resources文件夹已经存在"
fi

cd build

cmake ..

make -j 4

if [ ! -f "./libuv-webserver" ];then #执行可执行文件
   echo "./libuv-webserver 不存在"
# else
    # ./libuv-webserver
    # rm ./libuv-webserver
fi