#ifndef GUI_H
#define GUI_H

#define SHINE_ON_MIKE 1

#include <queue>
#include <mutex>
#include <atomic>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

extern std::atomic_bool b_gui_activa;

extern std::queue<int(*)()> queue_funptrs;
extern std::mutex mtx_funptrs;

void push_funptr(int(*funptr)());
int(*extract_funptr())();

void callback_fptrs(void*);
int main_gui();
int foo_gui();
int foobar();

////////////////////
#ifndef awalkinthewoods_h
#define awalkinthewoods_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <iostream>
extern Fl_Input *ti_servidor;
#include <FL/Fl_Light_Button.H>
extern Fl_Light_Button *boton_chk;
Fl_Double_Window* make_window();
#endif

#endif // GUI_H
