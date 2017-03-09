#include "ventana_principal.h"
#include "mjolnir.hpp"
#include "../recurso.h"
#include "commctrl.h"

using cv::Mat; using cv::Scalar; using cv::Point;
using namespace std;

HWND hEdit;
HWND hTool;
extern Mat region;
extern void renderizarDiagrama(cv::Mat&);
extern void procesar_queue_cntrl();

const char* nombreClase= "claseVentanaPrincipal";

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

  MoveWindow(hwnd, 0, 0, escritorio.right, escritorio.bottom-50, TRUE);

  return true;
}

void inicializar_diagrama(HWND& hwnd)
{
  RECT rVentana;                              //es para guardar el tamaño de la ventana principal temporalmente
  GetWindowRect(hwnd, &rVentana);             //guarda el tamaño de la ventana principal
  ancho_region = (rVentana.right - 200);      //hacemos al diagrama ligeramente más delgado que la ventana principal
  altura_region = (rVentana.bottom);
  dxy = Point(ancho_region/2, ancho_region/2);

  region = Mat(altura_region, ancho_region, CV_8UC4, Scalar(200,200,200));  //instanciamos la matriz del diagrama principal
  mat_header = Mat(region.colRange(0, region.cols).rowRange(0,30)); //instanciamos la submatriz del header
  HEADER_MSG = Point(region.cols-300, HEADER0.y);                   //instanciamos el lugar donde irán los mensajes de diagrama

  preparar_memoria(); //incluir elemento_diagrama.h añadiría costos al tiempo de compilacion
  anexar_zonas();
  rellenar_zona_telares();
}

void configuramos_parametros_diagrama(HWND& hwnd)
{
  cv::namedWindow(nombreDiagrama, CV_WINDOW_AUTOSIZE);
  HWND hWnd2 = (HWND) cvGetWindowHandle(nombreDiagrama);
  HWND hDiagrama = ::GetParent(hWnd2);
  ::SetParent(hDiagrama, hwnd);
  //EnableMenuItem(GetSystemMenu(hDiagrama, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED);
  /***************/
  LONG lStyle = GetWindowLong(hDiagrama, GWL_STYLE);
  lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
  SetWindowLong(hDiagrama, GWL_STYLE, lStyle);
  /***************/
  RECT sz_dia;
  GetWindowRect(hDiagrama, &sz_dia);// guarda el tamaño de la pantalla a la variable escritorio
  //SetWindowPos(hDiagrama, 0, 0, 0, sz_dia.right, sz_dia.bottom, SWP_NOSIZE);
  MoveWindow(hDiagrama, 0, -20, sz_dia.right, sz_dia.bottom, TRUE);
  SendMessage(hDiagrama, WM_SETICON, ICON_BIG, IDI_MJOLNIR);
  cv::setMouseCallback(nombreDiagrama, manejarInputMouse);
  cv::setKeyboardCallback(nombreDiagrama, manejarInputTeclado);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
  case WM_CREATE:
  {

  }
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam))
    {
    case ID_FILE_EXIT:
      DestroyWindow(hwnd);
      break;

    case ID_PB1:
    {
      LPWSTR buffer[256];
      SendMessage(hEdit, WM_GETTEXT, sizeof(buffer)/sizeof(buffer[0]), reinterpret_cast<LPARAM>(buffer));
      MessageBox(NULL, (LPCSTR)buffer, "Click boton", MB_ICONINFORMATION);
      break;
    }
    case ID_ACCION1:
    {
      LPWSTR buffer[256];
      SendMessage(hEdit, WM_GETTEXT, sizeof(buffer)/sizeof(buffer[0]), reinterpret_cast<LPARAM>(buffer));
      MessageBox(NULL, (LPCSTR)buffer, "Informacion!", MB_ICONINFORMATION);
      break;
    }

		break;
    }
	break;
    break;

  case WM_TIMER:
    switch (wParam)
    {
      case ID_T30:
        if(!b_cache_valida)
        {
          renderizarDiagrama(region); //actualizamos el contenido de la matriz
          imshow(nombreDiagrama, region);
          b_cache_valida = true;
        }
        procesar_queue_cntrl();
        break;

      case ID_T5000:
        break;
    }
    break;

  case WM_KEYDOWN:
    {
      break;
    }


  case WM_RBUTTONDOWN:
    {

    }
    break;

  case WM_RBUTTONUP:

    break;

  case WM_LBUTTONDOWN:

    break; ///LBUTTONDOWN

  case WM_LBUTTONUP:

    break;

  case WM_MOUSEMOVE:

    break;

  case WM_RBUTTONDBLCLK:

    break;

  case WM_CLOSE:
    DestroyWindow(hwnd);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}
