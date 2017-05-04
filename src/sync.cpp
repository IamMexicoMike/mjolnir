#include "sync.h"

using namespace std;
using namespace cv;

set<sync*> sync::set_modificados;
atomic<bool> sync::b_sync_cambio{false};

string to_string(Point& p)
{
  return string('(' + to_string(p.x) + ',' + to_string(p.y) + ')');
}

string descomponer_punto(Point& p)
{
  return string(to_string(p.x) + ',' + to_string(p.y) );
}

sync::sync(Point inicio, Point fin):
  rectangulo(inicio,fin)
{
  string q = "INSERT INTO rectangulo (p1x, p1y, p2x, p2y) VALUES ("
    + descomponer_punto(inicio) + ',' + descomponer_punto(fin) + ") RETURNING id";
  PGresult* res = PQexec(conexion, q.c_str());
  if (PQresultStatus(res) != PGRES_TUPLES_OK)
  {
    PQclear(res);
    throw runtime_error("error construyendo objeto compartido");
  }
  id_ = stoi(PQgetvalue(res, 0, 0));
  PQclear(res);
}

sync::sync(int id, Point inicio, Point fin):
  rectangulo(inicio,fin)
{
  id_ = id;
}

void sync::arrastrar(const Point pt) //no es realmente un punto, sino una diferencia entre dos puntos. Debe ser absoluto
{
  sync::b_sync_cambio=true; //ya levantaste una flag de que hubo un cambio en los sincronizados, ahora notifia que fuiste tu
  sync::set_modificados.insert(this);
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

void sync::actualizar_db()
{
  string q = "UPDATE rectangulo SET p1x=" + to_string(inicio_.x) + ",p1y=" + to_string(inicio_.y)
    + ",p2x=" + to_string(fin_.x) + ",p2y=" + to_string(fin_.y) + " WHERE id=" + to_string(id_); //usar un hashcode?
  PGresult* res = PQexec(conexion, q.c_str());
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    PQclear(res);
    throw "error actualizando objeto" + to_string(id_);
  }
  PQclear(res);
}

sync::~sync()
{
  //dtor
}

void sync::destruir()
{
  eliminar_de_la_db<sync>(id_);
  lock_guard<mutex> lck(mtx_objetos);
  string paq = "ro" + to_string(id_);
  //avisamos a las lineas que ese objeto ya no existe
  for(auto& o : objetos)
    o->avisar_objeto_destruido((*itr_seleccionado).get()); //get retorna el ptr al cual protege el unique_ptr
  objetos.erase(itr_seleccionado);
  itr_seleccionado=objetos.end();
  itr_highlight=objetos.end();
  empujar_queue_saliente(paq);
  b_drag=false; //cuando hacias drag y suprimias terminabas con un dangling ptr
}
