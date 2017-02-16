#ifndef CONFIGURACION_HPP
#define CONFIGURACION_HPP

#include <string>

extern string ip_servidor;
extern string puerto_servidor;
extern string version;

string cargar_valor(const string& arg);
void cargar_variables_configuracion();
void escribir_valor_configuracion(const string& simbolo, const string& valor);




#endif // CONFIGURACION_HPP
