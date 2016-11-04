#ifndef PEQ_SERVER_FTP_H
#define PEQ_SERVER_FTP_H

#include "asio.hpp"
#include <iostream>

using namespace std;
using asio::ip::tcp;

extern asio::io_service iosvc;

class sesion_receptora
  : public enable_shared_from_this<sesion_receptora>
{
public:
  sesion_receptora(tcp::socket socket)
    : socket_(std::move(socket))
  {
    cout << "Sesion receptora: " << socket_.remote_endpoint().address().to_string() << ":"
      << socket_.remote_endpoint().port() << '\n';
  }
  void iniciar();

private:
  void hacer_lectura();
  tcp::socket socket_;
  enum {longitud_maxima = 4096};
  char data_[longitud_maxima];
  string buffer_total_;
  string filename_;
};

class aceptor_sockets_ftp
{
public:
  aceptor_sockets_ftp(short puerto)
    : acceptor_(iosvc, tcp::endpoint(tcp::v4(), puerto)),
      socket_(iosvc)
  {
    hacer_aceptacion();
  }

private:
  void hacer_aceptacion();
  tcp::acceptor acceptor_;
  tcp::socket socket_;
};
#endif // PEQ_SERVER_FTP_H
