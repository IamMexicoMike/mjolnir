#ifndef ELEMENTO_DIAGRAMA_H
#define ELEMENTO_DIAGRAMA_H

#include <vector>
#include <map>
#include <memory>

#include <opencv2/opencv.hpp>

const cv::Scalar COLOR_FLECHA_DIBUJANDO(105, 205, 25);
const cv::Scalar COLOR_RECT_DIBUJANDO(150, 65, 150);
const cv::Scalar COLOR_SELECCION(50, 255, 25);
const cv::Scalar COLOR_HIGHLIGHT_(150, 215, 50);

enum class Notificacion{Movimiento, Destruccion /*añadir necesarias*/};

class objeto;

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
    flecha(int llave_origen, int llave_destino, std::map<int, objeto>& contenedor);


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

//tus objetos no conocen su propio id, lol. implementar
class objeto
{
public:
    objeto(cv::Point inicio, cv::Point fin):
        _id (id_++), //primero asignamos, luego incrementamos
        _inicio(inicio), _fin(fin),
        _centro(_inicio.x + (_fin.x - _inicio.x)/2, _inicio.y + (_fin.y - _inicio.y)/2),
        _ancho(_fin.x - _inicio.y), _altura(_fin.y - _inicio.y),
        _b_seleccionado(false), _b_highlighteado(false),
        _color(_color_inicial)
        {
          std::cout << "soy el constructor de objetos[" << _id << "]\n";
        }

    ~objeto(); //si el objeto es destruido avisa a sus relaciones y luego las destruye

    void dibujarse(cv::Mat& m, cv::Point despl);
    bool pertenece_a_area(const cv::Point pt);
    void seleccionar(bool val=true){_b_seleccionado = val;} //seleccionamos para un efecto más permanente
    void highlightear(bool val=true){_b_highlighteado = val;} //highlighteamos para efecto visual
    void imprimir_datos(); //debug
    std::vector<int>& get_relaciones() {return _relaciones;}
    void arrastrar(const cv::Point pt); //es para drag
    friend flecha::flecha(int llave_origen, int llave_destino, std::map<int, objeto>& contenedor); //SOSPECHOSO
    cv::Point get_centro() const {return _centro;} /**añadiste const*/
    int get_id() const {return _id;}

    cv::Point anadir_relacion(int id_relacion){_relaciones.push_back(id_relacion); return _centro;}; //demasiado específico
    void quitar_relacion(int id_relacion)
    {
        auto itr = find(objeto::_relaciones.begin(), objeto::_relaciones.end(), id_relacion);
        objeto::_relaciones.erase(itr);
    };
    /**notifica a todas las relaciones suscritas*/
    void notificar(Notificacion noti);
    /**notificar a alguna relacion*/
    void notificar(Notificacion noti, int id);
    /**recibe notificaciones de alguna relación*/
    void recibir_notificacion(int id, Notificacion noti);

    static int id_;

private:
    int _id;
    cv::Point _inicio;
    cv::Point _fin;
    cv::Point _centro;
    int _ancho, _altura;
    bool _b_seleccionado;
    bool _b_highlighteado;   //se usa para objeto::dibujarse
    cv::Scalar _color;

    std::vector<int> _relaciones; //las relaciones (conexiones) que este objeto tiene con otros

    static const cv::Scalar _color_inicial;
};

/*Tanto relación como objeto pueden notificarse uno a otro*/

/**relacion enlaza dos objetos*/
class relacion
{
public:
    relacion(int id_ob1, int id_ob2);
    ~relacion()
    {
      std::cout << "soy el destructor de relacion[" << _id << "]" << std::endl;
    }

    void recibir_notificacion(int id, Notificacion noti); //el id del objeto que emitió la notificiación y el tipo de esta
    void notificar(int id, Notificacion noti); //necesita pensarse profundamente. Es como un mensajero. a quién? a los dos?
    void dibujarse(cv::Mat& m, cv::Point despl);
    int get_id(){return _id;}
    std::pair<int,int> get_objetos(){return ids;}

    static int id_;

private:
    int _id;
    std::pair<int,int> ids; //un par de pares<id, punto>
    cv::Point pt1, pt2;
    bool _b_seleccionado;
    cv::Scalar _color;
};

#endif // ELEMENTO_DIAGRAMA_H
