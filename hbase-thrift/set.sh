if [ ! -d "build" ];then
    mkdir build
else
    echo "build文件夹已经存在"
fi

cd build

# 使用thrift-0.10.0
# cmake -DBuild_Thrift_0_10=ON ..
cmake ..

make -j 8