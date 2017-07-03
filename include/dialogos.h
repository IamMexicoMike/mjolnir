#define IDD_SELECCIONAR_HOST 13000
#define IDD_QUERY 13001

/*esto es para evitar que windres arroje errores de compilación al toparse con <string> u otras cosas*/
#ifdef __cplusplus
#ifndef DIALOGOS_H
#define DIALOGOS_H

#define WIN32_LEAN_AND_MEAN

#include <string>
#include <windows.h>

class ventana;

namespace gui
{
  void alerta(std::string, HWND);
}

/**Retorna el IP y Puerto del host seleccionado*/
std::pair<std::string, std::string> dialogo_seleccion_host(ventana& v);
void dialogo_query(HWND padre);

#endif // DIALOGOS_H
#endif // __cplusplus
