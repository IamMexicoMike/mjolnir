#include "postgres_funciones.h"

#include <iostream>

/*libpq es reentrante y thread-safe por default*/

using namespace std;

PGconn* conexion;

//static int miProcesadorEventos(PGEventId evtid, void* evtInfo, void* pasar /*??*/);

void conectar_db()
{
  conexion = PQconnectdb("dbname = mjolnir");
  if(PQstatus(conexion) != CONNECTION_OK)
  {
    cerr << "Error al conectar a la base de datos: " << PQerrorMessage(conexion) << '\n';
    PQfinish(conexion);
    exit(-1);
  }
  cout << "Conectado a la base de datos\n";
}

void prueba_db()
{
  PGresult* res = PQexec(conexion, "SELECT * FROM public.User");
  auto campos = PQnfields(res);
  for(int i=0; i<campos; ++i)
    cout << PQfname(res, i) << '\t';
  cout << '\n';
  for(int i=0; i<PQntuples(res); ++i)
  {
    for(int j=0; j<campos; ++j)
      cout << PQgetvalue(res, i, j) << '\t';
    cout << '\n';
  }
  PQclear(res);
}
