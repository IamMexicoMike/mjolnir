#include "ventana_principal.h"
#include "mjolnir.hpp"
#include "../recurso.h"
#include "commctrl.h"
#include "postgres_funciones.h"
#include "ficha_tecnica.h"
#include "commdlg.h"
#include "wingdi.h"
#include "sync.h"

using cv::Mat; using cv::Scalar; using cv::Point;
using namespace std;

RECT ventana::rEscritorio;

extern Mat region;
extern void renderizarDiagrama(cv::Mat&);
extern void procesar_queue_cntrl();

const char* nombreClaseVentanaPrincipal= "claseVentanaPrincipal";
static WNDCLASSEX wc;
HINSTANCE ventana::instancia_programa_;

void ventana::registrarClase(HINSTANCE hInstance)
{
  ventana::instancia_programa_ = hInstance; //se use en createwindowEx
  wc.cbSize        = sizeof(WNDCLASSEX);
  wc.style         = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
  wc.lpfnWndProc   = WndProc; //aqu� se asigna el nombre del callback de la clase
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hInstance;
  wc.hIcon         = /*LoadIcon(NULL, IDI_APPLICATION);*/LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MJOLNIR));
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MYMENU);
  wc.lpszClassName = nombreClaseVentanaPrincipal;
  wc.hIconSm       = /*LoadIcon(NULL, IDI_APPLICATION);*/(HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MJOLNIR), IMAGE_ICON, 16, 16, 0);

  if(!RegisterClassEx(&wc))
      throw std::except("Error en registrar la clase de la ventana!");
}


void ventana::crearVentana()
{
  hwnd_ = CreateWindowEx(
      WS_EX_CONTEXTHELP,
      nombreClaseVentanaPrincipal,
      nombre_,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, rEscritorio.right, rEscritorio.bottom,
      NULL, NULL, ventana::instancia_programa_, NULL);

  if(hwnd_ == NULL)
    throw "Error creando ventana";

  mover(0, 0, ventana::rEscritorio.right, ventana::rEscritorio.bottom-50); //demasiada magia
}

void ventana::inicializar_diagrama()
{
  RECT rVentana;                              //es para guardar el tama�o de la ventana principal temporalmente
  GetWindowRect(hwnd_, &rVentana);             //guarda el tama�o de la ventana principal
  ancho_region = (rVentana.right - 200);      //hacemos al diagrama ligeramente m�s delgado que la ventana principal
  altura_region = (rVentana.bottom);
  dxy = Point(ancho_region/2, ancho_region/2);

  mjol_.diagrama_ = Mat(altura_region, ancho_region, CV_8UC4, Scalar(200,200,200));  //instanciamos la matriz del diagrama principal
  mjol_.encabezado_ = Mat(region.colRange(0, region.cols).rowRange(0,30)); //instanciamos la submatriz del header
  HEADER_MSG = Point(region.cols-300, HEADER0.y);                   //instanciamos el lugar donde ir�n los mensajes de diagrama

  preparar_memoria(); //sospechoso
  anexar_zonas();
  rellenar_zona_telares();
}

