#include "redes.h"


void redes_main()
{
  try
  {
    asio::io_service io_service;

    short puerto = 1337;

    cliente c(io_service, "127.0.0.1", puerto);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

}
