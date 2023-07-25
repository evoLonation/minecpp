## gl
opengl本身的逻辑存在系统的驱动中，如果没有工具帮助，我们就需要在程序中手动寻找这些接口的位置然后存到函数指针中调用。但是通过工具我们就可以省略这个过程。

[Dav1dde/glad: Multi-Language Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs. (github.com)](https://github.com/Dav1dde/glad)

[https://gen.glad.sh](https://gen.glad.sh/)

glad通过这个网站生成不同api的loader。选择3.3版本的core-profile，并勾选loader，然后点击generate。

生成出来的：

- include中的头文件放入项目目录的include目录下
- src中的源代码可以先链接为静态库，然后放到项目的lib目录下

静态库编译命令如下：
```
g++ --std=c++20 -I ..\..\include\ -c .\lib.cpp
ar rcs libstbimage.a .\lib.o
```
## fmtd

c++ libfmt库
https://github.com/fmtlib/fmt

## glfw3

[glfw/glfw: A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input (github.com)](https://github.com/glfw/glfw)

从github中下载release（我这里下载了win64）

> GLFW是一个用C语言编写的库，专门针对OpenGL。GLFW为我们提供了将好东西呈现到屏幕上所需的基本必需品。它允许我们创建OpenGL上下文、定义窗口参数和处理用户输入，这对于我们的目的来说已经足够了。
> 

我们需要的是：

- include中的头文件， 将其移到项目目录的include目录下
- 编译器对应的库文件（这里是mingw-w64）其中包含：
  - libglfw3.a，静态链接库，如果使用静态链接则需要**链接此库和gdi32库**
  - glfw3.dll，动态链接库，如果使用动态链接，需要放到和可执行文件相同目录下
  - libglfw3dll.a，动态链接库的引用文件，如果使用动态链接，则链接的时候需要通过-lglfw3dll链接该库。

## imgui

通过github的项目 
https://github.com/ocornut/imgui 
编译而来。

CMakeLists.txt:
```cmake
cmake_minimum_required(VERSION 3.0.0)
project(imgui)

# 设置 头文件 目录
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
# imgui glfw 相关的backend 与glfw存在依赖，因此需要包含glfw头文件
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include")

set(SOURCES)
# 如果要通过通配符获取文件，必须使用下面的命令
# GLOB表示使用文件的模式匹配
file(GLOB sub_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(APPEND SOURCES ${sub_files})
# 如果要添加单个文件，直接使用下面的命令
list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp")
list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_glfw.cpp")

# 设置 c++ 版本
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(imgui STATIC ${SOURCES})
```

## stbimage

https://github.com/nothings/stb

原项目是一个头文件库，但是需要在且只能在一个cpp文件中使用 `#define STB_IMAGE_IMPLEMENTATION` 来引入实现。

本项目为了方便起见，将实现打包为静态库，cpp文件如下：
```c++
#define STB_IMAGE_IMPLEMENTATION
// 开启utf-8字符支持
#define STBI_WINDOWS_UTF8
#include <stb_image.h>
```

## assimp

一个用于导入模型的库

github: https://github.com/assimp/assimp/tree/master


构建文档： https://github.com/assimp/assimp/blob/master/Build.md

使用自带的CmakeList.txt编译为动态库（编译为静态库后链接过程很慢）

通过编写CmakeList，在构建时将dll复制到二进制文件目录下：
```cmake
# 将动态库复制到二进制文件目录下
add_custom_command(
    TARGET minecpp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/dll/libassimp-5.dll"
            "${CMAKE_CURRENT_BINARY_DIR}/libassimp-5.dll"
)
```