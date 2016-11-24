#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

class objeto;
class relacion;

namespace glb
{
    extern int llave_objeto_seleccionado; //recuerda que asignar algo a este id CREAR� el mapa indeseablemente. Deber�a estar encapsulada en una clase
    extern int llave_objeto_highlight; //se inicializan en .cpp. Deber�a estar encapsulada en una clase
    extern bool b_drag;
    extern bool b_resize;
    extern cv::Point ptInicioArrastre;
    extern cv::Point ptFinArrastre;
    extern std::map<int, relacion> relaciones;
    extern std::map<int, objeto> objetos;
}

//hmpgfgmg
struct ubicacion
{
  enum class Flags {Vacia, Objeto, SinCambios}; //no me convence
  static std::pair<int, ubicacion::Flags> determinar_propiedades_ubicacion(cv::Point);
};


#endif // UTILIDADES_HPP
