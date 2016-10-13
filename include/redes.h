#ifndef REDES_H
#define REDES_H

#define ASIO_HAS_STD_CHRONO

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <chrono>
#include <queue>
#include <mutex>

#include "asio.hpp"
#include "asio/steady_timer.hpp"

using namespace asio::ip;
using namespace std;
extern asio::io_service iosvc;
//using asio::ip::tcp;

extern std::queue<std::string> queue_saliente;
extern std::mutex mtx_saliente;
extern std::mutex mtx_datos;

/**Añade datos al contenedor de salida para que sean enviados por la red.
Debe ser usado por la interfaz gráfica*/
void empujar_queue_saliente(std::string s);

/**Extrae de la queue saliente datos a enviar por la red.
Usarlo dentro de la sesión TCP para enviar los datos*/
std::string extraer_queue_saliente();

void clasificar_metadata(const char* paquete);

class cliente
{
public:
  cliente(asio::io_service& io_service, std::string huesped, short puerto) :
    socket_(io_service),
    temporizador(io_service)
  {
    tcp::resolver resolvedor(io_service);
    tcp::resolver::query query("127.0.0.1", "1337"); //puedes meter dns aqui
    tcp::resolver::iterator iter = resolvedor.resolve(query);
    endpoint_ = iter->endpoint();
    conectar();
    timer_queue(); //podemos hacerlo como un sistema operativo
  }

private:
  void conectar();
  void leer();
  void escribir(std::string str);
  void timer_queue();

  tcp::socket socket_;
  tcp::endpoint endpoint_;
  enum { sz_buf = 128 };
  char rx_buf_[sz_buf];
  string tx_buf_;
  asio::steady_timer temporizador;
};

void redes_main();


#endif // REDES_H
