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

void alerta_cierre_programa(std::string msg);

void mensaje(std::string msg, std::string titulo);

class ventana
{
private:
  HWND hwnd_;
  const char* nombre_;
  std::string nombre_clase_win32_;
  std::shared_ptr<Mjolnir> mjol_;
  //std::function<void(int)> f_teclado_;
  //std::function<void(int,int,int,int,void*)> f_mouse_;

public:
  static const char * sigh;
  static RECT rEscritorio;
  static HINSTANCE instancia_programa_;

  LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK _stWndProc_(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  inline static ventana* ventanaDeHwnd(HWND hWnd) { return (ventana*)GetWindowLong(hWnd, GWLP_USERDATA); }

  static void registrarClase();
  HWND get_hwnd() const { return hwnd_; }
  void crearVentana();
  void inicializar_diagrama();
  void configuramos_parametros_diagrama();
  void mover(int x, int y, int w, int h) { MoveWindow(hwnd_, x, y, w, h, TRUE); }

  ventana(const char* nombre):
    nombre_(nombre),
    nombre_clase_win32_(std::string("_c") + nombre),
    mjol_(std::make_shared<Mjolnir>(nombre_) )
  {
    GetWindowRect(GetDesktopWindow(), &rEscritorio);// guarda el tamaño de la pantalla a la variable escritorio
    crearVentana();
  }

  void iniciar()
  {
    //al arrancar el programa calculamos las dimensioens de la pantalla(escritorio) y las guardamos como una variable estática
    //const HWND hEscritorio = GetDesktopWindow();// obtén un handle a la ventana del escritorio

    inicializar_diagrama();
    configuramos_parametros_diagrama();

    mjol_->iniciar_callbacks();

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
