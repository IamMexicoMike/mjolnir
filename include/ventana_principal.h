#ifndef VENTANA_PRINCIPAL_H
#define VENTANA_PRINCIPAL_H

#define WIN32_LEAN_AND_MEAN //se supone que hace muchos headers derivados no sean incluidos, y por ende acelerar compilación

#include <windows.h>
#include <windowsx.h>
#include <elemento_diagrama.h>

#define ID_T30 1001
#define ID_T1000 1002
#define ID_PB1 5001
#define ID_RB1 5101
#define ID_RB2 5102
#define ID_CB1 5201
#define IDC_MAIN_EDIT 6000

extern HWND hVentanaPrincipal;
extern HWND hEdit;
extern HWND hTool;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool registrarClase(WNDCLASSEX& wc, HINSTANCE& hInstance);
bool crearVentana(HWND& hwnd, HINSTANCE& hInstance);
void inicializar_diagrama(HWND& hwnd);
void configuramos_parametros_diagrama(HWND& hwnd);
BOOL CALLBACK DialogoTextoProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void crear_dialogo_objeto(objeto* pobj);
void alerta_cierre_programa(std::string msg);

void crear_dialogo_objeto(objeto* pobj);
void mensaje(std::string msg, std::string titulo);


class ventana
{
public:
  WNDCLASSEX wc;
  HWND hwnd;
  HINSTANCE hInstance;
  cv::Mat diagrama;
  const char* nombre_;

  void registrarClase(WNDCLASSEX& clase);
  void crearVentana();
  void inicializar_diagrama();
  void configuramos_parametros_diagrama();

  ventana(HINSTANCE h_instancia, const char* nombre):
    hInstance(h_instancia),
    nombre_(nombre)
  {
    registrarClase(wc);
    crearVentana();
    inicializar_diagrama();
    configuramos_parametros_diagrama();

    ShowWindow(hwnd, SW_MAXIMIZE);
    UpdateWindow(hwnd);

    auto t30 = SetTimer(hwnd, ID_T30, 30, NULL);
    if(t30 == 0)
      throw runtime_error("Error estableciendo timer");

    auto t1000 = SetTimer(hwnd, ID_T1000, 1000, NULL);
    if(t1000 == 0)
      throw runtime_error("Error estableciendo timer");
  }
};

#endif // VENTANA_PRINCIPAL_H
