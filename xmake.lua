toolchain("myclang")
    set_toolset("cc", "clang")
    set_toolset("cxx", "clang", "clang++")
toolchain_end()

set_languages("c++20")

-- add_includedirs("D:/Apps/msys64/mingw64/lib/clang/16/include")

target("class")
    set_kind("binary")
    add_files("src/*.cpp")
    -- add_files("main.cpp")
    set_toolchains("myclang")
    add_linkdirs("./lib")
    add_includedirs("./include")
    add_links("glfw3", "gl", "gdi32", "fmtd", "imgui", "stbimage", "assimp.dll")
    set_targetdir("./build")
    -- set_policy("build.c++.modules", true)

