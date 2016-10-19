#include "peq_server_ftp.h"
#include <memory>

using namespace std;

void sesion_receptora::iniciar()
{
  hacer_lectura();
}

void sesion_receptora::hacer_lectura()
{
  try{
  auto si_mismo(shared_from_this());
  socket_.async_read_some(asio::buffer(data_, longitud_maxima),
    [this, si_mismo](std::error_code ec, std::size_t longitud)
  {
    if (!ec)
    {
      cout << longitud << " bytes leidos:\n";
      cout.write(data_, longitud);
      cout << "\n\n";
      hacer_lectura();
    }
    else
    {
      /*y no volvemos a leer, si no, se cicla*/
      cout << "Error sesión receptora: "<< ec.value() << ":" << ec.message()
        << longitud << " bytes leidos" << endl;
      if(ec.value() == 2)
      {
        std::cout.write(data_, longitud);
      }
    }

  });

  } catch (std::exception& e)
  {
    std::cerr << __PRETTY_FUNCTION__ << e.what() << "\n";
  }
}

void aceptor_sockets_ftp::hacer_aceptacion()
{
  acceptor_.async_accept(socket_,
    [this](std::error_code ec)
  {
    if (!ec)
    {
      //cout << "conexion establecida por " << socket_.remote_endpoint() << endl;
      auto sp = std::make_shared<sesion_receptora>(std::move(socket_));
      sp->iniciar();
    }
    else
    {
      cout << "Error aceptando: " << ec.value() << ":" << ec.message() << endl;
    }

    /*considera añadir aquí algún mecanismo que permita reiniciar la aceptacciónión de sockets si algo falla*/
    hacer_aceptacion();
  });
}
