#ifndef GUI_H
#define GUI_H

#define SHINE_ON_MIKE 1
#define WIN32

#include <queue>
#include <mutex>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

extern std::queue<int(*)()> queue_funptrs;
extern std::mutex mtx_funptrs;

void push_funptr(int(*funptr)());
int(*extract_funptr())();

void callback1(void*);
int main_gui();
int foo_gui();
int foobar();

#endif // GUI_H
