#ifndef PUERTO_SERIAL_H
#define PUERTO_SERIAL_H

#include <asio.hpp>
#include "elemento_diagrama.h"

extern asio::io_service iosvc_serial;

class puerto_serial : rectangulo
{
public:
  puerto_serial(cv::Point inicio, cv::Point fin, std::string puerto, unsigned int baudios) :
    rectangulo(inicio, fin),
    iosvc_(iosvc_serial),
    puerto_(iosvc_, puerto),
    periodo_{25},
    temporizador_(iosvc_)
  {
    nombre_ = "PS"+to_string(id_);
    puerto_.set_option(asio::serial_port_base::baud_rate(baudios));
    leer();
    iniciar_temporizador(); //para verificar estado del queue de mensajes
  }
  virtual ~puerto_serial();

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
  asio::steady_timer temporizador_;

  //constexpr static unsigned int PERIODO=1;
  bool b_cerrar_ps=false;
  int periodo_;
};

void serial_main();

#endif // PUERTO_SERIAL_H
