#ifndef REDES_H
#define REDES_H

#define ASIO_HAS_STD_CHRONO

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <chrono>
#include "asio.hpp"
#include "asio/steady_timer.hpp"

using namespace asio::ip;
//using asio::ip::tcp;

class cliente
{
public:
  cliente(asio::io_service& io_service, std::string huesped, short puerto) :
    socket_(io_service),
    endpoint_(asio::ip::address::from_string(huesped), puerto),
    temporizador(io_service)
  {
    conectar();
    timer_queue(); //podemos hacerlo como un sistema operativo
  }

private:
  void conectar()
  {
    socket_.async_connect(endpoint_,
      [this](std::error_code ec)
      {
        if(!ec)
        {
          leer();
        }
        else
        {
          std::cout << "Error conectando: " << ec.value() <<  ": " <<  ec.message() << std::endl;
          /*Puede ser un error 10061: Equipo destino denegó expresamente dicha conexión */
          if(ec.value() == 10061)
            conectar();

          /*Error 10056: Se solicitó conexión en socket ya conectado*/
          if(ec.value() == 10056)
          {
            std::error_code ec_cerrar;
            socket_.close(ec_cerrar);
            if(!ec_cerrar)
              conectar();
            else
              std::cout << "Error cerrando y conectando: " << ec_cerrar.value() <<  ": " <<  ec_cerrar.message() << std::endl;
          }
          //...
        }
      });
  } //conectar

  void leer()
  {
    socket_.async_read_some(asio::buffer(rx_buf_, sz_buf),
      [this](std::error_code ec, std::size_t bytes_leidos)
      {
        if(!ec)
        {
          //hacer algo con la información recibida
          std::cout.write(rx_buf_, bytes_leidos);
          leer();
        }
        else
        {
          /*Error 10054: Interrupción forzada por host remoto*/
          std::cout << "Error leyendo" << ec.value() <<  ": " << ec.message() << std::endl;
          if(ec.value() == 10054)
            conectar();
        }
      });
  }

  void timer_queue()
  {
    asio::error_code ec;
    temporizador.expires_from_now(std::chrono::seconds(1), ec);
    temporizador.async_wait(
      [this](std::error_code ec)
      {
        if(!ec)
          std::cout << "Tic" << std::endl;
        else
          std::cout << "Error temporizador: " << ec.value() <<  ": " <<  ec.message() << std::endl;
        timer_queue();
      });
  }

  tcp::socket socket_;
  tcp::endpoint endpoint_;
  enum { sz_buf = 1024 };
  char rx_buf_[sz_buf];
  char tx_buf_[sz_buf];
  asio::steady_timer temporizador;
};

void redes_main();


#endif // REDES_H
