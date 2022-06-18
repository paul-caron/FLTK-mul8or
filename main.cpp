// chip8 vm
#include "chip8.hpp"

// fltk
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

// libstd++
#include <iostream>
#include <map>
#include <cmath>


// globals and not extern
// keyboard mapping to chip8 keys
static std::map<char,int>keymap={
{'x',0},{'1',1},{'2',2},{'3',3}, 
{'q',4},{'w',5},{'e',6},{'a',7},
{'s',8},{'d',9},{'z',10},{'c',11},
{'4',12},{'r',13},{'f',14},{'v',15}
};

static Chip8 c8 = Chip8(); // Chip8 VM instance
static const int W = 720; // Width of Chip8 output
static const int H = 360; // Height of Chip8 output
static bool rom_loaded = false; // True if rom is loaded.
static int speed = 4;

void load_ROM(const char * path){
    std::cout << "Loading ROM: " << path << std::endl;
    c8.reset();
    if(!c8.load(path)){
        std::cerr << "ROM could not be loaded" << std::endl;
        exit(1);
    }
    rom_loaded = true;
    std::cout << "ROM loaded successfully" << std::endl;
}

void open_callback(Fl_Widget* w, void * data){
    char *newfile = fl_file_chooser("Open File?", "*.ch8", "/Users/Home/");
    if(newfile != NULL)
        load_ROM(newfile);
}

void exit_callback(Fl_Widget* w, void * data){
    exit(0);
}

void speedup_callback(Fl_Widget* w, void * data){
    speed = speed + 1;
    speed = fmin(10, speed);
}

void speeddown_callback(Fl_Widget* w, void * data){
    speed = speed - 1;
    speed = fmax(1, speed);
}

class Chip8_Window: public Fl_Window{

private:

Chip8 * c8;
Fl_Menu_Bar * menubar;

public:

Chip8_Window(Chip8 * chip8,int w, int h, const char * title): Fl_Window(w,h,title){
    c8 = chip8;
    begin();
    {
        menubar = new Fl_Menu_Bar(0,0,W,30,"MENU");
        menubar->box(FL_UP_BOX);
        menubar->add("File/&Open",FL_COMMAND + 'o', open_callback);
        menubar->add("File/&Exit",FL_COMMAND + 'q', exit_callback);
        menubar->add("Settings/&Speed Up",FL_COMMAND + '+', speedup_callback);
        menubar->add("Settings/&Speed Down",FL_COMMAND + '-', speeddown_callback);
    }
    end();
    this->resizable(0);
}

void draw(){
    if(rom_loaded == false){
        draw_children();
        return;
    }
    for(int i=0;i<speed;++i)
        c8->cycle();
    
    fl_draw_box(FL_FLAT_BOX, 0, 30, W, H, FL_WHITE);

    fl_color(FL_BLACK);    
    int i=0;
    for(int y=0;y<(1+c8->extended_mode)*32;++y)
    for(int x=0;x<(1+c8->extended_mode)*64;++x){
        if(c8->gfx[i++])
            fl_rectf(x*W/(64*(1+c8->extended_mode)),
                     30+ y*H/(32*(1+c8->extended_mode)),
                     ceil(W/(64.0*(1+c8->extended_mode))),
                     ceil(H/(32.0*(1+c8->extended_mode))));
    }

    if(c8->sound_timer)
        fl_beep(FL_BEEP_DEFAULT);
    
    draw_children();
}

int handle(int event){
    if(event == FL_KEYDOWN){
        int key = Fl::event_key();
        for(auto & k: c8->key) k=false;
        if(keymap.find(key)!=keymap.end()){
            c8->key[keymap[key]] = true;
            return 1;
        }
        return 0;
    }
    else if(event == FL_KEYUP){
        for(auto & k: c8->key) k=false;
    }
    else if(event == FL_PUSH){
        return menubar->handle(event);
    }
    return 0;
}
};

int main(int argc, char ** argv){
    Chip8_Window win(&c8,W,H+30,"FLTK CHIPMUL8OR");
    win.show();
    std::cout << "Welcome to fltk-chipmul8or" << std::endl;
    
    while(Fl::first_window()){
        Fl::first_window()->redraw();
        Fl::wait(0.001);
    }

    return 0;
}

