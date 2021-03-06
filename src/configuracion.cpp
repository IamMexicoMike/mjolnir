#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <mutex>
#include <configuracion.hpp>

extern void alerta_cierre_programa(std::string msg);

using namespace std;

mutex mtx_config;
configurador cfg;

string configurador::cargar_valor(const string& arg)
{
  lock_guard<mutex> lck(mtx_config);
  ifstream ifs("config.txt");
  if(!ifs.is_open())
    alerta_cierre_programa("Error al abrir archivo de configuracion: \
                           config.txt\nEste archivo es necesario para \
                           determinar la direccion IP del servidor\n\n\
                           Verifique que el archivo existe");
  string simbolo, valor;
  while(ifs >> simbolo >> valor)
  {
    if(simbolo==arg)
    {
      return valor;
    }
  }
  return string();
}

void configurador::cargar_variables_configuracion()
{
  ip_servidor = cargar_valor("servidor");
  puerto_servidor = cargar_valor("puerto");
  version = cargar_valor("version");
}

/** El simbolo y su valor que se quieren sobreescribir en el archivo de configuración. Reescribe todo el archivo*/
void configurador::escribir_valor_configuracion(const string& simbolo, const string& valor)
{
  lock_guard<mutex> lck(mtx_config);
  fstream iofs("config.txt", ios::in);
  if(!iofs.is_open())
    alerta_cierre_programa("El archivo config.txt fue movido o eliminado en el transcurso de la ejecucion del programa\nEsto pudo hacerse por negligencia o malicia de un usuario, o por error del programa.\n\nSaliendo");
  vector<string> lineas;
  string s, v;
  while(iofs >> s >> v)
  {
    if(s == simbolo)
      v=valor;
    lineas.emplace_back(s + " " + v + '\n');
  }
  iofs.close();
  iofs.open("config.txt", ios::out);

  for(auto& l : lineas)
  {
    iofs << l;
  }
}
