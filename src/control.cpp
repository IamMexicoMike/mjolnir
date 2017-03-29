#include "control.h"

#include <redes.h>
#include <thread>

using namespace std;

void reboot()
{
  using namespace std::chrono_literals;
  empujar_queue_saliente(CODIGO_ABORTAR);
  this_thread::sleep_for(1s);
  system("start rebooter.bat");
  exit(0);
}

void apagar()
{
  using namespace std::chrono_literals;
  empujar_queue_saliente(CODIGO_ABORTAR);
  this_thread::sleep_for(1s);
  exit(0);
}
