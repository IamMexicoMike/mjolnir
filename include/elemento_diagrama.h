#ifndef ELEMENTO_DIAGRAMA_H
#define ELEMENTO_DIAGRAMA_H

#include "redes.h"
#include "mjolnir.hpp"

#include <typeinfo>

void ordenar_objetos();
void destruir_objeto(int id);
void destruir_objeto_seleccionado();

extern std::mutex mtx_objetos;

const cv::Scalar COLOR_FLECHA_DIBUJANDO(105, 205, 25);
const cv::Scalar COLOR_RECT_DIBUJANDO(150, 65, 150);
const cv::Scalar COLOR_SELECCION(50, 255, 25);
const cv::Scalar COLOR_HIGHLIGHT_(150, 215, 50);
const cv::Scalar COLOR_NEGRO(0,0,0);

enum class Objetos
{
  Rectangulo,
  Circulo,
  Zona,
  Linea,
};

class objeto
{
public:
  objeto():
    id_ (sid++), //primero asignamos, luego incrementamos
    b_seleccionado_(false), b_highlighteado_(false),
    color_(cv::Scalar(100,65,150)) {}

  virtual ~objeto();

  cv::Point centro() const {return centro_;} //absoluto
  int id() const {return id_;}
  unsigned int area() const {return area_;}
  std::string nombre() const {return nombre_;}
  std::pair<cv::Point, cv::Point> pts() const {return std::pair<cv::Point, cv::Point>(inicio_, fin_);} //absolutos //REVISA
  void highlightear(bool val=true){b_highlighteado_ = val;} //highlighteamos para efecto visual
  void seleccionar(bool val=true){b_seleccionado_ = val;} //seleccionamos para un efecto más permanente
  bool operator<(const objeto& o2) const {return (this->area() < o2.area());}
  void dibujar_nombre(cv::Mat&) const;

  virtual void dibujarse(cv::Mat& m) const=0;
  virtual void arrastrar(const cv::Point pt)=0;
  virtual bool pertenece_a_area(const cv::Point pt) const=0;
  //virtual bool es_esquina(const cv::Point pt);
  //void resizear(const cv::Point pt);
  virtual void imprimir_datos() const=0; //debug
  static int sid;

protected:
  int id_;
  cv::Point inicio_;
  cv::Point fin_;
  cv::Point centro_;
  bool b_seleccionado_;
  bool b_highlighteado_;   //se usa para objeto::dibujarse
  bool cache_valida_;
  unsigned int area_;
  cv::Scalar color_;
  std::string nombre_;
  std::vector<cv::Point> puntos_clave_;
  static const cv::Point offset_puntos_clave_;
};

class rectangulo : public objeto
{
public:
  rectangulo(cv::Point inicio, cv::Point fin)
  {
    inicio_ = inicio, fin_ = fin;
    centro_ = cv::Point(inicio_.x + (fin_.x - inicio_.x)/2, inicio_.y + (fin_.y - inicio_.y)/2);
    area_ = std::abs((fin_.x - inicio_.x)*(fin.y - inicio_.y)); //base por altura
    nombre_ = "Rectangulo";
    puntos_clave_.emplace_back(inicio);
    puntos_clave_.emplace_back(fin);
  }
   std::pair<cv::Point, cv::Point> pts() const {return std::pair<cv::Point, cv::Point>(inicio_, fin_);} //absolutos

  virtual void dibujarse(cv::Mat&) const override;
  virtual void arrastrar(const cv::Point pt) override;
  virtual bool pertenece_a_area(const cv::Point) const override;
  virtual void imprimir_datos() const override;
};

class circulo : public objeto
{
public:
  circulo(cv::Point centro, int radio)
  {
    inicio_ = fin_ = cv::Point(0,0);
    centro_ = centro, radio_ = radio;
    area_ = CV_PI*radio*radio;
    color_ = cv::Scalar(200,65,100);
    nombre_ = "Circulo";
  }
  virtual void dibujarse(cv::Mat&) const override;
  virtual void arrastrar(const cv::Point pt) override;
  virtual bool pertenece_a_area(const cv::Point) const override;
  virtual void imprimir_datos() const override;

protected:
  int radio_;
};

class linea : public objeto
{
public:
  linea(cv::Point p1, cv::Point p2)
  {
    inicio_ = p1; fin_ = p2;
    area_ = 0xffffffff;
    color_ = cv::Scalar(255,255,255);
    nombre_ = "Linea";
  }
  virtual void dibujarse(cv::Mat&) const override;
  virtual void arrastrar(const cv::Point pt) override;
  virtual bool pertenece_a_area(const cv::Point) const override;
  virtual void imprimir_datos() const override;
};

/** Crea un unique_ptr del objeto y se lo pasa al vector de apuntadores a objetos del diagrama*/
template <typename T>
void crear_objeto(T& t)
{
  std::string nombre_tipo = typeid(t).name();
  std::lock_guard<std::mutex> lck(mtx_objetos);
  std::string paq = "objeto " + nombre_tipo + " creado, con id==" + std::to_string(t.id());
  if(itr_seleccionado >= objetos.begin() and itr_seleccionado < objetos.end())
    (*itr_seleccionado)->seleccionar(false);
  objetos.emplace_back(std::make_unique<T>(t));
  itr_highlight=objetos.end();
  itr_seleccionado=objetos.end();
  //cout << paq << '\n';
  //empujar_queue_saliente(paq); //dentro de una función lockeada llamas a otra que usa locks. aguas
}

#endif // ELEMENTO_DIAGRAMA_H
