#ifndef ELEMENTO_DIAGRAMA_H
#define ELEMENTO_DIAGRAMA_H

#include <vector>
#include <map>
#include <memory>

#include <opencv2/opencv.hpp>

#include "utilidades.hpp"


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
        inicio_(inicio), fin_(fin),
        centro_(inicio_.x + (fin_.x - inicio_.x)/2, inicio_.y + (fin_.y - inicio_.y)/2),
        b_seleccionado_(false)
        {}

    //parametrizar este constructor con contenedores de tipo arbitrario, no sólo rectángulo
    /**Construye una flecha a partir de las llaves de los rectángulos que conecta*/
    flecha(int llave_origen, int llave_destino, std::map<int, objeto>& contenedor);


    void dibujarse(cv::Mat& m, cv::Point despl);
    void seleccionar(bool val){b_seleccionado_ = val;}

private:
    cv::Point inicio_;
    cv::Point fin_;
    cv::Point centro_;
    bool b_seleccionado_;
    cv::Scalar _color;
};

//claramente estas tres clases son idénticas, con calma diseña una mejor interface.
//la idea es que ambas hereden de elemento_diagrama

class base
{
public:
  virtual void dibujar()=0;
  virtual cv::Point centro()=0;
protected:
  cv::Point centro_;
};

//tus objetos no conocen su propio id, lol. implementar
class objeto
{
public:
  objeto(cv::Point inicio, cv::Point fin):
    id_ (sid++), //primero asignamos, luego incrementamos
    inicio_(inicio), fin_(fin),
    centro_(inicio_.x + (fin_.x - inicio_.x)/2, inicio_.y + (fin_.y - inicio_.y)/2),
    b_seleccionado_(false), b_highlighteado_(false), b_esquina_(false),
    color_(cv::Scalar(100, 65, 150))
    {
      std::cout << "soy el constructor de objetos[" << id_ << "]\n";
    }

  virtual ~objeto(); //si el objeto es destruido avisa a sus relaciones y luego las destruye

  std::pair<cv::Point, cv::Point> pts() const {return std::pair<cv::Point, cv::Point>(inicio_, fin_);}
  cv::Point centro() const {return centro_;} /**añadiste const*/
  int id() const {return id_;}

  std::vector<int>& get_relaciones() {return relaciones_;}
  cv::Point anadir_relacion(int id_relacion){relaciones_.emplace_back(id_relacion); return centro_;};
  bool pertenece_a_area(const cv::Point pt);
  bool es_esquina(const cv::Point pt);

  void highlightear(bool val=true){b_highlighteado_ = val;} //highlighteamos para efecto visual
  void seleccionar(bool val=true){b_seleccionado_ = val;} //seleccionamos para un efecto más permanente
  virtual void dibujarse(cv::Mat& m, cv::Point despl);
  void arrastrar(const cv::Point pt);
  void resizear(const cv::Point pt);

  void imprimir_datos(); //debug

  friend flecha::flecha(int llave_origen, int llave_destino, std::map<int, objeto>& contenedor); //SOSPECHOSO


  /**notifica a todas las relaciones suscritas*/
  void notificar(Notificacion noti);

  /**recibe notificaciones de alguna relación*/
  void recibir_notificacion(int, Notificacion noti);

  static int sid;
private:
  int id_;
  cv::Point inicio_;
  cv::Point fin_;
  cv::Point centro_;
  bool b_seleccionado_;
  bool b_highlighteado_;   //se usa para objeto::dibujarse
  bool b_esquina_;
  cv::Scalar color_;
  std::vector<int> relaciones_; //las relaciones (conexiones) que este objeto tiene con otros
};

class cuenta : public objeto
{

};

/**relacion enlaza dos objetos*/
class relacion
{
public:
  relacion(int id_ob1, int id_ob2):
    id_(sid++),
    ids_({id_ob1, id_ob2}),
    pt1_(glb::objetos.at(id_ob1).anadir_relacion(id_)),
    pt2_(glb::objetos.at(id_ob2).anadir_relacion(id_))
  {
    std::cout << "soy el constructor de relacion[" << id_ << ']' << "y vinculo " << id_ob1 << ',' << id_ob2 << std::endl;
  }

  ~relacion()
  {
    std::cout << "soy el destructor de relacion[" << id_ << "]" << std::endl;
  }

  int id(){return id_;}
  void recibir_notificacion(int id, Notificacion noti); //el id del objeto que emitió la notificiación y el tipo de esta
  void notificar(int id, Notificacion noti); //necesita pensarse profundamente. Es como un mensajero. a quién? a los dos?
  void dibujarse(cv::Mat& m, cv::Point despl);

  std::pair<int,int> get_objetos(){return ids_;}

  static int sid;

private:
  int id_;
  std::pair<int,int> ids_; //un par de pares<id, punto>
  cv::Point pt1_, pt2_;
  bool b_seleccionado_;
  cv::Scalar _color;
};

void guardar_todo();
void cargar_todo();

#endif // ELEMENTO_DIAGRAMA_H
