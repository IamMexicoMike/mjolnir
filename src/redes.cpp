#include "redes.h"

using namespace std;
using namespace asio;

io_service iosvc;
std::queue<std::string> queue_saliente;
std::mutex mtx_saliente;
std::mutex mtx_datos;

void clasificar_metadata(const char* paquete)
{
  cout << "tamanio del paq: " << strlen(paquete) << " bytes " << endl;
}


void empujar_queue_saliente(std::string s)
{
  std::lock_guard<std::mutex> lck(mtx_saliente);
  queue_saliente.emplace(s);
}

std::string extraer_queue_saliente()
{
  std::lock_guard<std::mutex> lck(mtx_saliente);
  std::string paq;
  if(!queue_saliente.empty())
  {
    paq = queue_saliente.back();
    queue_saliente.pop();
    return paq;
  }
  else
    return std::string{};
}

void cliente::timer_queue()
{
  asio::error_code ec;
  temporizador.expires_from_now(std::chrono::milliseconds(25), ec);
  temporizador.async_wait([this](std::error_code ec)
  {
    if(!ec)
    {
      /*checar alguna queue o enviar algun mensaje*/;
      string paq = extraer_queue_saliente();
      if(!paq.empty())
      {
        //escrbirla
        escribir(paq);
      }
    }

    else
      std::cout << "Error temporizador: " << ec.value() <<  ": " <<  ec.message() << std::endl;
    timer_queue();
  });
}

void cliente::conectar()
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

void cliente::leer()
{
  socket_.async_read_some(asio::buffer(rx_buf_, sz_buf),
    [this](std::error_code ec, std::size_t bytes_leidos)
  {
    if(!ec)
    {
      //hacer algo con la información recibida
      clasificar_metadata(rx_buf_);
      //std::cout.write(rx_buf_, bytes_leidos);
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

void cliente::escribir(std::string str)
{
  tx_buf_ = str;
  asio::async_write(socket_, asio::buffer(tx_buf_.data(), tx_buf_.size()),
    [this](std::error_code ec, std::size_t len)
  {
    if (!ec)
    {
      cout << "Se escribio " << tx_buf_ << " con " << len << " bytes" << endl;
      /*procesar escritura exitosa*/
    }
  });
}

void redes_main()
{
  try
  {
    short puerto = 1337;

    cliente c(iosvc, "127.0.0.1", puerto);

    iosvc.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

}
