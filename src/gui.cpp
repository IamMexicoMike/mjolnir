#include "gui.h"
#include <iostream>
#include <FL/Fl_RGB_Image.H>

std::queue<int(*)()> queue_funptrs;
std::mutex mtx_funptrs;

void push_funptr(int(*funptr)())
{
  std::lock_guard<std::mutex> lck(mtx_funptrs);
  std::cout << "Valor: " << funptr << std::endl;
  queue_funptrs.emplace(funptr);
}

int (*extract_funptr())()
{
  std::lock_guard<std::mutex> lck(mtx_funptrs);
  if(!queue_funptrs.empty())
  {
    int (*funptr)() = queue_funptrs.back();
    queue_funptrs.pop();
    return funptr;
  }
  else
    return nullptr;
}

/**Este callback se llama cada 10ms y ejecuta funciones que el hilo del diagrama manda a ejecutar,
en el hilo de la GUI*/
void callback_fptrs(void*)
{
  int (*pfun)() = extract_funptr();
  if(pfun != nullptr)
  {
      pfun();
  }

  Fl::repeat_timeout(0.01, callback_fptrs);
}

int main_gui()
{
  Fl::add_timeout(0.01, callback_fptrs);
  Fl_Window *window = new Fl_Window(300,280);
  Fl_Box *box = new Fl_Box(20,40,260,100,"Venga Mike");
  box->box(FL_UP_BOX);
  box->labelsize(36);
  box->labelfont(FL_BOLD+FL_ITALIC);
  box->labeltype(FL_SHADOW_LABEL);
  window->end();
  window->show();

  //probando
  Fl::background(24,250,122);

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
  //Fl::run();
  return 0;
}
