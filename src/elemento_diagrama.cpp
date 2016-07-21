#include "elemento_diagrama.h"

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

void rectangulo::dibujarse(cv::Mat& m, cv::Point despl)
{
    cv::rectangle(m, cv::Rect(_inicio - despl, _fin - despl), cv::Scalar(150, 65, 150), 2, CV_AA);
    cv::rectangle(m, cv::Rect(cv::Point(_centro.x - despl.x - 4, _centro.y - despl.y - 4), cv::Size(8,8)),
                  cv::Scalar(150, 165, 250), 1, CV_AA);
    if(_b_seleccionado)
        cv::rectangle(m, cv::Rect(_inicio - despl, _fin - despl), cv::Scalar(150, 215, 50), 1, CV_AA);
}

/*Es crítico que esta función esté optimizada*/
/*Dos condiciones por dimensión para no pertenecer:
 Si pt es menor o mayor que tanto inicio como fin */
bool rectangulo::pertenece_a_area(cv::Point pt)
{
    return !((pt.x > _inicio.x && pt.x > _fin.x) || (pt.x < _inicio.x && pt.x < _fin.x) ||
        (pt.y > _inicio.y && pt.y > _fin.y) || (pt.y < _inicio.y && pt.y < _fin.y));
}
