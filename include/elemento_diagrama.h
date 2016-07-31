#ifndef ELEMENTO_DIAGRAMA_H
#define ELEMENTO_DIAGRAMA_H

#include <vector>
#include <map>
#include <memory>

#include <opencv2/opencv.hpp>

const cv::Scalar COLOR_FLECHA_DIBUJANDO(105, 205, 25);
const cv::Scalar COLOR_RECT_DIBUJANDO(150, 65, 150);
const cv::Scalar COLOR_HIGHLIGHT(150, 215, 50);
const cv::Scalar COLOR_SELECCION(50, 255, 25);

class rectangulo; //fwd decl

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
    /**Construye una flecha a partir de dos puntos*/
    flecha(cv::Point inicio, cv::Point fin):
        _inicio(inicio), _fin(fin),
        _centro(_inicio.x + (_fin.x - _inicio.x)/2, _inicio.y + (_fin.y - _inicio.y)/2),
        _b_seleccionado(false)
        {}

    //parametrizar este constructor con contenedores de tipo arbitrario, no sólo rectángulo
    /**Construye una flecha a partir de las llaves de los rectángulos que conecta*/
    flecha(int llave_origen, int llave_destino, std::map<int, rectangulo>& contenedor);


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
    void relocalizar(const cv::Point pt); //es abrupto (no se usa todavía)
    void arrastrar(const cv::Point pt); //es para drag
    friend flecha::flecha(int llave_origen, int llave_destino, std::map<int, rectangulo>& contenedor);

    void notificar_relaciones(); //notifica a las relaciones suscritas de cambios importantes //implementar
    void anadir_relacion(const std::vector<flecha>::iterator it); //esta interface nos permite cambiar la representación de "varias cosas?" sin re-declararla

private:
    cv::Point _inicio;
    cv::Point _fin;
    cv::Point _centro;
    int _ancho, _altura;
    bool _b_seleccionado;
    bool _b_higlighteado;   //se usa para rectangulo::dibujarse
    cv::Scalar _color;

    //cada flecha o relación es cuidada por dos objetos. Cuando ambos se destruyen ésta también
    std::vector<int> _relaciones;

    static int id_;
    static const cv::Scalar _color_inicial;
};

#endif // ELEMENTO_DIAGRAMA_H
