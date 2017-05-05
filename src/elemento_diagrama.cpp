#include "elemento_diagrama.h"
#include <windows.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <string>
#include <algorithm>

using namespace std;
using namespace cv;

mutex mtx_objetos;
int objeto::sid = 1; //hasta donde sabes debe definirse fuera de la clase, y no en el header
const Point objeto::offset_puntos_clave_(3,3);

//void comparar_por_area(const un)

void ordenar_objetos() //debe ser llamada expl�citamente por el usuario para evitar "sorpresas"
{
  lock_guard<mutex> lck(mtx_objetos);
  if(ptr_highlight != nullptr)
    ptr_highlight->highlightear(false);
  if(ptr_seleccionado != nullptr)
    ptr_seleccionado->seleccionar(false);
  sort(objetos.begin(),objetos.end(),[&](const unique_ptr<objeto>& a, const unique_ptr<objeto>& b) {return a->area() < b->area();});
  ptr_seleccionado=ptr_highlight=nullptr;
  b_drag=false; //cuando hacias drag y suprimias terminabas con un dangling ptr
}

void destruir_objeto(int id)
{
  lock_guard<mutex> lck(mtx_objetos);
  auto itr = find_if(objetos.begin(), objetos.end(), [&](unique_ptr<objeto> const& obj)
    { return obj->id() == id; });
  if(itr != objetos.end())
  {
    objeto* ptr = (*itr).get();
    for(auto& o : objetos)
      o->avisar_objeto_destruido(ptr); //atento a los usos de ptr y de itr
    objetos.erase(itr); //itr solo es necesario para llamar a esta funci�n
    ptr_seleccionado=ptr_highlight=nullptr;
    string paq = "ro" + to_string(id);
    empujar_queue_saliente(paq);
  }
}

//-----------------------------------------------------------------------------------------------------------------

void objeto::dibujar_nombre(Mat& m) const
{
  static const int factor = 10;
  Point cc = transformar(centro_);
  Point pt = Point(cc.x - (nombre_.size()/2)*factor, cc.y + factor/2);
  putText(m, nombre(), pt, FONT_HERSHEY_PLAIN, tamanio_texto, COLOR_NEGRO, ancho_texto, CV_AA);
  if(b_subrayar_)
  {
    Point pt2 = Point(cc.x + (nombre_.size()/2)*factor, cc.y + factor/2);
    line(m, pt, pt2, COLOR_NEGRO);
  }
}

bool objeto::pertenece_a_punto_clave(const cv::Point pt) //pt es absoluto
{
  int tolerancia = transformar_escalar_inverso(tolerancia_); //tolerancia es adaptiva
  for(auto u : puntos_clave_)
  {
    bool pertenece = (pt.x > u->x-tolerancia
                      and pt.x < u->x+tolerancia
                      and pt.y > u->y-tolerancia
                      and pt.y < u->y+tolerancia); //es un cuadradito
    if(pertenece)
    {
      punto_arrastrado_ = u;
      resizeando_ = true;
      return true;
    }
  }
  resizeando_ = false;
  punto_arrastrado_ = nullptr;
  return false;
}

/*Reflexionar*/
void objeto::destruir()
{
  lock_guard<mutex> lck(mtx_objetos);
  string paq = "ro" + to_string(id());
  //avisamos a las lineas que ese objeto ya no existe
  for(auto& o : objetos)
    o->avisar_objeto_destruido(ptr_seleccionado); //avisamos que el objeto con esa direcci�n ser� destruido
  auto itr_seleccionado = find_if(objetos.begin(), objetos.end(), [&](unique_ptr<objeto> const& obj)
    { return obj.get() == ptr_seleccionado; });
  objetos.erase(itr_seleccionado);
  ptr_seleccionado=ptr_highlight=nullptr;
  empujar_queue_saliente(paq);
  b_drag=false; //cuando hacias drag y suprimias terminabas con un dangling ptr
}


void rectangulo::dibujarse(Mat& m)
{
  Point inicio, fin;
  inicio = transformar(inicio_); fin = transformar(fin_);
  rectangle(m, Rect(inicio, fin), color_, -2, CV_AA);
  rectangle(m, Rect(inicio, fin), COLOR_NEGRO, 1, CV_AA);

  if(b_seleccionado_)
  {
    rectangle(m, Rect(inicio, fin), COLOR_SELECCION, 2, CV_AA); //selecci�n
    for(auto p : puntos_clave_)
    {
      Point pc = transformar(*p);
      //cout  << "p: "<< *p << " p': " << pc << '\t';
      rectangle(m, Rect(pc-offset_puntos_clave_, pc+offset_puntos_clave_), COLOR_BLANCO, 1, CV_AA );
    }
  }

  if(b_highlighteado_)
    rectangle(m, Rect(inicio, fin), COLOR_HIGHLIGHT_, 1, CV_AA); //highlight
}

