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

extern int tamanio_texto;
extern int ancho_texto;

mutex mtx_objetos;
int objeto::sid = 1; //hasta donde sabes debe definirse fuera de la clase, y no en el header

//void comparar_por_area(const un)

void ordenar_objetos() //debe ser llamada explícitamente por el usuario para evitar "sorpresas"
{
  lock_guard<mutex> lck(mtx_objetos);
  if(itr_highlight>=objetos.begin() && itr_highlight!=objetos.end())
    (*itr_highlight)->highlightear(false);
  if(itr_seleccionado>=objetos.begin() && itr_seleccionado!=objetos.end())
    (*itr_seleccionado)->seleccionar(false);
  sort(objetos.begin(),objetos.end(),[&](const unique_ptr<objeto>& a, const unique_ptr<objeto>& b) {return a->area() < b->area();});
  itr_seleccionado=objetos.end();
  itr_highlight=objetos.end();
  b_drag=false; //cuando hacias drag y suprimias terminabas con un dangling ptr
}

void destruir_objeto(int id)
{
  lock_guard<mutex> lck(mtx_objetos);
  auto encontrarItrId = [&]() -> vector<unique_ptr<objeto>>::iterator
  {
    for(auto itr=objetos.begin(); itr!=objetos.end(); ++itr)
      if((*itr)->id() == id) //si el id existe retornamos el iterador que apunta a este
        return itr;
    return objetos.end();
  };
  auto itr = encontrarItrId();
  if(itr != objetos.end())
  {
    objetos.erase(itr);
    itr_seleccionado=objetos.end();
    itr_highlight=objetos.end();
    string paq = "ro" + to_string(id);
    empujar_queue_saliente(paq);
  }
}

void destruir_objeto_seleccionado()
{
  lock_guard<mutex> lck(mtx_objetos);
  if(itr_seleccionado>=objetos.begin() && itr_seleccionado != objetos.end())
  {
    //cout << "\tobjetos.begin()\tobjetos.end()\titr_highlight\titr_seleccion\n";
    //cout << '\t' << &*objetos.begin() << '\t' << &*objetos.end() << '\t' << &*itr_highlight << '\t' << &*itr_seleccionado << '\n';
    string paq = "ro" + to_string((*itr_seleccionado)->id());
    objetos.erase(itr_seleccionado);
    itr_seleccionado=objetos.end();
    itr_highlight=objetos.end();
    empujar_queue_saliente(paq);
    b_drag=false; //cuando hacias drag y suprimias terminabas con un dangling ptr
  }
}
//-----------------------------------------------------------------------------------------------------------------

void rectangulo::dibujarse(Mat& m) const
{
  Point iniciodespl, findespl;
  iniciodespl = transformar(inicio_); findespl = transformar(fin_);
  rectangle(m, Rect(iniciodespl, findespl), color_, 2, CV_AA);
  //rectangle(m, Rect(Point(centro_.x - despl.x - 4, centro_.y - despl.y - 4), Size(8,8)),Scalar(150, 165, 250), 1, CV_AA);
  //fillConvexPoly(matriz, ps.data(), ps.size(), z.color());

  if(b_seleccionado_)
    rectangle(m, Rect(iniciodespl, findespl), COLOR_SELECCION, 3, CV_AA); //selección

  if(b_highlighteado_)
    rectangle(m, Rect(iniciodespl, findespl), COLOR_HIGHLIGHT_, 1, CV_AA); //highlight
}

void objeto::dibujar_nombre(Mat& m) const
{
  Point entiende_esto = centro();
  Point pt = transformar(entiende_esto);
  putText(m, nombre(), pt, FONT_HERSHEY_PLAIN, tamanio_texto, Scalar(0,0,0), ancho_texto, CV_AA);
}

//se llama continuamente cuando haces drag, no sólo una vez
void rectangulo::arrastrar(const Point pt) //no es realmente un punto, sino una diferencia entre dos puntos. Debe ser absoluto
{
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
  cout << inicio_ << ", " << fin_;
}

/*bool objeto::es_esquina(const Point pt)
{
  return (pt.x <= fin_.x && pt.x > fin_.x-5 && pt.y <= fin_.y && pt.y > fin_.y-5); //XXXXXXXXXXXXXXXXXXXXX -5
}*/

/*void objeto::resizear(const Point pt)
{
  fin_ += pt;
  centro_ = Point(inicio_.x + (fin_.x - inicio_.x)/2, inicio_.y + (fin_.y - inicio_.y)/2);
}*/

objeto::~objeto()
{
    //cout << "soy el destructor de objetos[" << id_ << "]\n";
}

ostream& operator<<(ostream& os, objeto& o)
{
  auto pts = o.pts();
  return os << 'o' << o.id() << " " << pts.first.x << " " << pts.first.y << " " << pts.second.x << " " << pts.second.y;
}

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
