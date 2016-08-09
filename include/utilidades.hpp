#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

#include "elemento_diagrama.h"

namespace global
{
    extern int llave_objeto_seleccionado; //recuerda que asignar algo a este id CREARÁ el mapa indeseablemente
    extern int llave_objeto_highlight; //se inicializan en .cpp
    extern bool b_drag;
    extern cv::Point ptInicioArrastre;
    extern cv::Point ptFinArrastre;
    extern std::map<int, relacion> relaciones;
    extern std::map<int, objeto> objetos;

    /*Esta es de las variables más importantes del diagrama. Todos los "widgets" necesitan conocer su valor
    * para ser dibujados. Ejemplo:
    * arrowedLine(matriz, Point(400-desplazamientoOrigen.x,400-desplazamientoOrigen.y),
    * Point(500-desplazamientoOrigen.x,100-desplazamientoOrigen.y), Scalar(205,155,25), 2, CV_AA);
    * Esta sintaxis genera un efecto visual de desplazamiento sobre la matriz. Realmente los objetos tienen una posición absoluta,
    * pero los redibujamos con una posición relativa diferente (a la matriz de despliegue (región))
    */
    extern cv::Point desplazamientoOrigen;
}

namespace header
{
    const cv::Point HEADER0(5,20);
    const cv::Point HEADER1(100,20);
}


class ubicacion
{
public:
    enum class Flags { Vacia, Objeto, SinCambios}; //no me convence
    static std::pair<int, Flags> determinar_propiedades_ubicacion(cv::Point, std::vector<flecha>&, std::map<int, objeto>&);
};


#endif // UTILIDADES_HPP
