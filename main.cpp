#include <iostream>
#include <thread>

#include "redes.h"
#include "ventana_principal.h"
#include "mjolnir.hpp"
#include "elemento_diagrama.h"
#include "postgres_funciones.h"
#include "sync.h"

using namespace std;
using namespace cv;

/** Symbols for long term thinking */ /* qué acidos me metí cuando escribí eso?!*/

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
  db::conectar_db();
  db::prueba_db();
  db::entablar_escuchador_db();

  ventana v(hInstance, "Mjolnir");
  //ventana v2(hInstance, "ventana dos");
  //v.mover(0,0,ventana::rEscritorio.right, ventana::rEscritorio.bottom);

  db::construir_objetos_sincronizados();

  MSG Msg;
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
