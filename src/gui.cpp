#include "gui.h"
#include <iostream>
#include <memory>
#include <FL/Fl_RGB_Image.H>

extern void empujar_queue_saliente(std::string s); //redes.cpp
extern std::string extraer_queue_saliente();

std::queue<int(*)()> queue_funptrs;
std::mutex mtx_funptrs;

std::atomic_bool b_gui_activa;

void push_funptr(int(*funptr)())
{
  std::lock_guard<std::mutex> lck(mtx_funptrs);
  queue_funptrs.emplace(funptr);
}

int (*extract_funptr())()
{
  std::lock_guard<std::mutex> lck(mtx_funptrs);
  if(!queue_funptrs.empty())
  {
    int (*funptr)() = queue_funptrs.front();
    queue_funptrs.pop();
    return funptr;
  }
  else
    return nullptr;
}

/**Este callback se llama cada 10ms y ejecuta funciones que el hilo del diagrama (o el de redes)
 mandan a ejecutar, en el hilo de la GUI*/
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
  Fl::add_timeout(0.1, callback_fptrs);
  auto window = std::make_unique<Fl_Window>(300,280);
  Fl_Box *box = new Fl_Box(20,40,260,100,"constexpr noexcept");
  box->box(FL_UP_BOX);
  box->labelsize(36);
  box->labelfont(FL_BOLD+FL_ITALIC);
  box->labeltype(FL_SHADOW_LABEL);
  window->end();
  window->show();

  //probando
  Fl::background(24,150,122);

  auto rrr = Fl::run();
  b_gui_activa = false; //avisar que salimos de la gui
  return rrr;
}

int foo_gui()
{
  auto window = std::make_unique<Fl_Window>(300,300);
  Fl_Box *box = new Fl_Box(20,40,260,100,"Mike ftw");
  box->box(FL_UP_BOX);
  box->labelsize(36);
  box->labelfont(FL_BOLD);
  box->labelcolor(14);
  window->end();
  window->show();
  Fl_Double_Window* vv = make_window();
  vv->end();
  vv->show();
  //Fl::run();
  return 0;
}

Fl_Input *ti_servidor=(Fl_Input *)0;

static void cb_ti_servidor(Fl_Input* i, void*) {
  std::string s = i->value();
  std::cout << s << std::endl;
  empujar_queue_saliente(s);
}

Fl_Light_Button *boton_chk=(Fl_Light_Button *)0;

static void cb_boton_chk(Fl_Light_Button*, void*) {
  std::string s = "click";
  std::cout << s << std::endl;
  empujar_queue_saliente(s);
}

Fl_Double_Window* make_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(378, 220, "Ventana");
    w = o;
    { ti_servidor = new Fl_Input(68, 25, 103, 24, "servidor");
      ti_servidor->box(FL_GTK_DOWN_BOX);
      ti_servidor->callback((Fl_Callback*)cb_ti_servidor);
      ti_servidor->when(FL_WHEN_ENTER_KEY_ALWAYS);
    } // Fl_Input* ti_servidor
    { boton_chk = new Fl_Light_Button(80, 68, 68, 22, "button");
      boton_chk->box(FL_GTK_UP_BOX);
      boton_chk->down_box(FL_GTK_DOWN_BOX);
      boton_chk->selection_color((Fl_Color)79);
      boton_chk->callback((Fl_Callback*)cb_boton_chk);
      boton_chk->when(FL_WHEN_CHANGED);
    } // Fl_Light_Button* boton_chk
    o->end();
  } // Fl_Double_Window* o
  return w;
}
