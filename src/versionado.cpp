#include <fstream>
//#include <chrono>
#include <thread>
#include <redes.h>

#include <iostream>

using namespace std;

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
