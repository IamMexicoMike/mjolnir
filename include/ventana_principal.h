#ifndef VENTANA_PRINCIPAL_H
#define VENTANA_PRINCIPAL_H

#define WIN32_LEAN_AND_MEAN //se supone que hace muchos headers derivados no sean incluidos, y por ende acelerar compilación

#include <functional>
#include <windows.h>
#include <windowsx.h>
#include <mjolnir.hpp>
#include <elemento_diagrama.h>

#define ID_T30 1001
#define ID_T1000 1002
#define ID_PB1 5001
#define ID_RB1 5101
#define ID_RB2 5102
#define ID_CB1 5201
#define IDC_MAIN_EDIT 6000

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool crearVentana(HWND& hwnd, HINSTANCE& hInstance);
void inicializar_diagrama(HWND& hwnd);
void configuramos_parametros_diagrama(HWND& hwnd);
BOOL CALLBACK DialogoTextoProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void crear_dialogo_objeto(objeto* pobj);
void alerta_cierre_programa(std::string msg);

void crear_dialogo_objeto(objeto* pobj);
void mensaje(std::string msg, std::string titulo);

class CallbackBase
{
public:
  virtual void operator()() const { };
  virtual ~CallbackBase() = 0;
};

CallbackBase::~CallbackBase() { }

template<typename T>
class Callback : public CallbackBase
{
public:
  typedef void (T::*F)();

  Callback( T& t, F f ) : t_(&t), f_(f) { }
  void operator()() const { (t_->*f_)(); }

private:
  T* t_;
  F  f_;
};

template<typename T>
Callback<T> make_callback( T& t, void (T::*f) () )
{
  return Callback<T>( t, f );
}

class ventana
{
private:
  HWND hwnd_;
  const char* nombre_;
  Mjolnir mjol_;
  std::function<void(int)> f_teclado_;
  std::function<void(int,int,int,int,void*)> f_mouse_;

public:
  static RECT rEscritorio;
  static HINSTANCE instancia_programa_;

  LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  void registrarClase();
  HWND get_hwnd() const { return hwnd_; }
  void crearVentana();
  void inicializar_diagrama();
  void configuramos_parametros_diagrama();
  void mover(int x, int y, int w, int h) { MoveWindow(hwnd_, x, y, w, h, TRUE); }

  ventana(const char* nombre):
    nombre_(nombre),
    mjol_(nombre)
  {
    registrarClase();
    //al arrancar el programa calculamos las dimensioens de la pantalla(escritorio) y las guardamos como una variable estática
    //const HWND hEscritorio = GetDesktopWindow();// obtén un handle a la ventana del escritorio
    GetWindowRect(GetDesktopWindow(), &rEscritorio);// guarda el tamaño de la pantalla a la variable escritorio

    crearVentana();
    inicializar_diagrama();
    configuramos_parametros_diagrama();

    ShowWindow(hwnd_, SW_MAXIMIZE);
    UpdateWindow(hwnd_);

    auto t30 = SetTimer(hwnd_, ID_T30, 30, NULL);
    if(t30 == 0)
      throw std::runtime_error("Error estableciendo timer");

    auto t1000 = SetTimer(hwnd_, ID_T1000, 1000, NULL);
    if(t1000 == 0)
      throw std::runtime_error("Error estableciendo timer");
  }
};

#endif // VENTANA_PRINCIPAL_H
