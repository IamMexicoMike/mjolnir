#ifndef CONFIGURACION_HPP
#define CONFIGURACION_HPP

#include <string>

struct configurador
{
  std::string cargar_valor(const std::string& arg);
  void cargar_variables_configuracion();
  void escribir_valor_configuracion(const std::string& simbolo, const std::string& valor);
  std::string get_puerto() const { return puerto_servidor; }
  std::string get_ip_servidor() const { return ip_servidor; }
  std::string get_version() const { return version; }

  std::string ip_servidor;
  std::string puerto_servidor;
  std::string version;
};





#endif // CONFIGURACION_HPP
