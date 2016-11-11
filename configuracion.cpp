#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

extern std::string ip_servidor;
extern short puerto_servidor;
extern unsigned int version;

using namespace std;

string cargar_valor(string);

void cargar_variables_configuracion()
{
  ip_servidor = cargar_valor("servidor");
  puerto_servidor = (short)stoi(cargar_valor("puerto"));
  version = stoi(cargar_valor("version"));
}

string cargar_valor(string arg)
{
  ifstream ifs("config.txt");
  string linea, valor;
  while(ifs >> linea >> valor)
  {
    if(linea==arg)
    {
      return valor;
    }
  }
  return std::string();
}
