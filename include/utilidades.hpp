#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

#include "elemento_diagrama.h"

namespace global
{
    extern int llave_rectangulo_seleccionado; //recuerda que asignar algo a este id CREARÁ el mapa indeseablemente
    extern int llave_rectangulo_highlight; //se inicializan en .cpp
    extern bool b_drag;
    extern cv::Point ptInicioArrastre;
    extern cv::Point ptFinArrastre;
}


class ubicacion
{
public:
    enum class Flags { Vacia, Objeto, SinCambios}; //no me convence
    static std::pair<int, Flags> determinar_propiedades_ubicacion(cv::Point, std::vector<flecha>&, std::map<int, rectangulo>&);
};


#endif // UTILIDADES_HPP
