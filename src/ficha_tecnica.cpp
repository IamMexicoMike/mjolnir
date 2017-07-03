#include "ficha_tecnica.h"
#include "ventana_principal.h"
//#include "mjolnir.hpp"
#include "../recurso.h"

#include <windows.h>
#include <windowsx.h>

using namespace std;

ficha_tecnica::ficha_tecnica()
{
  //ctor
}

ficha_tecnica::~ficha_tecnica()
{
  //dtor
}


BOOL CALLBACK DialogoFichaTecnicaProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  //static HWND hEditPeso;
  HWND cboAnchos;
  const vector<string> anchos_telares= {"25","34","40","60","68","69","74","84","88","92","100","150","214","220","233","240","280"};

  switch(Message)
  {
    case WM_INITDIALOG:
      cboAnchos = GetDlgItem(hwnd, IDCOMBO_FT_ANCHO);

      for(auto& s : anchos_telares)
      {
        SendMessage(cboAnchos, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(s.c_str()));
      }

      /* hEditPeso = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE |  ES_NUMBER,
        30,60,144,100, hwnd, (HMENU)IDT_HEDIT1, GetModuleHandle(NULL), NULL);*/
      break;

    return TRUE;

    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDOK:
          {
            char buf[8];
            GetDlgItemText(hwnd, IDCOMBO_FT_ANCHO, buf, 7);
            string ancho(buf);

            cout << "ancho: " << ancho;
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

void dialogo_ficha_tecnica(ventana& padre)
{
  int ret = DialogBox/*Param*/(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FICHA_TECNICA), padre.get_hwnd(),
                           (DLGPROC)DialogoFichaTecnicaProc/*, reinterpret_cast<LPARAM>(pobj)*/);
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
