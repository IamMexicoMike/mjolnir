#ifndef CONFIGURACION_HPP
#define CONFIGURACION_HPP

#include <string>

extern std::string ip_servidor;
extern std::string puerto_servidor;
extern std::string version;

std::string cargar_valor(const std::string& arg);
void cargar_variables_configuracion();
void escribir_valor_configuracion(const std::string& simbolo, const std::string& valor);




#endif // CONFIGURACION_HPP
