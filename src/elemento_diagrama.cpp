#include "elemento_diagrama.h"
#include <iostream>

elemento_diagrama::elemento_diagrama()
{
    //ctor
}

elemento_diagrama::~elemento_diagrama()
{
    //dtor
}

void flecha::dibujarse(cv::Mat& m, cv::Point despl) //deberíamos inlinear?
{
    arrowedLine(m, cv::Point(_inicio.x - despl.x, _inicio.y - despl.y), //despl es desplazamientoOrigen
                cv::Point(_fin.x - despl.x, _fin.y - despl.y), cv::Scalar(205,155,25), 2, CV_AA);
}

int rectangulo::id_ = 1; //hasta donde sabes debe definirse fuera de la clase, y no en el header
const cv::Scalar rectangulo::_color_inicial(100, 65, 150);

void rectangulo::dibujarse(cv::Mat& m, cv::Point despl)
{
    cv::rectangle(m, cv::Rect(_inicio - despl, _fin - despl), _color, 2, CV_AA);
    cv::rectangle(m, cv::Rect(cv::Point(_centro.x - despl.x - 4, _centro.y - despl.y - 4), cv::Size(8,8)),
                  cv::Scalar(150, 165, 250), 1, CV_AA);
    if(_b_seleccionado)
        cv::rectangle(m, cv::Rect(_inicio - despl, _fin - despl), COLOR_SELECCION, 3, CV_AA); //selección

    if(_b_higlighteado)
        cv::rectangle(m, cv::Rect(_inicio - despl, _fin - despl), COLOR_HIGHLIGHT, 1, CV_AA); //highlight
}

/*Es crítico que esta función esté optimizada*/
/*Dos condiciones por dimensión para no pertenecer:
 Si pt es menor o mayor que tanto inicio como fin */
bool rectangulo::pertenece_a_area(const cv::Point pt)
{
    return !((pt.x > _inicio.x && pt.x > _fin.x) || (pt.x < _inicio.x && pt.x < _fin.x) ||
        (pt.y > _inicio.y && pt.y > _fin.y) || (pt.y < _inicio.y && pt.y < _fin.y));
}

void rectangulo::imprimir_datos()
{
    std::cout << _ancho << ", " << _altura << " | A=" <<  _altura*_ancho << std::endl;
}

//se llama continuamente cuando haces drag, no sólo una vez
void rectangulo::arrastrar(const cv::Point pt) //no es realmente un punto, sino una diferencia entre dos puntos
{
    _fin += pt;
    _inicio += pt;
}

void rectangulo::relocalizar(const cv::Point pt) //el punto tiene una referencia absoluta
{

}
