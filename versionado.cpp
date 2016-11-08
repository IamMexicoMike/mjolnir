#include <fstream>
#include <asio.hpp>

using namespace std;

extern asio::io_service iosvc;
unsigned int version;
string sversion;

void reboot()
{
  system("move opencv_mjolnir ./mjol2.exe"); //pasamos la version vieja a un archivo temporal

  system("move archivo_rx ./opencv_mjolnir.exe"); //escribimos la version nueva

  iosvc.stop();

  system("del ./mjol2.exe"); //borramos la version vieja

  system("start opencv_mjolnir.exe");

  exit(0);
}
