#include <windows.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <string>

#include "elemento_diagrama.h"

using namespace std;
using namespace cv;

mutex mtx_objetos;
int objeto::sid = 1; //hasta donde sabes debe definirse fuera de la clase, y no en el header

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
    string paq = "mo" + to_string(id);
    empujar_queue_saliente(paq);
  }
}

void destruir_objeto_seleccionado()
{
  if(itr_seleccionado>=objetos.begin() && itr_seleccionado != objetos.end())
  {
    //cout << "\tobjetos.begin()\tobjetos.end()\titr_highlight\titr_seleccion\n";
    //cout << '\t' << &*objetos.begin() << '\t' << &*objetos.end() << '\t' << &*itr_highlight << '\t' << &*itr_seleccionado << '\n';
    string paq = "mo" + to_string((*itr_seleccionado)->id());
    objetos.erase(itr_seleccionado);
    itr_seleccionado=objetos.end();
    itr_highlight=objetos.end();
    empujar_queue_saliente(paq);
  }
}
//-----------------------------------------------------------------------------------------------------------------

void rectangulo::dibujarse(Mat& m)
{
  Point iniciodespl, findespl;
  iniciodespl = transformar(inicio_); findespl = transformar(fin_);
  rectangle(m, Rect(iniciodespl, findespl), color_, 2, CV_AA);
  //rectangle(m, Rect(Point(centro_.x - despl.x - 4, centro_.y - despl.y - 4), Size(8,8)),Scalar(150, 165, 250), 1, CV_AA);
  //fillConvexPoly(matriz, ps.data(), ps.size(), z.color());

  if(b_seleccionado_)
    rectangle(m, Rect(iniciodespl, findespl), COLOR_SELECCION, 3, CV_AA); //selecci�n

  if(b_highlighteado_)
    rectangle(m, Rect(iniciodespl, findespl), COLOR_HIGHLIGHT_, 1, CV_AA); //highlight
}

//se llama continuamente cuando haces drag, no s�lo una vez
void rectangulo::arrastrar(const Point pt) //no es realmente un punto, sino una diferencia entre dos puntos. Debe ser absoluto
{
    fin_ += pt;
    inicio_ += pt;
    centro_ += pt;
}

bool rectangulo::pertenece_a_area(const Point pt) //pt debe ser absoluto, obtenido mediante p = g(p')
{
    return !((pt.x > inicio_.x && pt.x > fin_.x) || (pt.x < inicio_.x && pt.x < fin_.x) ||
        (pt.y > inicio_.y && pt.y > fin_.y) || (pt.y < inicio_.y && pt.y < fin_.y));
}

/*bool objeto::es_esquina(const Point pt)
{
  return (pt.x <= fin_.x && pt.x > fin_.x-5 && pt.y <= fin_.y && pt.y > fin_.y-5); //XXXXXXXXXXXXXXXXXXXXX -5
}*/

void objeto::imprimir_datos()
{
  cout << inicio_ << ", " << fin_;
}

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
