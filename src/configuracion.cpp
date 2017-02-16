#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <mutex>

using namespace std;

string ip_servidor;
string puerto_servidor;
string version;

mutex mtx_config;

using namespace std;

string cargar_valor(const string& arg)
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
  return string();
}

void cargar_variables_configuracion()
{
  ip_servidor = cargar_valor("servidor");
  puerto_servidor = cargar_valor("puerto");
  version = cargar_valor("version");
}

/** El simbolo y su valor que se quieren sobreescribir en el archivo de configuración. Reescribe todo el archivo*/
void escribir_valor_configuracion(const string& simbolo, const string& valor)
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