//se llama continuamente cuando haces drag, no s�lo una vez
void rectangulo::arrastrar(const Point pt) //no es realmente un punto, sino una diferencia entre dos puntos. Debe ser absoluto
{
  if(resizeando_)
  {
    (*punto_arrastrado_) += pt;
    recalcular_dimensiones();
    return;
  }
  fin_ += pt;
  inicio_ += pt;
  centro_ += pt;
}

bool rectangulo::pertenece_a_area(const Point pt) const //pt debe ser absoluto, obtenido mediante p = g(p')
{
    return !((pt.x > inicio_.x && pt.x > fin_.x) || (pt.x < inicio_.x && pt.x < fin_.x) ||
        (pt.y > inicio_.y && pt.y > fin_.y) || (pt.y < inicio_.y && pt.y < fin_.y));
}

void rectangulo::imprimir_datos() const
{
  cout << nombre() << " : " << id() << '\t';
  cout << inicio_ << ", " << fin_ << '\n';
  cout << "dir de inicio: " << &inicio_ << "\tdir de fin: " << &fin_ << '\n';
  cout << "dir punto c[0]: " << puntos_clave_.at(0) << "dir punto c[1]: " << puntos_clave_.at(1) << '\n';
}

void rectangulo::guardar(ofstream& ofs) const
{
  ofs << 'r' << id() << ':' << inicio_ << ',' << fin_ << "\r\n";
}

void circulo::dibujarse(Mat& m)
{
  Point centro = transformar(centro_);
  int radio = transformar_escalar(radio_);
  cv::circle(m, centro, radio, color_, -2, CV_AA);
  cv::circle(m, centro, radio, COLOR_NEGRO, 1, CV_AA);

  if(b_seleccionado_)
  {
    cv::circle(m, centro, radio, COLOR_SELECCION, 2, CV_AA);
    for(auto p : puntos_clave_)
    {
      Point pc = transformar(*p);
      //cout  << "p: "<< *p << " p': " << pc << '\t';
      rectangle(m, Rect(pc-offset_puntos_clave_, pc+offset_puntos_clave_), COLOR_BLANCO, 1, CV_AA );
    }
  }


  if(b_highlighteado_)
    cv::circle(m, centro, radio, COLOR_HIGHLIGHT_, 1, CV_AA);
}

void circulo::arrastrar(const Point pt)
{
  if(resizeando_)
  {
    (*punto_arrastrado_) += pt;
    recalcular_dimensiones();
    return;
  }
  fin_ += pt;
  inicio_ += pt;
  centro_ += pt;
}

bool circulo::pertenece_a_area(const Point pt) const //pt debe ser absoluto
{
  double x2 = double(pt.x - centro_.x)*double(pt.x - centro_.x);
  double y2 = double(pt.y - centro_.y)*double(pt.y - centro_.y);
  double r2 = double(radio_)*double(radio_);
  return (x2 + y2 < r2); //esto se hizo porque estaban overfloweando los valores de int
}

void circulo::imprimir_datos() const
{
  cout << nombre() << " : " << id() << '\t';
  cout << centro_ << ", " << radio_ << '\n';
}

void circulo::guardar(ofstream& ofs) const
{
  ofs << 'c' << id() << ':' << centro_ << ',' << radio_ << "\r\n";
}

void linea::dibujarse(Mat& m)
{
  Point tinicio=inicio_;
  Point tfin=fin_;

  if(ptrf_!=nullptr)
  {
    fin_ = ptrf_->centro();
    recalcular_dimensiones();
  }

  if(ptri_!=nullptr)
  {
    inicio_ = ptri_->centro();
    recalcular_dimensiones();
  }

  tinicio = transformar(inicio_);
  tfin = transformar(fin_);

  cv::line(m, tinicio, tfin, COLOR_BLANCO, 1, CV_AA);


  if(b_seleccionado_)
  {
    cv::line(m, tinicio, tfin, COLOR_SELECCION, 2, CV_AA);
    for(auto p : puntos_clave_)
    {
      Point pc = transformar(*p);
      //cout  << "p: "<< *p << " p': " << pc << '\t';
      rectangle(m, Rect(pc-offset_puntos_clave_, pc+offset_puntos_clave_), COLOR_BLANCO, 1, CV_AA );
    }
  }

  if(b_highlighteado_)
    cv::line(m, tinicio, tfin, COLOR_HIGHLIGHT_, 1, CV_AA);
}

