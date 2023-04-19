if [ ! -d "build" ];then
    mkdir build
else
    echo "build文件夹已经存在"
fi

cd build

cmake -DUSE_FIND_PACKAGE=OFF -DSPDLOG_BUILD_EXAMPLE_HO=ON ..

make -j 16