#ifndef POSTGRES_FUNCIONES_H
#define POSTGRES_FUNCIONES_H

#include "pg/libpq-fe.h"
#include <string>

extern PGconn* conexion;//ATTN RAW PTR GLOBAL SIN COMMENTS

namespace db
{
  void conectar_db();
  void prueba_db();
  void entablar_escuchador_db();
  void checar_input_db();
  void query_db(std::string query);
  void construir_objetos_sincronizados();
}

/*siempre que se crea un objeto de la base de datos, su creación debe ser "mutexeada" por la base de datos
un mutex local no impediría que dos procesos cliente creasen un objeto sincronizado con el mismo id*/
/*debe ser movible y no copiable. Y su constructor movimiento y operador asignación movimiento deben estar
extremadamente bien diseñados*/
/*su destructor... debe borrar el objeto? Si salgo del programa invoco al destructor implícitamente. Tal vez
este comportamiento fuera adecuado en otras situaciones, pero en esta no. */
class sincronizado
{

};

#endif // POSTGRES_FUNCIONES_H
