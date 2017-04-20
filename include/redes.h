#ifndef REDES_H
#define REDES_H

#include <queue>
#include <mutex>

#include "asio.hpp"
#include "asio/steady_timer.hpp"

using namespace asio::ip;

extern asio::io_service iosvc;

extern std::queue<std::string> queue_saliente;
extern std::queue<std::string> queue_cntrl;
extern std::mutex mtx_saliente;
extern std::mutex mtx_cntrl;
extern const std::string CODIGO_ABORTAR;

/*Manejan reboots*/
void empujar_queue_cntrl(std::string s);
std::string extraer_queue_cntrl();
void procesar_queue_cntrl();

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
  cliente(asio::io_service& io_service, std::string huesped, std::string puerto) :
    iosvc_(io_service),
    socket_(io_service),
    temporizador(io_service)
  {
    tcp::resolver resolvedor(io_service);
    tcp::resolver::query query(huesped, puerto); //puedes meter dns aqui
    tcp::resolver::iterator iter = resolvedor.resolve(query);
    endpoint_ = iter->endpoint();
    conectar();
    timer_queue(); //podemos hacerlo como un sistema operativo
  }

private:
  void conectar();
  void leer();
  void procesar_lectura();
  void escribir(std::string str);
  void timer_queue();

  asio::io_service& iosvc_;
  tcp::socket socket_;
  tcp::endpoint endpoint_;
  enum { sz_buf = 128 };
  char rx_buf_[sz_buf];
  std::string tx_buf_;
  asio::steady_timer temporizador;
};

void redes_main();


#endif // REDES_H
