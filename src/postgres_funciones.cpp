#include "postgres_funciones.h"

#include <iostream>

/*libpq es reentrante y thread-safe por default*/

using namespace std;

PGconn* conexion;//ATTN RAW PTR GLOBAL SIN COMMENTS

//static int miProcesadorEventos(PGEventId evtid, void* evtInfo, void* pasar /*??*/);

void conectar_db()
{
  conexion = PQconnectdb("dbname=mjolnir hostaddr=192.168.1.10 user=turambar"); //el archivo pg_hba.txt está en la instalación de postgres/data ATTN
  if(PQstatus(conexion) != CONNECTION_OK)
  {
    cerr << "Error al conectar a la base de datos: " << PQerrorMessage(conexion) << '\n';
    PQfinish(conexion);
    system("pause");//ATTN
    exit(-1);
  }
  cout << "Conectado a la base de datos\n";
}

void prueba_db()
{
  PGresult* res = PQexec(conexion, "SELECT * FROM public.User"); //se ejecuta una query
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

void entablar_escuchador_db()
{
  PGresult* res = PQexec(conexion, "LISTEN TBL2");
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    cerr << "Error entablando LISTEN: " << PQerrorMessage(conexion) << "\n";
  }

  PQclear(res);
}

void checar_input_db()
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



