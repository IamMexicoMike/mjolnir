#include <iostream>
#include <cstring>
#include "socket_y_sesion.h"

#define DBG 1

void procesar_metadata(const char* paquete)
{
  #ifdef DBG
  cout << "tamanio del paq: " << strlen(paquete) << " bytes " << endl;
  #endif // DBG
}

void sesion::iniciar()
{
  hacer_lectura();
}

void sesion::hacer_lectura()
{
  auto si_mismo(shared_from_this());
  socket_.async_read_some(asio::buffer(data_, max_length),
    [this, si_mismo](std::error_code ec, std::size_t longitud)
  {
    if (!ec)
    {
      procesar_metadata(data_);
      /*procesar lectura y responder si es necesario*/
      hacer_lectura();
    }
    else
    {
      /*y no volvemos a leer, si no, se cicla*/
      cout << "ec=" << ec.value() << " sz=" << longitud << endl;
    }

  });
}

void sesion::hacer_escritura(std::string str)
{
  str_ = str;
  auto si_mismo(shared_from_this());
  asio::async_write(socket_, asio::buffer(str_.data(), str_.size()),
    [this, si_mismo](std::error_code ec, std::size_t /*length*/)
  {
    if (!ec)
    {
      /*procesar escritura exitosa*/
    }
  });
}

//ssssssssssssssssssssssssssssssssssssssssssssssssssss

void servidor::hacer_aceptacion()
{
  acceptor_.async_accept(socket_,
    [this](std::error_code ec)
  {
    if (!ec)
    {
      auto sp = std::make_shared<sesion>(std::move(socket_));
      sp->iniciar();
      //causa error cout << "conexion establecida por " << socket_.remote_endpoint() << endl;
    }

    hacer_aceptacion();
  });
}


