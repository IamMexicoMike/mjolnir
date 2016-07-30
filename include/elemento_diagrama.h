#ifndef ELEMENTO_DIAGRAMA_H
#define ELEMENTO_DIAGRAMA_H

#include <vector>
#include <opencv2/opencv.hpp>

const cv::Scalar COLOR_FLECHA_DIBUJANDO(105, 205, 25);
const cv::Scalar COLOR_RECT_DIBUJANDO(150, 65, 150);
const cv::Scalar COLOR_HIGHLIGHT(150, 215, 50);
const cv::Scalar COLOR_SELECCION(50, 255, 25);

/*clase base para todas las formas (rectángulo, círculo, etc.). Trabájala, falta hacer uso de ella*/
class elemento_diagrama
{
public:
    elemento_diagrama();
    virtual ~elemento_diagrama();

protected:
    //virtual void dibujarse(cv::Mat&, cv::Point despl); //descoméntalo cuando lo implementes

private:
};


class flecha
{
public:
    flecha(cv::Point inicio, cv::Point fin):
        _inicio(inicio), _fin(fin),
        _centro(_inicio.x + (_fin.x - _inicio.x)/2, _inicio.y + (_fin.y - _inicio.y)/2),
        _b_seleccionado(false)
        {}

    void dibujarse(cv::Mat& m, cv::Point despl);
    void seleccionar(bool val){_b_seleccionado = val;}

private:
    cv::Point _inicio;
    cv::Point _fin;
    cv::Point _centro;
    bool _b_seleccionado;
    cv::Scalar _color;
};

//claramente estas tres clases son idénticas, con calma diseña una mejor interface.
//la idea es que ambas hereden de elemento_diagrama

class rectangulo
{
public:
    rectangulo(cv::Point inicio, cv::Point fin):
        _inicio(inicio), _fin(fin),
        _centro(_inicio.x + (_fin.x - _inicio.x)/2, _inicio.y + (_fin.y - _inicio.y)/2),
        _ancho(_fin.x - _inicio.y), _altura(_fin.y - _inicio.y),
        _b_seleccionado(false), _b_higlighteado(false),
        _color(_color_inicial)
        {}

    void dibujarse(cv::Mat& m, cv::Point despl);
    bool pertenece_a_area(const cv::Point pt);
    void seleccionar(bool val=true){_b_seleccionado = val;} //seleccionamos para un efecto más permanente
    void highlightear(bool val=true){_b_higlighteado = val;} //highlighteamos para efecto visual
    static int consecutivo(){return id_++;} /**otorga un entero consecutivo a la siguiente instancia de rectángulo*/
    void imprimir_datos(); //debug
    void relocalizar(const cv::Point pt); //es abrupto
    void arrastrar(const cv::Point pt); //es para drag

private:
    cv::Point _inicio;
    cv::Point _fin;
    cv::Point _centro;
    int _ancho, _altura;
    bool _b_seleccionado;
    bool _b_higlighteado;   //se usa para rectangulo::dibujarse
    cv::Scalar _color;

    static int id_;
    static const cv::Scalar _color_inicial;
};

#endif // ELEMENTO_DIAGRAMA_H
