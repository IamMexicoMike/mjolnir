#include <iostream>
#include <chrono> //para profiles
#include <ctime> //para profiles
#include <fstream>
#include <algorithm>
#include <bitset> //borrame si no debuggeas
#include <future>
#include <thread>

#include <opencv2/opencv.hpp>

#include "mjolnir.hpp"
#include "elemento_diagrama.h"
#include "utilidades.hpp"
#include "redes.h"

#include "gui.h"
#include "cuenta_nueva.h"

using namespace std;
using namespace cv;

extern cv::Mat region;
extern void cargar_variables_configuracion();
extern void procesar_queue_cntrl();

extern void mostrar_gif(string);

/* La forma más rápida de fracasar es no intentarlo */
/* Querer es poder */

int borrame;

/**Necesitamos tres o más hilos:
1) Diagrama
2) Ciclo de eventos de asio (networking)
3) Ciclo de eventos de la gui
n) Más diagramas*/
int main()
{
  //this_thread::sleep_for(chrono::seconds(1));
  cargar_variables_configuracion();
  std::thread hilo_redes(redes_main); //hilo de redes siempre debe de correr

  /*Hilo de gui debe de correr al inicio, y siempre y cuando sea requerido por mjolnir
  Si mjolnir solicita una función de la gui y no hay ningún ciclo de fltk corriendo
  mjolnir debe iniciarlo. O el hilo de la redes tal vez.*/
  //std::thread hilo_gui(main_gui);
  thread hilogif(mostrar_gif, "catfail1.mp4");
  hilogif.detach();

  inicializar_diagrama();

  namedWindow("Mjolnir");
  moveWindow("Mjolnir",0,0);
  setMouseCallback("Mjolnir", manejarInputMouse);
  renderizarDiagrama(region);

  while (true)
  {
    int k = waitKey(15);

    if(k == 27) //tecla ESC
    {
      destroyWindow("Mjolnir");
      break;
    }

    if(k != -1) //-1 es retornado cuando el timer expira o cuando se cierra la ventana
      manejarInputTeclado(region, k);

    renderizarDiagrama(region);
    setMouseCallback("Mjolnir", manejarInputMouse);
    procesar_queue_cntrl();
  }

  //hilo_gui.join();

  //fin de la aplicación
  iosvc.stop();
  hilo_redes.join();
  return 0;
}

/*
Se inician redes (qué pasa si llega algo por redes y se está iniciando mjolnir?) no importa por el mecanismo
  de la queue y fptrs, etc.
Se crea la GUI.
Se crea MJOLNIR

El programa termina sí y sólo sí la GUI y MJOLNIR tan terminado. Uno puede reiniciar al otro.
*/
