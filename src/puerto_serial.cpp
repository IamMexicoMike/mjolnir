#include "puerto_serial.h"
#include "../recurso.h"

using namespace asio;

string puerto_serial::puerto_temporal_ = "";
int puerto_serial::baudios_temporales_=0; //plz borra esto cuando tengas una mejor solucion

void puerto_serial::leer()
{
  puerto_.async_read_some(asio::buffer((void*)buf_.data(), buf_.size()),
    [&](std::error_code ec, std::size_t length)
    {
      if(!ec)
      {
        cout.write(buf_.data(), length);
        //escribir_display(buf_.data(), length); //vamos a ver si no hay pedos de buffers temporales
        memset(buf_.data(), '\0', buf_.size() );
        leer();
      }

      else
        cout << ec.value() << " " << ec.message() << endl;

    });
}

void puerto_serial::iniciar_temporizador()
{
  temporizador_.expires_from_now(std::chrono::milliseconds(periodo_));
  temporizador_.async_wait([&](std::error_code ec)
  {
    if(!ec)
    {
      iniciar_temporizador();
    }
    else
    {
      cout << "ERROR TEMPORIZADOR: " << ec.value() << ec.message() << endl;
    }
  });
}

void puerto_serial::escribir(string& s) //es asíncrono pero no hacemos nada al terminar
{
  asio::async_write(puerto_, asio::buffer(s.c_str(), s.size()),
    [] (std::error_code ec, size_t len)
  {
    if(ec)
      cout << ec.value() << ec.message() << endl;
  });
}

extern HWND hVentanaPrincipal;

BOOL CALLBACK DialogoSerial(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  vector<string>* puertos_disponibles = reinterpret_cast<vector<string>*>(lParam);
  const vector<string> baudios= {"9600","19200","38400","57600","115200","230400"};

  switch(Message)
  {
    case WM_INITDIALOG:
    {
      auto resBaudios = GetDlgItem(hwnd, IDCOMBO_BAUDIOS);
      for(auto& s : baudios)
        SendMessage(resBaudios, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(s.c_str()));

      auto resPuertos = GetDlgItem(hwnd, IDCOMBO_PUERTOS_SERIALES_DISPONIBLES);
      for(auto& s : *puertos_disponibles)
        SendMessage(resPuertos, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(s.c_str()));
    }
      return TRUE;

    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDOK:
        {
          char buf[32];
          GetDlgItemText(hwnd, IDCOMBO_PUERTOS_SERIALES_DISPONIBLES, buf, 31);
          string puerto(buf);
          memset(buf, '\0', 32);
          GetDlgItemText(hwnd, IDCOMBO_BAUDIOS, buf, 31);
          string sbaudios(buf);
          if(puerto.empty() || sbaudios.empty())
          {
            MessageBox(hVentanaPrincipal, "Puerto o baudios vacio, reintenta", "Error", MB_OK | MB_ICONINFORMATION);
            break;
          }

          int baudios = stoi(sbaudios, nullptr);
          puerto_serial::puerto_temporal_ = puerto;
          puerto_serial::baudios_temporales_ = baudios;

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

bool crear_dialogo_serial(vector<string>* puertos_disponibles)
{
  int ret = DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROPIEDADES_SERIAL), hVentanaPrincipal,
                           (DLGPROC)DialogoSerial, reinterpret_cast<LPARAM>(puertos_disponibles));
 /*int ret = DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROPIEDADES_OBJETO), hVentanaPrincipal,
      (DLGPROC)DialogoTextoProc, 0);*/
  if(ret==-1)
  {
    MessageBox(hVentanaPrincipal, "Error al crear props de Puerto Serial", "Error", MB_OK | MB_ICONINFORMATION);
    return false;
  }
  return true;
}
