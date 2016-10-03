#include "gui.h"

int main_gui()
{
  Fl::lock();
  Fl_Window *window = new Fl_Window(300,180);
  Fl_Box *box = new Fl_Box(20,40,260,100,"Hello, World!");
  box->box(FL_UP_BOX);
  box->labelsize(36);
  box->labelfont(FL_BOLD+FL_ITALIC);
  box->labeltype(FL_SHADOW_LABEL);
  window->end();
  window->show();
  return Fl::run();
}

int foo_gui()
{

  Fl_Window *window = new Fl_Window(300,300);
  Fl_Box *box = new Fl_Box(20,40,260,100,"Hello, World!");
  box->box(FL_UP_BOX);
  box->labelsize(36);
  box->labelfont(FL_BOLD);
  box->labelcolor(14);
  window->end();
  window->show();
  Fl::run();
  return 0;
}
