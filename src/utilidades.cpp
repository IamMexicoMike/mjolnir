#include "utilidades.hpp"
#include "elemento_diagrama.h"

int glb::llave_objeto_highlight=-1;
int glb::llave_objeto_seleccionado=-1;
bool glb::b_drag=false;
bool glb::b_resize=false;

cv::Point glb::ptInicioArrastre(0,0);
cv::Point glb::ptFinArrastre(0,0);

std::map<int, relacion> glb::relaciones;
std::map<int, objeto> glb::objetos;

/**Debe determinar propiedades del punto en funci�n de la dimensi�n en la que est�.
 No debe determinar si debe dibujarse. Actualmente highlightea... y muestra x,y del mouse*/
std::pair<int, ubicacion::Flags> ubicacion::determinar_propiedades_ubicacion(cv::Point p)
{
  /*Este lambda podr�a generalizarse si recibiera como argumentos el tipo de operaci�n y la categor�a del contenedor. Nubloso*/
  auto encontrarIdHighlight = [&]() -> int
  {
    for(auto& ob : glb::objetos)
      if(ob.second.pertenece_a_area(p)) //si el punto cae dentro del �rea de un objeto...
        return ob.first;
    return -1;
  };

  int llave = encontrarIdHighlight(); //asignamos el id del rect�ngulo que cumpli� las condiciones, o -1.

  /*El caso m�s frecuente: no hay cambios en lo que se highlighteo*/
  if(llave == glb::llave_objeto_highlight) //no hay cambios, no renderizamos
    return std::pair<int, ubicacion::Flags>(llave, ubicacion::Flags::SinCambios);

  /*Segundo caso: Hab�a algo highlighteado pero ya no*/
  if(llave < 0)
  {
    glb::objetos.at(glb::llave_objeto_highlight).highlightear(false);//des-highlighteamos el anterior
    glb::llave_objeto_highlight = -1;
    return std::pair<int, ubicacion::Flags>(llave, ubicacion::Flags::Vacia);
  }

  /*Tercer caso: hay algo highlighteable*/
  glb::objetos.at(llave).highlightear(true); //highlighteamos al nuevo

  /*Si es que hab�a otro highlighteado antes lo deshighlighteamos*/
  if(glb::llave_objeto_highlight > 0)
    glb::objetos.at(glb::llave_objeto_highlight).highlightear(false);

  glb::llave_objeto_highlight = llave; //actualizamos la llave highlight

  return std::pair<int, ubicacion::Flags>(llave, ubicacion::Flags::Objeto);
}
