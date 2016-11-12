#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <mutex>

extern std::string ip_servidor;
extern std::string puerto_servidor;
extern std::string version;

std::mutex mtx_config;

using namespace std;

string cargar_valor(string arg)
{
  lock_guard<mutex> lck(mtx_config);
  ifstream ifs("config.txt");
  string simbolo, valor;
  while(ifs >> simbolo >> valor)
  {
    if(simbolo==arg)
    {
      return valor;
    }
  }
  return std::string();
}

void cargar_variables_configuracion()
{
  ip_servidor = cargar_valor("servidor");
  puerto_servidor = cargar_valor("puerto");
  version = cargar_valor("version");
}

/** El simbolo y su valor que se quieren sobreescribir en el archivo de configuración. Reescribe todo el archivo*/
void escribir_valor_configuracion(string simbolo, string valor)
{
  lock_guard<mutex> lck(mtx_config);
  fstream iofs("config.txt", ios::in);
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
