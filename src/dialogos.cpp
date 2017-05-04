#include "dialogos.h"
#include "windows.h"
#include <control.h>
#include "postgres_funciones.h"

using namespace std;

extern HWND hVentanaPrincipal;

namespace dialogos
{
  string ip;
  string puerto;
  string query_string;
}

bool CALLBACK callback_seleccionar_host(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  static HWND hEditIP;
  static HWND hEditPuerto;
  int id_edit_ip=42;
  int id_edit_puerto=43;

  switch(Message)
  {
    case WM_INITDIALOG:
      hEditIP = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE,
        15,65,130,20, hwnd, (HMENU)id_edit_ip, GetModuleHandle(NULL), NULL); //no están en la misma escala que en los .rc
      hEditPuerto = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE,
        15,90,130,20, hwnd, (HMENU)id_edit_puerto, GetModuleHandle(NULL), NULL);
      break;

    return TRUE;
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDOK:
          {
            char buf[128];
            GetDlgItemText(hwnd, id_edit_ip, buf, 128);
            string s(buf);
            dialogos::ip = s;

            char buf2[128];
            GetDlgItemText(hwnd, id_edit_puerto, buf2, 128);
            string s2(buf2);
            dialogos::puerto = s2;
          }

          EndDialog(hwnd, IDOK);
        break;

        case IDCANCEL:
          EndDialog(hwnd, IDCANCEL);
        break;

      }
    break;
    default:
        return false;
  }
  return true;
}

pair<string, string> dialogo_seleccion_host()
{
  int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECCIONAR_HOST), hVentanaPrincipal,
                      (DLGPROC)callback_seleccionar_host);
  if(ret == IDOK)
  {
    return pair<string,string>(dialogos::ip, dialogos::puerto);
  }

  else if(ret == IDCANCEL)
  {
    return pair<string,string>(string(),string());
  }

  else if(ret == -1)
    MessageBox(hVentanaPrincipal, "Error creando dialogo", "Error", MB_OK | MB_ICONINFORMATION);
}

bool CALLBACK callback_query(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  static HWND hEditIP;
  int id_query=42;

  switch(Message)
  {
    case WM_INITDIALOG:
      hEditIP = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE,
        15,25,650,20, hwnd, (HMENU)id_query, GetModuleHandle(NULL), NULL); //no están en la misma escala que en los .rc
      SetFocus(hEditIP);
      break;
    return true;

    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDOK:
          {
            char buf[128];
            GetDlgItemText(hwnd, id_query, buf, 128);
            string s(buf);
            dialogos::query_string = s;
          }
          EndDialog(hwnd, IDOK);
        break;

        case IDCANCEL:
          EndDialog(hwnd, IDCANCEL);
        break;
      }
    break;
    default:
        return false;
  }
  return true;
}

void dialogo_query()
{
  int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_QUERY), hVentanaPrincipal,
                      (DLGPROC)callback_query);
  if(ret == IDOK)
  {
    if(!dialogos::query_string.empty())
      db::query_db(dialogos::query_string);
  }

  else if(ret == -1)
    MessageBox(hVentanaPrincipal, "Error creando dialogo", "Error", MB_OK | MB_ICONINFORMATION);
}



void gui::alerta(string m)
{
  MessageBox(hVentanaPrincipal, m.c_str(), "Alerta!", MB_ICONEXCLAMATION | MB_OK | MB_TOPMOST);
}
