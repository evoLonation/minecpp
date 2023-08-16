set_languages("c++20")
set_toolchains("clang")

-- 使用xmake的模块功能时会出现没有include这个目录的bug，因此手动include
add_includedirs("D:/Apps/msys64/mingw64/lib/clang/16/include")

-- 将header unit所在目录设置为全局的include查找目录，方便import<>
add_includedirs("./header-module")

-- compile_commands.json 自动更新
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

-- 依赖之间的执行顺序是：比如B依赖A，则执行顺序为A.load->B.load->A.build->B.build等等，两者的各阶段是穿插进行的

-- 头文件单元的预编译，在 config 期间编译，并且会设置 header-modules 标签为pcm文件目录
target("header-modules")
    set_kind("object")
    add_headerfiles("header-module/*")
    -- 有个问题是，使用xmake build 构建与本target无关的其他target，仍然会进行该target的配置行为
    on_config(function (target)
        import("core.project.depend")
        print(target:name()..": prebuild the module header")
        for _, headerfile in ipairs(target:headerfiles()) do
            local filename = path.filename(headerfile)
            local target_path = target:autogendir().."/header-modules/"..filename..".pcm"
            print("header file: "..filename..", target path: "..target_path)
            depend.on_changed(function ()
                print("no cache, need rebuild it")
                os.mkdir(path.directory(target_path))
                -- -xc++-header 参数后面的文件会被视为头文件（无论后缀）
                os.execv("clang", {"-fmodule-header", "-xc++-header", headerfile, "-std=c++20", "-fretain-comments-from-system-headers",  "-o", target_path, "-I./include"})
            end, {files = {headerfile, target_path}})
            target:add("header-modules", target_path)
        end
        
    end)
target_end()

-- .mpp模块文件的预编译并且在build时会设置 module-path 标签为pcm文件目录
target("modules")
    set_kind("object")
    add_deps("header-modules")
    on_config(function (target)
        print(target:name()..": config the header file flags")
        for _, dep in pairs(target:deps()) do 
            if dep:name() == "header-modules" then
                for _, path in pairs(dep:get("header-modules")) do 
                    target:add("cxxflags", "-fmodule-file="..path)
                end 
            end 
        end 
    end)
    add_cxxflags("-fretain-comments-from-system-headers")
    add_files("src/**.mpp")
    set_policy("build.c++.modules", true)
    after_build(function (target) 
        import("lib.detect.find_file")
        local modulepath = path.directory(find_file("*.pcm", { target:autogendir().."**"}))
        target:set("module-path", modulepath)
    end)
target_end()

-- 依赖解析会在config之后，build之前进行
target("test")
    set_kind("binary")
    add_deps("header-modules")
    on_config(function (target)
        for _, dep in pairs(target:deps()) do 
            if dep:name() == "header-modules" then
                for _, path in pairs(dep:get("header-modules")) do 
                    target:add("cxxflags", "-fmodule-file="..path)
                end
            end
        end
    end)
    add_deps("modules")
    before_build(function (target)
        for _, dep in pairs(target:deps()) do 
            if dep:name() == "modules" then
                target:add("cxxflags", "-fprebuilt-module-path="..dep:get("module-path"))
            end
        end
    end)
    add_cxxflags("-fretain-comments-from-system-headers")
    add_files("test/*.cpp")
    set_policy("build.c++.modules", true)
    add_linkdirs("./lib")
    add_links("glfw3", "gl", "gdi32", "fmtd", "imgui", "stbimage", "assimp.dll", "gtest_main", "gtest")
    set_targetdir("./build")
target_end()

target("minecpp")
    set_kind("binary")
    add_deps("header-modules")
    on_config(function (target)
        for _, dep in pairs(target:deps()) do 
            if dep:name() == "header-modules" then
                for _, path in pairs(dep:get("header-modules")) do 
                    target:add("cxxflags", "-fmodule-file="..path)
                end
            end
        end
    end)
    add_deps("modules")
    before_build(function (target)
        for _, dep in pairs(target:deps()) do 
            if dep:name() == "modules" then
                target:add("cxxflags", "-fprebuilt-module-path="..dep:get("module-path"))
            end
        end
    end)
    add_cxxflags("-fretain-comments-from-system-headers")
    add_files("src/*.cpp")
    set_policy("build.c++.modules", true)
    add_linkdirs("./lib")
    add_links("glfw3", "gl", "gdi32", "fmtd", "imgui", "stbimage", "assimp.dll")
    set_targetdir("./build")
target_end()