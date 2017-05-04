#include "postgres_funciones.h"
#include "sync.h"

#include <iostream>
#include <sstream>
#include <array>

/*libpq es reentrante y thread-safe por default*/

using namespace std;

PGconn* conexion;//ATTN RAW PTR GLOBAL SIN COMMENTS

//static int miProcesadorEventos(PGEventId evtid, void* evtInfo, void* pasar /*??*/);

void db::conectar_db()
{
  conexion = PQconnectdb("dbname=heredera host=201.139.98.214 port=5432 user=turambar"); //el archivo pg_hba.txt está en la instalación de postgres/data ATTN
  if(PQstatus(conexion) != CONNECTION_OK)
  {
    cerr << "Error al conectar a la base de datos: " << PQerrorMessage(conexion) << '\n';
    PQfinish(conexion);
    system("pause");//ATTN
    exit(-1);
  }
  cout << "Conectado a la base de datos\n";
}

void db::prueba_db()
{
  PGresult* res = PQexec(conexion, "SELECT * FROM Usuario"); //se ejecuta una query
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

void db::query_db(string query)
{
  PGresult* res = PQexec(conexion, query.c_str()); //se ejecuta una query
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

void db::entablar_escuchador_db()
{
  PGresult* res = PQexec(conexion, "LISTEN rectangulo");
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    cerr << "Error entablando LISTEN: " << PQerrorMessage(conexion) << "\n";
  }

  PQclear(res);
}

void db::checar_input_db()
{
  PGnotify* noti;
  PQconsumeInput(conexion);
  while( (noti = PQnotifies(conexion)) != NULL)
  {
    cout << "Notificacion de la DB recibida: " << noti->relname << "(pid=" << noti->be_pid<< ")\n";
    cout << "extras: " << noti->extra << "\t\n";// << noti->next()
    PQfreemem(noti);
  }
}

void db::construir_objetos_sincronizados()
{
  PGresult* res = PQexec(conexion, "SELECT * FROM rectangulo"); //se ejecuta una query
  for(int i=0; i<PQntuples(res); ++i)
  {
    int id = stoi(PQgetvalue(res, i, 0));
    array<int,4> coords;
    coords[0] = stoi(PQgetvalue(res, i, 1)); //solo por ser rectangulos
    coords[1] = stoi(PQgetvalue(res, i, 2)); //solo por ser rectangulos
    coords[2] = stoi(PQgetvalue(res, i, 3)); //solo por ser rectangulos
    coords[3] = stoi(PQgetvalue(res, i, 4)); //solo por ser rectangulos
    cv::Point p1(coords[0], coords[1]);
    cv::Point p2(coords[2], coords[3]);
    sync s(id, p1, p2);
    crear_objeto(s);
  }
  PQclear(res);
}


