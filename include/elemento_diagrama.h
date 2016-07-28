#ifndef ELEMENTO_DIAGRAMA_H
#define ELEMENTO_DIAGRAMA_H

#include <vector>
#include <opencv2/opencv.hpp>

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
        _b_seleccionado(false), _b_higlighteado(false),
        _color(cv::Scalar(150, 65, 150))
        {}

    void dibujarse(cv::Mat& m, cv::Point despl);
    bool pertenece_a_area(cv::Point pt);
    void seleccionar(bool val=true){_b_seleccionado = val;} //seleccionamos para un efecto más permanente
    void highlightear(bool val=true){_b_higlighteado = val;} //highlighteamos para efecto visual
    static int consecutivo(){return id_++;} //provisional? otorga un entero consecutivo a la siguiente instancia de rectángulo
    static int id_;

private:
    cv::Point _inicio;
    cv::Point _fin;
    cv::Point _centro;
    int _altura, _ancho;
    bool _b_seleccionado;
    bool _b_higlighteado;   //se usa para rectangulo::dibujarse
    cv::Scalar _color;
};

#endif // ELEMENTO_DIAGRAMA_H
