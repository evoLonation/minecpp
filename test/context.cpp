import <stl>;
import <fmt>;
import resource;
import input;
import context;

#include <snitch_all.hpp>

TEST_CASE("context", "[context]") {
    using namespace minecpp;
    int i = 3;
    while(i--){
        try{
            Context ctx {1920, 1080};
            Drawer drawer;
            InputProcessor processor;
            ctx.startLoop([&]{
                fmt::println("frame duration: {}", ctx.getFrameDuration());
                processor.processInput();
                drawer.draw();
            });
        }catch(std::string a){
            fmt::println("{}", a);
        }
    }
}