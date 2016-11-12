#include <fstream>
#include <chrono>
#include <thread>
#include <atomic>
#include <asio.hpp>

#include <iostream>

using namespace std;

extern asio::io_service iosvc;
extern string extraer_queue_cntrl();
extern void empujar_queue_saliente(string);
const string CODIGO_ABORTAR = "xas343wraASrqwr36"; //definido DOS veces. que gcc no pinches mame.

void reboot()
{
  using namespace std::chrono_literals;
  empujar_queue_saliente(CODIGO_ABORTAR);
  this_thread::sleep_for(1s);
  system("start rebooter.bat");
  exit(0);
}

void procesar_queue_cntrl()
{
  string comando = extraer_queue_cntrl();
  if(comando.empty())
    return;
  if (comando == "reboot")
    reboot();
}
