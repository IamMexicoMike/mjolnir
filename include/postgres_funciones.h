#ifndef POSTGRES_FUNCIONES_H
#define POSTGRES_FUNCIONES_H

#include "pg/libpq-fe.h"
#include <string>

extern PGconn* conexion;

namespace db
{
  void conectar_db();
  void prueba_db();
  void entablar_escuchador_db();
  void checar_input_db();
  void query_db(std::string query);
  void construir_objetos_sincronizados();
}


#endif // POSTGRES_FUNCIONES_H
