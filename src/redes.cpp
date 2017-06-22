#include "redes.h"
#include "control.h"
#include "dialogos.h"

#include <iostream>
#include <future>
#include <fstream>
#include <configuracion.hpp>

using namespace std;
using namespace asio;

extern void establecer_mensaje(string);
extern configurador cfg;

io_service iosvc;
queue<string> queue_saliente;
queue<string> queue_cntrl;
mutex mtx_saliente;
mutex mtx_cntrl;

#ifdef _WIN64
const string NOMBRE_APLICACION = "opencv_mjolnir.exe";
#else
const string NOMBRE_APLICACION = "opencv_mjolnir32.exe";
#endif // _WIN64

const string CODIGO_ABORTAR = "xas343wraASrqwr36"; //mal estilo. Diseña algo mejor bruh

void empujar_queue_cntrl(string s)
{
  lock_guard<mutex> lck(mtx_cntrl);
  queue_cntrl.emplace(s);
}

string extraer_queue_cntrl()
{
  lock_guard<mutex> lck(mtx_cntrl);
  if(!queue_cntrl.empty())
  {
    string cntrl = queue_cntrl.front();
    queue_cntrl.pop();
    return cntrl;
  }
  else
    return string();
}

void procesar_queue_cntrl()
{
  string comando = extraer_queue_cntrl();
  if(comando.empty())
    return;
  if (comando == "reboot")
    reboot();
  if(comando == "apagar")
    apagar();
}

void empujar_queue_saliente(string s)
{
  lock_guard<mutex> lck(mtx_saliente);
  queue_saliente.push(s);
}

string extraer_queue_saliente()
{
  std::lock_guard<std::mutex> lck(mtx_saliente);
  std::string paq;
  if(!queue_saliente.empty())
  {
    paq = queue_saliente.front();
    queue_saliente.pop();
    return paq;
  }
  else
    return std::string{};
}


/**Función miembro que verifica cada 25ms si hay algo que debamos enviar al servidor*/
void cliente::timer_queue()
{
  asio::error_code ec;
  temporizador.expires_from_now(std::chrono::milliseconds(25), ec);
  temporizador.async_wait([this](std::error_code ec)
  {
    if(!ec)
    {
      /*checamos el buzón de mensajes de salida y lo enviamos*/
      string paq;
      do {
        paq = extraer_queue_saliente();
        if(!paq.empty())
        {
          if(paq == CODIGO_ABORTAR)
          {
            cout << "paq==CODIGO_ABORTAR -> cerrando socket\n";
            socket_.close();
          }
          else
            escribir(paq);
        }
      } while(!paq.empty());
    }

    else
      std::cout << "Error temporizador: " << ec.value() <<  ": " <<  ec.message() << std::endl;

    timer_queue();
  });
}

void cliente::conectar()
{
  socket_.async_connect(endpoint_,
    [this](error_code ec)
  {
    if(!ec)
    {
      cout << "conectado a " << socket_.remote_endpoint().address().to_string()
          << ":" << socket_.remote_endpoint().port() << '\n';
      escribir("mike;ftw;");
      //escribir("version " + version);
      leer();
    }
    else
    {
      cout << "Error conectando: " << ec.value() <<  ": " <<  ec.message() << '\n';
      /*Puede ser un error 10061: Equipo destino denegó expresamente dicha conexión */
      if(ec.value() == 10061) //levantar dialogo de seleccion de ip y puertos? esto es un buen comentario
      {
        /*este codigo esta sucio, pero basicamente lidia con que el usuario introduzca valores para conectarse con el host*/

        /*
        auto h = dialogo_seleccion_host(); //retorna un par con {ip,puerto}
        if(h.first.empty() && h.second.empty())
        {
          gui::alerta("No se entabló una conexión con el servidor");
          return;
        }

        try
        {
          ip::tcp::resolver resolvedor(iosvc_);
          tcp::resolver::query query(h.first, h.second); //puedes meter dns aqui
          tcp::resolver::iterator iter = resolvedor.resolve(query);
          endpoint_ = iter->endpoint();
          conectar();
        } catch(...)
        {
          gui::alerta("La combinación de IP y Puerto produjo una excepción");
          conectar();
        }
        */
      }

      /*Error 10056: Se solicitó conexión en socket ya conectado*/
      else if(ec.value() == 10056)
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
      std::cout << "lectura exitosa...\n";
      procesar_lectura();
    }
    else
    {
      std::cout << "Error leyendo " << ec.value() <<  ": " << ec.message() << std::endl;
      if(ec.value() == 10054) /*Error 10054: Interrupción forzada por host remoto*/
      {
        Sleep(1000);
        conectar();
      }
    }
  });
}

void cliente::procesar_lectura()
{
  string lectura = rx_buf_;
  if(lectura.substr(0,7) == "version")
  {
    string version_serv = lectura.substr(8);
    if(version_serv != cfg.get_version())
    {
      cfg.version = version_serv;
      empujar_queue_saliente("ftp " + NOMBRE_APLICACION);
    }
  }
  else
  {
    cout << rx_buf_ << '\n';
  }
  memset(rx_buf_, '\0', sz_buf);
  leer();
}


void cliente::escribir(std::string str)
{
  tx_buf_ = str;
  asio::async_write(socket_, asio::buffer(tx_buf_.data(), tx_buf_.size()),
    [this](std::error_code ec, std::size_t len)
  {
    if (!ec)
    {
      //cout << "Se escribio " << tx_buf_ << " con " << len << " bytes" << endl;
      /*procesar escritura exitosa*/
    }
  });
}

void redes_main()
{
  try
  {
    cliente tu_cliente(iosvc, cfg.get_ip_servidor(), cfg.get_puerto()); //"127.0.0.1" no sé si excluya conexiones externas

    iosvc.run();
    std::cout << "Saliendo de ciclo de iosvc\n";
  }
  catch (std::exception& e)
  {
    std::cerr << "Excepcion en redes_main: " << e.what() << "\n";
  }

}
