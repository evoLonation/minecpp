cmake_minimum_required(VERSION 3.0.0)
project(minecpp VERSION 0.1.0)


# 设置 头文件 目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui)

# 设置 编译文件 目录
set ( SOURCES )
# 如果要通过通配符获取文件，必须使用下面的命令
# GLOB表示使用文件的模式匹配
file ( GLOB sub_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )
list ( APPEND SOURCES ${sub_files} )
# 如果要添加单个文件，直接使用下面的命令
# list ( APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp )

# 设置 库文件 目录
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
link_directories(${LIB_DIR})

# 设置 c++ 版本
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译
add_executable(minecpp ${SOURCES})

# 链接
target_link_libraries(minecpp glfw3 gl gdi32 fmtd imgui)