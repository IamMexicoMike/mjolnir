#include <iostream>
#include <chrono> //para profiles
#include <ctime> //para profiles
#include <fstream>
#include <algorithm>
#include <bitset> //borrame si no debuggeas
#include <future>
#include <thread>
#include <cstdio>

#include <opencv2/opencv.hpp>

#include "redes.h"
#include "mjolnir.hpp"
#include "elemento_diagrama.h"


#include "gui.h"
#include "cuenta_nueva.h"

#include "recurso.h"
#include "windowsx.h"

using namespace std;
using namespace cv;

extern void cargar_variables_configuracion();
extern void procesar_queue_cntrl();
extern void mostrar_gif(string);

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

/* La forma más rápida de fracasar es no intentarlo */
/* Querer es poder */
const char* nombreClase= "claseVentanaPrincipal";
const char* nombreDiagrama="diagrama planta";
const int ID_TIMER = 42;

bool registrarClase(WNDCLASSEX& wc, HINSTANCE& hInstance)
{
  wc.cbSize        = sizeof(WNDCLASSEX);
  wc.style         = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
  wc.lpfnWndProc   = WndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hInstance;
  wc.hIcon         = /*LoadIcon(NULL, IDI_APPLICATION);*/LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MJOLNIR));
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MYMENU);
  wc.lpszClassName = nombreClase;
  wc.hIconSm       = /*LoadIcon(NULL, IDI_APPLICATION);*/(HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MJOLNIR), IMAGE_ICON, 16, 16, 0);

  if(!RegisterClassEx(&wc))
  {
      MessageBox(NULL, "Error en registrar la clase de la ventana!", "Error!",
          MB_ICONEXCLAMATION | MB_OK);
      return false;
  }
  return true;
}

bool crearVentana(HWND& hwnd, HINSTANCE& hInstance)
{
  const HWND hEscritorio = GetDesktopWindow();// obtén un handle a la ventana del escritorio
  RECT escritorio;
  GetWindowRect(hEscritorio, &escritorio);// guarda el tamaño de la pantalla a la variable escritorio
  hwnd = CreateWindowEx(
      WS_EX_CONTEXTHELP,
      nombreClase,
      "MJOLNIR",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, escritorio.right, escritorio.bottom,
      NULL, NULL, hInstance, NULL);

  if(hwnd == NULL)
  {
    MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    return false;
  }
  return true;
}

void inicializar_diagrama(HWND& hwnd)
{
  RECT rVentana;
  GetWindowRect(hwnd, &rVentana);// guarda el tamaño de la pantalla a la variable escritorio
  ancho_region = (rVentana.right - 200);
  altura_region = (rVentana.bottom);
  dxy = Point(ancho_region/2, ancho_region/2);

  cout << rVentana.right << '\t' << rVentana.bottom << '\n'
       << ancho_region << '\t' << altura_region << endl;

  region = Mat(altura_region, ancho_region, CV_8UC4, cv::Scalar(200,200,200));
  mat_header = Mat(region.colRange(0, region.cols).rowRange(0,30)); //mn
  HEADER_MSG = Point(region.cols-300, HEADER0.y);

  anexar_zonas();
  rellenar_zona_telares();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
  cargar_variables_configuracion();
  std::thread hilo_redes(redes_main); //hilo de redes siempre debe de correr

  WNDCLASSEX wc;
  HWND hwnd;
  MSG Msg;

  //Paso 1: Registramos la clase de la ventana.
  if(!registrarClase(wc, hInstance))
    return -1;

  // Paso 2: Creamos la ventana.
  if(!crearVentana(hwnd, hInstance))
    return -1;

  inicializar_diagrama(hwnd);
  renderizarDiagrama(region);

  namedWindow(nombreDiagrama, CV_WINDOW_AUTOSIZE);

  HWND hWnd2 = (HWND) cvGetWindowHandle(nombreDiagrama);
  HWND hParent = ::GetParent(hWnd2);
  ::SetParent(hParent, hwnd);
  EnableMenuItem(GetSystemMenu(hParent, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED);
  SendMessage(hParent, WM_SETICON, ICON_BIG, IDI_MJOLNIR);
  setMouseCallback(nombreDiagrama, manejarInputMouse);

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  auto ret = SetTimer(hwnd, ID_TIMER, 15, NULL);
    if(ret == 0)
        MessageBox(hwnd, "Error al llamar SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);

/*
  while (true)
  {
    int k = waitKey(30);

    if(k == 27) //tecla ESC
    {
      destroyWindow("Mjolnir");
      break;
    }

    if(k != -1) //-1 es retornado cuando el timer expira o cuando se cierra la ventana
      manejarInputTeclado(region, k);

    renderizarDiagrama(region);
    setMouseCallback("Mjolnir", manejarInputMouse);
    procesar_queue_cntrl();
  }
*/
  //fin de la aplicación

  // Step 3: The Message Loop
  while(GetMessage(&Msg, NULL, 0, 0) > 0)
  {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }

  iosvc.stop();
  hilo_redes.join();

  return Msg.wParam;
}
