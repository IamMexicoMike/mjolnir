#include "puerto_serial.h"

using namespace asio;

asio::io_service iosvc_serial;

void puerto_serial::leer()
{
  puerto_.async_read_some(asio::buffer((void*)buf_.data(), buf_.size()),
    [&](std::error_code ec, std::size_t length)
    {
      if(!ec)
      {
        cout.write(buf_.data(), length);
        //escribir_display(buf_.data(), length); //vamos a ver si no hay pedos de buffers temporales
        memset(buf_.data(), '\0', buf_.size() );
        leer();
      }

      else
        cout << ec.value() << " " << ec.message() << endl;

    });
}

void puerto_serial::iniciar_temporizador()
{
  temporizador_.expires_from_now(std::chrono::milliseconds(periodo_));
  temporizador_.async_wait([&](std::error_code ec)
  {
    if(!ec)
    {
      iniciar_temporizador();
    }
    else
    {
      cout << "ERROR TEMPORIZADOR: " << ec.value() << ec.message() << endl;
    }
  });
}

void puerto_serial::escribir(string& s) //es asíncrono pero no hacemos nada al terminar
{
  asio::async_write(puerto_, asio::buffer(s.c_str(), s.size()),
    [] (std::error_code ec, size_t len)
  {
    if(ec)
      cout << ec.value() << ec.message() << endl;
  });
}

struct dummy_loop
{
  dummy_loop():
    ios(iosvc_serial),
    temporizador_(iosvc_serial)
    {
      t_forever();
    }
  void t_forever()
{
  temporizador_.expires_from_now(std::chrono::milliseconds(10000));
  temporizador_.async_wait([&](std::error_code ec)
  {
    if(!ec)
    {
      t_forever();
    }
    else
    {
      cout << "Error dummy_loop: " << ec.value() << ec.message() << endl;
    }
  });
}
  io_service& ios;
  asio::steady_timer temporizador_;
};

void serial_main()
{
  try
  {
    iosvc_serial.run();
    std::cout << "Saliendo de ciclo de iosvc_serial\n";
  }
  catch (std::exception& e)
  {
    std::cerr << "Excepcion en hilo del puerto serial: " << e.what() << "\n";
  }
}
