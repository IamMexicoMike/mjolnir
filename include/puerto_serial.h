#ifndef PUERTO_SERIAL_H
#define PUERTO_SERIAL_H

#include "elemento_diagrama.h"
#include "redes.h"

class puerto_serial;

bool crear_dialogo_serial(std::vector<std::string>*);
void crear_dialogo_puerto(puerto_serial* pobj);

class puerto_serial : public rectangulo
{
public:
  puerto_serial(cv::Point inicio, cv::Point fin, asio::io_service& ios, std::string puerto, unsigned int baudios) :
    rectangulo(inicio, fin),
    iosvc_(ios),
    puerto_(iosvc_, puerto),
    temporizador_(ios),
    nombre_puerto_(puerto),
    baudios_{baudios}
  {
    nombre_ = "PS"+std::to_string(id_);
    puerto_.set_option(asio::serial_port_base::baud_rate(baudios));
    //puerto_.set_option(asio::serial_port_base:: );
    //cout << "Opciones del puerto: \n";
    //asio::serial_port_base::parity paridad;
    //cout << "Paridad: " << puerto_.get_option(paridad) << '\n';
    leer();
    iniciar_temporizador(); //para verificar estado del queue de mensajes
  }
  static std::string puerto_temporal_; //esta solución no es elegante, no me encanta. El causante fue la incapacidad de obtener
  static int baudios_temporales_; //valores de retorno a partir de un callback de windows
  virtual void dialogo_objeto() override { crear_dialogo_puerto(this); }

  void leer();
  void iniciar_temporizador();
  void escribir(std::string& s);
  void cerrar() { b_cerrar_ps=true; }
  void salir_limpiamente() { puerto_.close(); }
  std::string nombre_puerto() const { return nombre_puerto_; }
  unsigned int baudios() const { return baudios_; }

private:
  asio::io_service& iosvc_;
  asio::serial_port puerto_;
  std::array<char,1024> rx_buf_;
  std::string buffer_saliente_;
  int periodo_=2500;
  asio::steady_timer temporizador_;
  std::string nombre_puerto_;
  int baudios_;

  //constexpr static unsigned int PERIODO=1;
  bool b_cerrar_ps=false;

};

#endif // PUERTO_SERIAL_H
