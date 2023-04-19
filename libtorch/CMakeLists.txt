cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(learnlibtorch)

# 指定std版本
set(CMAKE_CXX_STANDARD 11) 

add_definitions(-D _GLIBCXX_USE_CXX11_ABI=0)

set(Torch_Root ${CMAKE_SOURCE_DIR}/3rdparty/libtorch)
#添加TorchConfig.cmake路径
set(Torch_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libtorch/share/cmake/Torch)
# set(ATen_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libtorch/share/cmake/ATen)
# set(Caffe2_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libtorch/share/cmake/Caffe2)
# set(Gloo_DIR ${CMAKE_SOURCE_DIR}3rdparty/libtorch/share/cmake/Gloo)
# set(Tensorpipe_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libtorch/share/cmake/Tensorpipe)

INCLUDE_DIRECTORIES(
    ${Torch_Root}/include
    ${Torch_Root}/include/torch/csrc/api/include
    )

find_package(Torch REQUIRED)

add_executable(dcgan dcgan.cpp)
target_link_libraries(dcgan "${TORCH_LIBRARIES}")
set_property(TARGET dcgan PROPERTY CXX_STANDARD 14)