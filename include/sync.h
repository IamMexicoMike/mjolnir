#ifndef SYNC_H
#define SYNC_H

#include <map>
#include "elemento_diagrama.h"
#include "postgres_funciones.h"

/*siempre que se crea un objeto de la base de datos, su creación debe ser "mutexeada" por la base de datos
un mutex local no impediría que dos procesos cliente creasen un objeto sincronizado con el mismo id*/
/*debe ser movible y no copiable. Y su constructor movimiento y operador asignación movimiento deben estar
extremadamente bien diseñados*/
/*su destructor... debe borrar el objeto? Si salgo del programa invoco al destructor implícitamente. Tal vez
este comportamiento fuera adecuado en otras situaciones, pero en esta no. */
class sincronizado
{

};

/**Debes descomponer esta clase en dos:
1. La interfaz y clase abstracta sync, con sus respectivos miembros estáticos
2. La clase derivada sync_rect o sync::rect?*/
class sync : public rectangulo, public sincronizado
{
  public:
    //la dirección de los objetos no es inestable, pues unique_ptr la maneja y mover a unique_ptr no es problema
    static std::map<int, objeto*> objetos_sincronizados;
    static constexpr char const* nombreclase="rectangulo"; //PARADOXXXX jaja
    static constexpr char const* classname() { return sync::nombreclase; }
    static std::set<sync*> set_modificados;
    static std::atomic<bool> b_sync_cambio;
    void actualizar_db(); //esto deberia ser virtual propio de una interfaz sync
    sync(cv::Point inicio, cv::Point fin); //para construirse a partir del diagrama y ser insertado en la base de datos
    sync(int id, cv::Point inicio, cv::Point fin); //para construirse a partir de la base de datos
    virtual ~sync();
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
  std::string q = "DELETE FROM " + std::string(S::classname()) + " WHERE id = " + std::to_string(id);
  PGresult* res = PQexec(conexion, q.c_str());
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    PQclear(res);
    throw std::runtime_error("error eliminando objeto sincronizado");
  }
}

#endif // SYNC_H