void linea::arrastrar(const Point pt)
{
  if(resizeando_)
  {
    (*punto_arrastrado_) += pt;
    recalcular_dimensiones();
    return;
  }

  if(ptrf_==nullptr)
    fin_    +=pt;

  if(ptri_==nullptr)
    inicio_ +=pt;

  recalcular_dimensiones();
}

/**S�lo si el punto satisface la ecuaci�n de la l�nea determinamos si cae dentro de la regi�n acotada*/
bool linea::pertenece_a_area(const Point pt) const
{
  int y = efe_de_x(pt.x);
  int tolerancia = transformar_escalar_inverso(tolerancia_);
  if(pt.y < y-tolerancia || pt.y > y+tolerancia)
    return false;


  //el punto satisfizo la ecuaci�n, ahora vemos si cae en el rango acotado por los dos puntos
  else if(inicio_.x < fin_.x)
  {
    if(pt.x > inicio_.x and pt.x < fin_.x)
      return true;
  }

  else
  {
    if(pt.x > fin_.x and pt.x < inicio_.x)
      return true;
  }
  return false;
}

void linea::imprimir_datos() const
{
  cout << nombre() << " : " << id() << '\t';
  cout << inicio_ << ", " << fin_ << '\n';
  cout << "m=" << m << ", " << "b=" << b << '\n';
}

void linea::avisar_objeto_destruido(objeto* o)
{
  if(o==ptrf_)
    ptrf_ = nullptr;
  if(o==ptri_)
    ptri_ = nullptr;
}

void linea::guardar(ofstream& ofs) const
{
  ofs << 'l' << id() << ':' << inicio_ << ',' << fin_ << "\r\n";
}

void cuadrado_isometrico::dibujarse(cv::Mat& m)
{
  vector<Point> ps = puntos_desplazados();
  fillConvexPoly(m, ps.data(), ps.size(), color_);
  polylines(m, ps, true, COLOR_NEGRO, 1, CV_AA);
  if(b_seleccionado_)
  {
    polylines(m, ps, true, COLOR_SELECCION, 2, CV_AA);
    for(auto p : puntos_clave_)
    {
      Point pc = transformar(*p);
      //cout  << "p: "<< *p << " p': " << pc << '\t';
      rectangle(m, Rect(pc-offset_puntos_clave_, pc+offset_puntos_clave_), COLOR_BLANCO, 1, CV_AA );
    }
  }

  if(b_highlighteado_)
    polylines(m, ps, true, COLOR_HIGHLIGHT_, 1, CV_AA);
}

void cuadrado_isometrico::arrastrar(const cv::Point pt)
{
  if(resizeando_)
  {
    punto_arrastrado_->x += pt.x;
    recalcular_dimensiones();
    return;
  }
  inicio_+=pt;
  centro_+=pt;
  fin_+=pt;
  for(auto& v : vertices_)
    v+=pt;
}

bool cuadrado_isometrico::pertenece_a_area(const cv::Point pt) const
{
  if(pointPolygonTest(vertices_, pt, false) > 0)
    return true;
  return false;
}

void cuadrado_isometrico::imprimir_datos() const
{
  cout << nombre() << " : " << id() << '\t';
  cout << inicio_ << ", " << fin_ << '\n';
  cout << "vertices: ";
  for(auto& v : vertices_)
    cout << v << " ";
  cout << '\n';
}

void cuadrado_isometrico::guardar(ofstream& ofs) const
{
  ofs << 'i' << id() << ':' << inicio_ << ',' << fin_ << "\r\n";
}

objeto::~objeto()
{
    //cout << "soy el destructor de objetos[" << id_ << "]\n";
}

ostream& operator<<(ostream& os, objeto& o)
{
  return os << 'o' << o.id() << '\t' << typeid(o).name() << '\n';
}

void crear_relacion(objeto* o1, objeto* o2)
{
  lock_guard<mutex> lck(mtx_objetos);
  auto up = make_unique<linea>(o1->centro(),o2->centro()); //es solo para construir algo
  up->punto_inicial(o1);
  up->punto_final(o2);
  up->actualizar_pointers();
  objetos.emplace_back(move(up));
}

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
