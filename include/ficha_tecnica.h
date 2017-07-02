#ifndef FICHA_TECNICA_H
#define FICHA_TECNICA_H

#define WIN32_LEAN_AND_MEAN //se supone que hace muchos headers derivados no sean incluidos, y por ende acelerar compilación

#include <windows.h>

class ficha_tecnica
{
  public:
    ficha_tecnica();
    virtual ~ficha_tecnica();

  protected:

  private:
};

void dialogo_ficha_tecnica(HWND padre);

#endif // FICHA_TECNICA_H