void ventana::configuramos_parametros_diagrama()
{
  cv::namedWindow(nombre_, CV_WINDOW_AUTOSIZE);
  HWND hWnd2 = (HWND) cvGetWindowHandle(nombre_);
  HWND hDiagrama = ::GetParent(hWnd2);
  ::SetParent(hDiagrama, hwnd_);
  //EnableMenuItem(GetSystemMenu(hDiagrama, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED);
  /***************/
  LONG lStyle = GetWindowLong(hDiagrama, GWL_STYLE);
  lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
  SetWindowLong(hDiagrama, GWL_STYLE, lStyle);
  /***************/
  RECT sz_dia;
  GetWindowRect(hDiagrama, &sz_dia);// guarda el tama�o de la pantalla a la variable escritorio
  SetWindowPos(hDiagrama, 0, 0, 0, sz_dia.right, sz_dia.bottom, SWP_NOSIZE);
  //mover( 0, -20, sz_dia.right, sz_dia.bottom);
  SendMessage(hDiagrama, WM_SETICON, ICON_BIG, IDI_MJOLNIR);
  cv::setMouseCallback(nombre_, manejarInputMouse); //probablemente esto por cada instancia, en lugar de en el constructor
  cv::setKeyboardCallback(nombre_, manejarInputTeclado);// "
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
  case WM_CREATE:
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
      //SendMessage(hEdit, WM_GETTEXT, sizeof(buffer)/sizeof(buffer[0]), reinterpret_cast<LPARAM>(buffer));
      MessageBox(NULL, (LPCSTR)"dise�ame", "Click boton", MB_ICONINFORMATION);
      break;
    }
    case ID_ACCION1:
    {
      LPWSTR buffer[256];
      //SendMessage(hEdit, WM_GETTEXT, sizeof(buffer)/sizeof(buffer[0]), reinterpret_cast<LPARAM>(buffer));
      MessageBox(NULL, (LPCSTR)/*buffer*/"accion1", "Informacion!", MB_ICONINFORMATION);
      break;
    }
    case IDM_CREAR_FICHA:
    {
      dialogo_ficha_tecnica(hwnd);
      break;
    }

    case IDM_CFG_COLORES:
    {
      COLORREF ccref[16];
      COLORREF csel=0x000000;
      CHOOSECOLOR c; ZeroMemory(&c, sizeof(CHOOSECOLOR)); c.lStructSize = sizeof(CHOOSECOLOR);
      c.Flags=CC_FULLOPEN|CC_RGBINIT; c.hwndOwner=hwnd; c.lpCustColors=ccref; c.rgbResult=csel;
      if(ChooseColor(&c))
        Bckgnd = Scalar(GetBValue(c.rgbResult), GetGValue(c.rgbResult), GetRValue(c.rgbResult) );
      break;
    }

    }
	break;

  case WM_TIMER:
    switch (wParam)
    {
      case ID_T30:
        if(!b_cache_valida)
        {
          renderizarDiagrama(region); //actualizamos el contenido de la matriz
          b_cache_valida = true;
        }
        procesar_queue_cntrl();
        db::checar_input_db();
        break;

      case ID_T1000:
        if(sync::b_sync_cambio)
        {
          for(sync* p : sync::set_modificados)
            p->actualizar_db();
          sync::set_modificados.clear();
          sync::b_sync_cambio = false;
        }
        break;
    }
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


BOOL CALLBACK DialogoTextoProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  static objeto* pobj;

  switch(Message)
  {
    case WM_INITDIALOG:
      pobj = reinterpret_cast<objeto*>(lParam);
      SetDlgItemTextA(hwnd, IDT_NVONOMBRE, pobj->nombre().c_str());
      {
        /*
        cout << "huh\n";
      HWND hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE |  ES_NUMBER,
        30,60,144,100, hwnd, (HMENU)IDT_HEDIT1, GetModuleHandle(NULL), NULL);*/
      }
      break;

    return TRUE;
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDOK:
          {
            char buf[128];
            GetDlgItemText(hwnd, IDT_NVONOMBRE, buf, 128);
            string s(buf);
            pobj->nombre(s);
            cout << s << '\t' << pobj->id() << pobj->nombre() << '\n';
          }

          EndDialog(hwnd, IDOK);
        break;

        case IDCANCEL:
          EndDialog(hwnd, IDCANCEL);
        break;

      }
    break;
    default:
        return FALSE;
  }
  return TRUE;
}

void crear_dialogo_objeto(objeto* pobj, ventana& padre)
{
  int ret = DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROPIEDADES_OBJETO), padre.get_hwnd(),
                           (DLGPROC)DialogoTextoProc, reinterpret_cast<LPARAM>(pobj));
  if(ret == IDOK)
  {
    //has algo?
  }

  else if(ret == IDCANCEL)
  {

  }

  else if(ret == -1)
    MessageBox(padre.get_hwnd(), "Dialog failed!", "Error", MB_OK | MB_ICONINFORMATION);

}

void alerta_cierre_programa(string msg)
{
  MessageBox(NULL, msg.c_str(), "Cerrando programa", MB_OK);
  exit(0);
}

void mensaje(ventana& padre, string msg, string titulo)
{
  MessageBox(padre.get_hwnd(), msg.c_str(), titulo.c_str(), MB_OK | MB_ICONINFORMATION);
}
