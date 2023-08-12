import resource;
import input;

import <headers-test>;


TEST(context, context) {
    using namespace minecpp;
    int i = 5;
    while(i--){
        try{
            Context ctx {1920, 1080};
            Drawer drawer;
            InputProcessor processor;
            ctx.startLoop([&]{
                processor.processInput();
                drawer.draw();
            });
        }catch(std::string a){
            fmt::println("{}", a);
        }
    }
}