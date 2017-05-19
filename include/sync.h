#ifndef SYNC_H
#define SYNC_H

#include <map>
#include "elemento_diagrama.h"
#include "postgres_funciones.h"

/*siempre que se crea un objeto de la base de datos, su creaci�n debe ser "mutexeada" por la base de datos
un mutex local no impedir�a que dos procesos cliente creasen un objeto sincronizado con el mismo id*/
/*debe ser movible y no copiable. Y su constructor movimiento y operador asignaci�n movimiento deben estar
extremadamente bien dise�ados*/
/*su destructor... debe borrar el objeto? Si salgo del programa invoco al destructor impl�citamente. Tal vez
este comportamiento fuera adecuado en otras situaciones, pero en esta no. */
class sync
{
public:
  //la direcci�n de los objetos no es inestable, pues unique_ptr la maneja y mover a unique_ptr no es problema
  /*cuando un objeto es creado se pasa la direccion que maneja el unique_ptr, y cuando
    sea borrado se asigna la direccion a nullptr*/
  static std::map<std::pair<std::string,int>, sync*> objetos_sincronizados; /**La llave es un par string int (nombre de la tabla, id)*/
  static std::set<sync*> set_modificados;
  static std::atomic<bool> b_sync_cambio;

  virtual void actualizar_db()=0; //transmite los cambios efectuados en el cliente a la db
  virtual void actualizarse()=0; //busca en la db sus propios cambios y se actualiza en el cliente
};

/**Debes descomponer esta clase en dos:
1. La interfaz y clase abstracta sync, con sus respectivos miembros est�ticos
2. La clase derivada sync_rect o sync::rect?*/
/**Un must:
   Cuando llegue una notificaci�n de la base de datos afectando a un objeto, el objeto adecuado debe modificarse.
   Para no volver este programa un infierno en vida, debes definir alg�n template que sea capaz de tomar esa expresi�n
   y convertirla a la modificaci�n de uno de sus objetos.
   Objetivo: Dise�ar una interfaz que permita a las clases heredar de ella y al hacerlo, cualquier creaci�n o edici�n
   de una instancia de esta clase se ver� reflejada en la base de datos. Del mismo modo, dicha instancia ser� modificada
   o eliminada si la base de datos as� lo exige. En pocas palabras, un ORM.*/
class sync_rect : public rectangulo, public sync
{
  public:
    static const std::string nombreclase;
    virtual void actualizar_db() override;
    sync_rect(cv::Point inicio, cv::Point fin); //para construirse a partir del diagrama y ser insertado en la base de datos
    sync_rect(int id, cv::Point inicio, cv::Point fin); //para construirse a partir de la base de datos

    virtual void actualizarse() override;
    virtual void destruir() override;

  protected:
    virtual void arrastrar(const cv::Point pt) override;
  private:
};

/**Esta plantilla pinta bien: el unico requisito es que la clase sincronizada tenga el mismo nombre que en la
   base de datos. */
template<typename S>
void eliminar_de_la_db(int id)
{
  std::string q = "DELETE FROM " + S::nombreclase + " WHERE id = " + std::to_string(id);
  PGresult* res = PQexec(conexion, q.c_str());
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    PQclear(res);
    throw std::runtime_error("error eliminando objeto sincronizado");
  }
}

template<typename T>
T* crear_sincronizado(T& t)
{
  std::string nombre_tipo = typeid(t).name();
  std::lock_guard<std::mutex> lck(mtx_objetos);
  std::string paq = "objeto syncronizado " + nombre_tipo + " creado, con id==" + std::to_string(t.id());
  if(ptr_seleccionado != nullptr)
    ptr_seleccionado->seleccionar(false);
  std::unique_ptr<T> po = std::make_unique<T>(t);
  T* ptr = po.get();
  po->actualizar_pointers(); //los pointers a los miembros apuntaban a la stack si los actualizabas en el ctor
  std::pair<std::string,int> tabla_y_id(T::nombreclase, po->id() );
  sync::objetos_sincronizados[tabla_y_id]=ptr;
  objetos.emplace_back(std::move(po));
  ptr_highlight=ptr_seleccionado=nullptr;
  return ptr;
}

#endif // SYNC_H
