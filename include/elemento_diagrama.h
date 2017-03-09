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
const cv::Scalar COLOR_AZUL_COMO_EL_MAR_AZUL(255,100,100);

enum class Objetos
{
  Rectangulo,
  Circulo,
  Zona,
  Linea,
  Cuadrado_Isometrico,
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
  void highlightear(bool val=true){b_highlighteado_ = val;} //highlighteamos para efecto visual
  void seleccionar(bool val=true){b_seleccionado_ = val;} //seleccionamos para un efecto más permanente
  bool operator<(const objeto& o2) const {return (this->area() < o2.area());}
  void dibujar_nombre(cv::Mat&) const;
  void nombre(std::string nuevo_nombre) { nombre_ = nuevo_nombre; }

  virtual void dibujarse(cv::Mat& m)=0;
  virtual void arrastrar(const cv::Point pt)=0;
  virtual bool pertenece_a_area(const cv::Point pt) const=0;
  bool pertenece_a_punto_clave(const cv::Point pt);
  //virtual bool es_esquina(const cv::Point pt);
  //void resizear(const cv::Point pt);
  virtual void imprimir_datos() const=0; //debug
  virtual void avisar_objeto_destruido(objeto* o) { }
  virtual void actualizar_pointers() { puntos_clave_.push_back(&inicio_); puntos_clave_.push_back(&fin_); }
  virtual void recalcular_dimensiones() {}
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
  std::vector<cv::Point*> puntos_clave_;
  static const cv::Point offset_puntos_clave_;
  bool resizeando_{false};
  cv::Point* punto_arrastrado_{nullptr};
  const int tolerancia_ = 8; //valor encontrado experimentalmente, es para seleccionar línea y puntos clave
  bool acepta_drops_{true};
  bool es_dropeable_{false};
  std::vector<objeto*> objetos_cliente_;
};

class rectangulo : public objeto
{
public:
  rectangulo(cv::Point inicio, cv::Point fin)
  {
    inicio_ = inicio, fin_ = fin;
    recalcular_dimensiones();
    nombre_ = "Rectangulo";
  }
   std::pair<cv::Point, cv::Point> pts() const {return std::pair<cv::Point, cv::Point>(inicio_, fin_);} //absolutos

  virtual void dibujarse(cv::Mat&) override;
  virtual void arrastrar(const cv::Point pt) override;
  virtual bool pertenece_a_area(const cv::Point) const override;
  virtual void imprimir_datos() const override;
  virtual void recalcular_dimensiones() {
    centro_ = cv::Point(inicio_.x + (fin_.x - inicio_.x)/2, inicio_.y + (fin_.y - inicio_.y)/2);
    area_ = std::abs((fin_.x - inicio_.x)*(fin_.y - inicio_.y)); //base por altura}
  };
};

class circulo : public objeto
{
public:
  circulo(cv::Point centro, int radio)
  {
    centro_ = centro;
    radio_ = std::abs(radio);
    inicio_ = fin_ = cv::Point(inicio_.x + radio_, inicio_.y);
    area_ = CV_PI*radio_*radio_;
    color_ = cv::Scalar(200,65,100);
    nombre_ = "Circulo";
  }
  virtual void dibujarse(cv::Mat&) override;
  virtual void arrastrar(const cv::Point pt) override;
  virtual bool pertenece_a_area(const cv::Point) const override;
  virtual void imprimir_datos() const override;

protected:
  int radio_;
};

class linea : public objeto
{
public:
  linea(cv::Point p1, cv::Point p2, bool relaciona=false)
  {
    inicio_ = p1; fin_ = p2;
    area_ = 0xffffffff;
    color_ = cv::Scalar(255,255,255);
    nombre_ = "";
    recalcular_dimensiones();
    acepta_drops_ = false;
    es_dropeable_ = true;
  }
  virtual void dibujarse(cv::Mat&) override;
  virtual void arrastrar(const cv::Point pt) override;
  virtual bool pertenece_a_area(const cv::Point) const override;
  virtual void imprimir_datos() const override;
  virtual void avisar_objeto_destruido(objeto* o) override;

  virtual void punto_inicial(objeto* p) { ptri_=p; }
  virtual void punto_final(objeto* p) { ptrf_=p; }
  virtual void recalcular_dimensiones() override {
    centro_ = (inicio_ + fin_)/2;
    m=(float)(fin_.y - inicio_.y)/(float)(fin_.x-inicio_.x);
    b = fin_.y - m*fin_.x;
  }

private:
  int efe_de_x(int x) const { return m*x + b; }
  int b;
  float m;
  objeto* ptrf_{nullptr};
  objeto* ptri_{nullptr};
};

class cuadrado_isometrico : public objeto
{
public:
   cuadrado_isometrico(cv::Point inicio, cv::Point fin)
  {
    cout << inicio << '\t' << fin << '\n';
    inicio_ = inicio;
    fin_ = cv::Point(fin.x, inicio_.y);
    nombre_ = "Relacion";
    for(int i=0; i<4; ++i)
      vertices_.push_back(cv::Point() );
    color_ = COLOR_AZUL_COMO_EL_MAR_AZUL;
    recalcular_dimensiones();
  }
   std::pair<cv::Point, cv::Point> pts() const {return std::pair<cv::Point, cv::Point>(inicio_, fin_);} //absolutos

  virtual void dibujarse(cv::Mat&) override;
  virtual void arrastrar(const cv::Point pt) override;
  virtual bool pertenece_a_area(const cv::Point) const override;
  virtual void imprimir_datos() const override;
  virtual void recalcular_dimensiones() override {
    centro_ = cv::Point((inicio_.x + fin_.x)/2, inicio_.y);
    l_ = calcular_l(inicio_, fin_);
    area_ = std::abs((fin_.x - inicio_.x)*l_/2); //base por altura
    calcular_vertices();
  }
private:
  float calcular_l(cv::Point ini, cv::Point fin) { return std::abs( float(fin.x-ini.x)/std::sqrt(3) ); };
  void calcular_vertices() { //vaya cuanta verbosidad
    vertices_[0] = inicio_;
    vertices_[1] = cv::Point((inicio_.x + fin_.x)/2,inicio_.y-l_/2);
    vertices_[2] = fin_;
    vertices_[3] = cv::Point((inicio_.x + fin_.x)/2,inicio_.y+l_/2);
  };
  std::vector<cv::Point> puntos_desplazados() const
  {
    std::vector<cv::Point> poff; //p'
    for(auto p : vertices_)
      poff.emplace_back(transformar(p));
    return poff;
  }
  float l_;
  std::vector<cv::Point> vertices_;
  //std::array<cv::Point,4> vertices_; //std::array no es un argumento aceptable para pointPolygonTest...
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
  std::unique_ptr<T> po = std::make_unique<T>(t);
  po->actualizar_pointers(); //los pointers a los miembros apuntaban a la stack si los actualizabas en el ctor
  objetos.emplace_back(std::move(po));
  itr_highlight=objetos.end();
  itr_seleccionado=objetos.end();
  //cout << paq << '\n';
  //empujar_queue_saliente(paq); //dentro de una función lockeada llamas a otra que usa locks. aguas
}

void crear_relacion(objeto* o1, objeto* o2);

#endif // ELEMENTO_DIAGRAMA_H
