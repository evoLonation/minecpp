import resource;
import input;
import gui;
import context;
import <gtest>;
import <fmt>;

using namespace minecpp;
class TestWindow: public GuiWindow<TestWindow>{
friend GuiWindow<TestWindow>;
private:
    float a;
    void innerDraw(){
        slider("abc", a);
    }
public:
    using GuiWindow<TestWindow>::GuiWindow;
};

TEST(gui, gui) {
    try{
    Context ctx {1920, 1080};
    Drawer drawer;
    InputProcessor processor;
    TestWindow window {{"123"}, false};
    ctx.startLoop([&]{
        drawer.draw();
        processor.processInput();
    });
    }catch(std::string a){
        fmt::println("{}", a);
    }
}