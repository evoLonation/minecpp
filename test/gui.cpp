import resource;
import input;
import gui;
import <headers-test>;



TEST(gui, gui) {
    using namespace minecpp;
    try{
    Context ctx {1920, 1080};
    Drawer drawer;
    InputProcessor processor;
    GuiWindow window {{"123"}, false};
    ctx.startLoop([&]{
        drawer.draw();
        processor.processInput();
    });
    }catch(std::string a){
        fmt::println("{}", a);
    }
}