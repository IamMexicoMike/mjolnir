#include "redes.h"

#include <future>
#include <fstream>

using namespace std;
using namespace asio;

io_service iosvc;
std::queue<std::string> queue_saliente;
std::mutex mtx_saliente;
std::queue<std::string> queue_cntrl;
std::mutex mtx_cntrl;

const string NOMBRE_APLICACION = "opencv_mjolnir.exe";
const string CODIGO_ABORTAR = "xas343wraASrqwr36"; //mal estilo. Dise�a algo mejor bruh
string ip_servidor;
short puerto_servidor; //inicializados a partir de config.txt
extern unsigned int version;

void generar_cliente_ftp(string);

void empujar_queue_cntrl(string s)
{
  std::lock_guard<mutex> lck(mtx_cntrl);
  queue_cntrl.emplace(s);
}

string extraer_queue_cntrl()
{
  lock_guard<mutex> lck(mtx_cntrl);
  if(!queue_cntrl.empty())
  {
    string cntrl = queue_cntrl.back();
    queue_cntrl.pop();
    return cntrl;
  }
  else
    return string();
}

void empujar_queue_saliente(string s)
{
  if(s.substr(0,3) == "ftp") //las peticiones ftp las mandamos por otro socket temporal, por eso no se a�aden al queue
  {
    string archivo = s.substr(4);
    std::thread t(generar_cliente_ftp, archivo);
    t.detach();
  }
  else
  {
    std::lock_guard<std::mutex> lck(mtx_saliente);
    queue_saliente.emplace(s);
  }
}

string extraer_queue_saliente()
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

void generar_cliente_ftp(string archivo)
{
  io_service servicio_ftp;
  asio::error_code ec;

  ip::tcp::resolver resolutor(servicio_ftp);
  ip::tcp::resolver::query consulta(ip_servidor, "1339");
  ip::tcp::resolver::iterator iterador_endpoint = resolutor.resolve(consulta);

  ip::tcp::socket socket_ftp(servicio_ftp);
  asio::connect(socket_ftp, iterador_endpoint,ec);

  string peticion = "ftp " + archivo;
  asio::write(socket_ftp, asio::buffer(peticion, peticion.size()), ec);
  if(ec)
  {
    cerr << "error cliente creando/enviando" << ec.value() << ec.message() << endl;
    return;
  }

  string buffer_total;
  buffer_total.reserve(4096*4096*4); // 4 MiB Mike
  for(;;)
  {
    char buf[4096];

    size_t len = socket_ftp.read_some(asio::buffer(buf), ec);
    buffer_total.append(buf, len);
    cout << "recibi " << len << " bytes\n";

    if(ec == asio::error::eof)
    {
      ofstream ofs("descargas/" + archivo, std::ios::binary);
      ofs.write(&buffer_total[0], buffer_total.size());
      cout << archivo << " recibido " << buffer_total.size() << " bytes\n";
      socket_ftp.close();
      if(archivo == NOMBRE_APLICACION)
        empujar_queue_cntrl("reboot");
      break;
    }
    else if(ec)
    {
      cerr << "error cliente recibiendo" << ec.value() << ec.message() << endl;
      break;
    }
  }
}

/**Funci�n miembro que verifica cada 25ms si hay algo que debamos enviar al servidor*/
void cliente::timer_queue()
{
  asio::error_code ec;
  temporizador.expires_from_now(std::chrono::milliseconds(25), ec);
  temporizador.async_wait([this](std::error_code ec)
  {
    if(!ec)
    {
      /*checamos el buz�n de mensajes de salida y lo enviamos*/;
      string paq = extraer_queue_saliente();
      if(!paq.empty())
      {
        if(paq == CODIGO_ABORTAR)
          socket_.close();
        else
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
      string sversion = "version " + to_string(version);
      escribir(sversion);
      leer();
    }
    else
    {
      std::cout << "Error conectando: " << ec.value() <<  ": " <<  ec.message() << '\n';
      /*Puede ser un error 10061: Equipo destino deneg� expresamente dicha conexi�n */
      if(ec.value() == 10061) //levantar dialogo de seleccion de ip y puertos?
        conectar();

      /*Error 10056: Se solicit� conexi�n en socket ya conectado*/
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
      procesar_lectura();
      leer();
    }
    else
    {
      /*Error 10054: Interrupci�n forzada por host remoto*/
      std::cout << "Error leyendo" << ec.value() <<  ": " << ec.message() << std::endl;
      if(ec.value() == 10054)
        conectar();
    }
  });
}

void cliente::procesar_lectura()
{
  string lectura = rx_buf_;
  if(lectura.substr(0,11) == "advertencia")
  {
    string advertencia = lectura.substr(12);
    if(advertencia == "actualizar")
    {
      empujar_queue_saliente("ftp opencv_mjolnir.exe");
      cout << "se te pidio actualizar\n" << endl;
    }
  }
  else
  {
    cout << rx_buf_ << '\n';
  }
  memset(rx_buf_, '\0', sz_buf);
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
    cliente tu_cliente(iosvc, ip_servidor, puerto_servidor); //"127.0.0.1" no s� si excluya conexiones externas

    iosvc.run();
    std::cout << "Saliendo de ciclo de iosvc\n";
  }
  catch (std::exception& e)
  {
    std::cerr << "Excepcion en redes_main: " << e.what() << "\n";
  }

}
