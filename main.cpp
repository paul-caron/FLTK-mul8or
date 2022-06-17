// chip8 vm
#include "chip8.hpp"

// fltk
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

// libstd++
#include <iostream>
#include <map>
#include <cmath>

// global
std::map<char,int>keymap={
{'x',0},
{'1',1},
{'2',2}, 
{'3',3}, 
{'q',4},
{'w',5}, 
{'e',6}, 
{'a',7}, 
{'s',8},
{'d',9},
{'z',10},
{'c',11},
{'4',12}, 
{'r',13}, 
{'f',14}, 
{'v',15}
};

// global
Chip8 c8 = Chip8();
const int W = 720;
const int H = 360;

class Chip8_Window:public Fl_Window{
Chip8 * c8;

public:
Chip8_Window(Chip8 * chip8,int w, int h, const char * title): Fl_Window(w,h,title){
    c8 = chip8;
}
void draw(){
    for(int i=0;i<4;++i) c8->cycle();
    
    fl_draw_box(FL_FLAT_BOX,
                    0,
                    0,
                    W,
                    H,
                    FL_WHITE);
    fl_color(FL_BLACK);    
    int i=0;
    for(int y=0;y<(1+c8->extended_mode)*32;++y)
    for(int x=0;x<(1+c8->extended_mode)*64;++x){
        if(c8->gfx[i++]) fl_rectf(x*W/(64*(1+c8->extended_mode)), y*H/(32*(1+c8->extended_mode)), ceil(W/(64.0*(1+c8->extended_mode))), ceil(H/(32.0*(1+c8->extended_mode))));
    }

    if(c8->sound_timer) fl_beep(FL_BEEP_DEFAULT);

}
int handle(int event){
    if(event == FL_KEYDOWN){
        int key = Fl::event_key();
        for(auto & k: c8->key) k=false;
        if(keymap.find(key)!=keymap.end()){
            c8->key[keymap[key]] = true;
            return 1;
        }
    }
    else if(event == FL_KEYUP){
        for(auto & k: c8->key) k=false;
    }
    return 0;
}
};


void load_ROM(const char * path){
    std::cout << "Loading ROM: " << path << std::endl;
    if(!c8.load(path)){
        std::cerr << "ROM could not be loaded" << std::endl;
        exit(1);
    }
    std::cout << "ROM loaded successfully" << std::endl;
}

int main(int argc, char ** argv){
    // second argument x for extended mode
    if(argc > 2 && argv[2][1] == 'x') c8.extended_mode = true;
    Chip8_Window win(&c8,W,H,"FLTK CHIPMUL8OR");
    win.show();
    std::cout << "Welcome to fltk-chipmul8or" << std::endl;
    // first argument is the rom path
    if(argc > 1) { load_ROM(argv[1]); }
    else{
        std::cout << "Please provide a ROM path: " ;
        std::string path;
        std::cin >> path;
        load_ROM(path.c_str());
    }
    while(Fl::first_window()){
        Fl::first_window()->redraw();
        Fl::wait(0.0001);        
    }

    return 0;
}
