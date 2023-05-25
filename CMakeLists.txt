cmake_minimum_required(VERSION 3.0.0)
project(minecpp VERSION 0.1.0)


# add sub diretory
set ( SUB_LIST )
file ( GLOB sub_files "${CMAKE_CURRENT_SOURCE_DIR}/render/*.cpp" )
list ( APPEND SUB_LIST ${sub_files} )


set ( ROOT_LIST)
file ( GLOB root_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
list ( APPEND ROOT_LIST ${root_files} )


# 设置 头文件 目录
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${INCLUDE_DIR})
# 设置 库文件 目录
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
link_directories(${LIB_DIR})

# 设置 c++ 版本
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(minecpp ${ROOT_LIST} ${SUB_LIST})

target_link_libraries(minecpp glfw3 gl gdi32)