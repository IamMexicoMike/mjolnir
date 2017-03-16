#include <iostream>
#include <thread>

#include "redes.h"
#include "ventana_principal.h"
#include "mjolnir.hpp"
#include "elemento_diagrama.h"
#include "postgres_funciones.h"

using namespace std;
using namespace cv;

/** Symbols for long term thinking */

extern void cargar_variables_configuracion();
extern void mostrar_gif(string);

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

/* Si no lo intentas entonces definitivamente tienes 0% de chance */
/* Ta ta raaan! Ta ta raaan! */
/* Ta raaan taan! */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
  cargar_variables_configuracion();
  thread hilo_redes(redes_main); //hilo de redes siempre debe de correr
  conectar_db();
  prueba_db();

  WNDCLASSEX wc;
  HWND hwnd;
  MSG Msg;

  if(!registrarClase(wc, hInstance))
    return -1;

  if(!crearVentana(hwnd, hInstance))
    return -1;

  inicializar_diagrama(hwnd);
  configuramos_parametros_diagrama(hwnd);

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  auto t30 = SetTimer(hwnd, ID_T30, 30, NULL);
  if(t30 == 0)
    MessageBox(hwnd, "Error al llamar SetTimer() de t30", "Error", MB_OK | MB_ICONEXCLAMATION);

  auto t5000 = SetTimer(hwnd, ID_T5000, 5000, NULL);
  if(t5000 == 0)
    MessageBox(hwnd, "Error al llamar SetTimer() de t5000", "Error", MB_OK | MB_ICONEXCLAMATION);

  while(GetMessage(&Msg, NULL, 0, 0) > 0)
  {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  //fin de la aplicación
  iosvc.stop();
  hilo_redes.join();

  return Msg.wParam;
}
