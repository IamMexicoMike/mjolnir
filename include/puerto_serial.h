#ifndef PUERTO_SERIAL_H
#define PUERTO_SERIAL_H

#include "elemento_diagrama.h"
#include "redes.h"

class puerto_serial : public rectangulo
{
public:
  puerto_serial(cv::Point inicio, cv::Point fin, asio::io_service& ios, std::string puerto, unsigned int baudios) :
    rectangulo(inicio, fin),
    iosvc_(ios),
    puerto_(iosvc_, puerto),
    temporizador_(ios)
  {
    nombre_ = "PS"+to_string(id_);
    puerto_.set_option(asio::serial_port_base::baud_rate(baudios));
    std::cout << "!!!!!\n";
    leer();
    iniciar_temporizador(); //para verificar estado del queue de mensajes
  }
  static std::string puerto_temporal_; //esta solución no es elegante, no me encanta. El causante fue la incapacidad de obtener
  static int baudios_temporales_; //valores de retorno a partir de un callback de windows

protected:
  void leer();
  void iniciar_temporizador();
  void escribir(string& s);
  void cerrar() { b_cerrar_ps=true; }
  void salir_limpiamente() { puerto_.close(); }

private:
  asio::io_service& iosvc_;
  asio::serial_port puerto_;
  vector<char> buf_;
  string buffer_saliente_;
  int periodo_=25;
  asio::steady_timer temporizador_;

  //constexpr static unsigned int PERIODO=1;
  bool b_cerrar_ps=false;

};

bool crear_dialogo_serial(vector<string>*);

#endif // PUERTO_SERIAL_H
