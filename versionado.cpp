#include <fstream>
#include <chrono>
#include <thread>
#include <atomic>
#include <asio.hpp>

#include <iostream>

using namespace std;

extern asio::io_service iosvc;

unsigned int version;
string sversion;
atomic<bool> flag_reboot;

void reboot()
{
  cout << "rebooteando..\n";
  this_thread::sleep_for(chrono::milliseconds(100));

  system("move archivo_rx temp.exe"); //escribimos la version nueva

  iosvc.stop();

  system("start rebooter.bat");

  exit(0);
}
