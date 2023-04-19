if [ ! -d "build" ];then
    mkdir build
else
    echo "build文件夹已经存在"
fi

cd build

cmake -DUSE_NEW_LOG4CXX=ON ..
# cmake -DUSE_NEW_LOG4CXX=OFF ..

make -j 16