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
        _inicio(inicio), _fin(fin) {}

    void dibujarse(cv::Mat& m, cv::Point despl);

private:
    cv::Point _inicio;
    cv::Point _fin;
};

//claramente estas tres clases son idénticas, con calma diseña una mejor interface.
//la idea es que ambas hereden de elemento_diagrama

class rectangulo
{
public:
    rectangulo(cv::Point inicio, cv::Point fin):
        _inicio(inicio), _fin(fin),
        _centro(_inicio.x + (_fin.x - _inicio.x)/2, _inicio.y + (_fin.y - _inicio.y)/2) {}

    void dibujarse(cv::Mat& m, cv::Point despl);

private:
    cv::Point _inicio;
    cv::Point _fin;
    cv::Point _centro;
};

#endif // ELEMENTO_DIAGRAMA_H
