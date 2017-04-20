#define IDD_SELECCIONAR_HOST 13000

/*esto es para evitar que windres arroje errores de compilación al toparse con <string> u otras cosas*/
#ifdef __cplusplus
#ifndef DIALOGOS_H
#define DIALOGOS_H

#include <string>

namespace gui
{
  void alerta(std::string);
}

/**Retorna el IP y Puerto del host seleccionado*/
std::pair<std::string, std::string> dialogo_seleccion_host();

#endif // DIALOGOS_H
#endif // __cplusplus
