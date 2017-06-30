#include "sync.h"

using namespace std;
using namespace cv;

string to_string(Point& p)
{
  return string('(' + to_string(p.x) + ',' + to_string(p.y) + ')');
}

string descomponer_punto(Point& p)
{
  return string(to_string(p.x) + ',' + to_string(p.y) );
}

sync_rect::sync_rect(Mjolnir* ptrm, Point inicio, Point fin):
  rectangulo(ptrm, inicio,fin)
{
  string q = "INSERT INTO " + string(nombreclase) + " (p1x, p1y, p2x, p2y) VALUES ("
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

sync_rect::sync_rect(Mjolnir* ptrm, int id, Point inicio, Point fin):
  rectangulo(ptrm, inicio,fin)
{
  id_ = id;
}


void sync_rect::arrastrar(const Point pt) //diferencia entre dos puntos. Debe ser absoluto
{
  sync::b_sync_cambio=true; //ya levantaste una flag de que hubo un cambio en los sincronizados
  sync::set_modificados.insert(this); //ahora notifica que este objeto fue modificado
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

void sync_rect::actualizar_db()
{
  string q = "UPDATE sync_rect SET p1x=" + to_string(inicio_.x) + ",p1y=" + to_string(inicio_.y)
    + ",p2x=" + to_string(fin_.x) + ",p2y=" + to_string(fin_.y) + " WHERE id=" + to_string(id_); //usar un hashcode?
  PGresult* res = PQexec(conexion, q.c_str());
  if (PQresultStatus(res) != PGRES_TUPLES_OK)
  {
    PQclear(res);
    throw runtime_error("error actualizando objeto" + to_string(id_));
  }
  PQclear(res);
}

void sync_rect::actualizarse()
{
  string query = "SELECT * FROM sync_rect WHERE id = " + to_string(id_);
  PGresult* res = PQexec(conexion, query.c_str() ); //se ejecuta una query
  for(int i=0; i<PQntuples(res); ++i) //este loop es un bug detecter? probablemente sospechoso
  {
    array<int,4> coords;
    inicio_.x = stoi(PQgetvalue(res, i, 1)); //solo por ser rectangulos
    inicio_.y = stoi(PQgetvalue(res, i, 2)); //solo por ser rectangulos
    fin_.x = stoi(PQgetvalue(res, i, 3)); //solo por ser rectangulos
    fin_.y = stoi(PQgetvalue(res, i, 4)); //solo por ser rectangulos
  }
  PQclear(res);
}

/*
void sync_rect::destruir()
{
  eliminar_de_la_db<sync_rect>(id_);
  lock_guard<mutex> lck(mtx_objetos);
  string paq = "ro" + to_string(id_);
  //avisamos a las lineas que ese objeto ya no existe
  for(auto& o : objetos)
    o->avisar_objeto_destruido(ptr_seleccionado);
  auto itr_seleccionado = find_if(objetos.begin(), objetos.end(), [&](unique_ptr<objeto> const& obj)
    { return obj.get() == ptr_seleccionado; }); //este lambda solo es necesario por que erase requiere un iterador
  objetos.erase(itr_seleccionado);
  ptr_seleccionado=ptr_highlight=nullptr;
  empujar_queue_saliente(paq);
  b_drag=false; //cuando hacias drag y suprimias terminabas con un dangling ptr

}
*/
