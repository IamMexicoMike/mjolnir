#define IDD_SELECCIONAR_HOST 13000
#define IDD_QUERY 13001

/*esto es para evitar que windres arroje errores de compilación al toparse con <string> u otras cosas*/
#ifdef __cplusplus
#ifndef DIALOGOS_H
#define DIALOGOS_H

#include <string>
#include "ventana_principal.h"

namespace gui
{
  void alerta(std::string);
}

/**Retorna el IP y Puerto del host seleccionado*/
std::pair<std::string, std::string> dialogo_seleccion_host(ventana& v);
void dialogo_query();

#endif // DIALOGOS_H
#endif // __cplusplus
