#include <iostream>
#include <chrono> //para profiles
#include <ctime> //para profiles
#include <fstream>
#include <algorithm>
#include <bitset> //borrame si no debuggeas
#include <future>
#include <thread>

#include <opencv2/opencv.hpp>

#include "datos.hpp"
#include "elemento_diagrama.h"
#include "redes.h"

#include "gui.h"
#include "cuenta_nueva.h"

using namespace std;
using namespace cv;

/* La forma m�s r�pida de fracasar es no intentarlo */
/* Querer es poder */
/* El que busca encuentra */


const cv::Point HEADER0(5,20);
const cv::Point HEADER1(100,20);

const cv::Scalar BLANCO(255,255,255); //no se pueden constexpresear
const cv::Scalar GRIS(200,200,200);
const cv::Scalar AZUL_PALIDO(240,200,200);

constexpr int ANCHO_REGION = 1000;
constexpr int ANCHO_MENU = 200;
constexpr int ALTURA_REGION = 600;

cv::Mat region(ALTURA_REGION, ANCHO_REGION, CV_8UC3, cv::Scalar(200,200,200)); //debe tener un scope global
cv::Mat mat_panel = region.colRange(region.cols - 100, region.cols); //mn
cv::Mat mat_header = region.colRange(0, region.cols - 100).rowRange(0,30); //mn

bool botonMouseIzquierdoAbajo=false; //flechas, drag, drag n drop
bool botonMouseDerechoAbajo=false; //panning
cv::Point puntoClickMouseDerecho(0,0); //panning
cv::Point puntoInicioDesplazamiento(0,0); //panning

bool b_dibujando_flecha; //flechas temporales
cv::Point puntoInicioFlecha(0,0); //flechas temporales
cv::Point puntoTerminoFlecha(0,0); //flechas temporales

bool b_dibujando_circulo;           //completar esta interfaz
cv::Point puntoOrigenCirculo(0,0);

bool b_dibujando_objeto; //objeto temporal
cv::Point puntoOrigenobjeto(0,0); //objeto temporal
cv::Point puntoFinobjeto(0,0); //objeto temporal

cv::Point puntoActualMouse(0,0); //evita llamar WINAPI, se actualiza en cada evento del mouse

//demasiados magic numbers
void renderizarDiagrama(Mat& matriz) //No hay pedo si tratamos de dibujar una regi�n que no pertenece a matriz
{
  cv::Point& despl = glb::desplazamientoOrigen; //Alias, using s�lo sirve para tipos y no para variables

  std::chrono::time_point<std::chrono::system_clock> t_inicial, t_final; //empezamos a medir tiempo
  t_inicial = std::chrono::system_clock::now();

  /**Esto es para el "efecto cuadr�cula", que simula una matriz infinita*/
  matriz = GRIS;
  for(int i=15-(despl.x % 15); i<matriz.cols; i+=15) //"generamos" un efecto de desplazamiento de la cuadr�cula
    line(matriz, Point(i,0), Point(i,matriz.rows), BLANCO, 1, 4, 0); //cuadr�cula, vertical
  for(int i=15-(despl.y % 15); i<matriz.rows; i+=15) //Mat::cols es int, no uint
    line(matriz, Point(0,i), Point(matriz.cols,i), BLANCO, 1, 4, 0); //cuadr�cula, horizontal

  if(b_dibujando_flecha) //dibujamos una flecha temporal
    arrowedLine(matriz, Point(puntoInicioFlecha.x - despl.x, puntoInicioFlecha.y - despl.y),
                Point(puntoTerminoFlecha.x - despl.x,puntoTerminoFlecha.y - despl.y),
                COLOR_FLECHA_DIBUJANDO, 2, CV_AA);

  if(b_dibujando_objeto) //dibujamos un rect�ngulo temporal
  {
    rectangle(matriz, Rect(puntoOrigenobjeto - despl, puntoFinobjeto - despl),
              COLOR_RECT_DIBUJANDO, 2, CV_AA);
  }


  //dibujamos todas als flechas - actualmente no hace nada
  //for_each(flechas.begin(), flechas.end(), [&](flecha& f) {f.dibujarse(matriz, despl);});

  //dibujamos todos los objetos
  for(auto& rec : glb::objetos)
    rec.second.dibujarse(matriz, despl);
  //dibujamos todas las relaciones
  for(auto& rel : glb::relaciones)
    rel.second.dibujarse(matriz, despl);

  mat_panel = AZUL_PALIDO;
  mat_header = BLANCO;

  if(glb::llave_objeto_seleccionado > 0)
    putText(matriz, std::string("Seleccionado: " + std::to_string(glb::llave_objeto_seleccionado)),
            HEADER1, FONT_HERSHEY_PLAIN, 1, Scalar(230,100,0));

  putText(matriz, "Ayuda: F1", HEADER0, FONT_HERSHEY_PLAIN, 1, Scalar(230,100,0));

  // medimos tiempo
  t_final = std::chrono::system_clock::now();
  std::chrono::duration<double> t_renderizar = t_final - t_inicial;
  //cout << t_renderizar.count() << "s\n";

  imshow("Mjolnir", matriz); //actualizamos el diagrama
}

void manejarInputTeclado(Mat& matriz, int k) //k no incluye ni ctrl, ni shift, ni alt por mencionar algunas
{
  //cout << k << "!\n"; //borrame si no debugeas, o com�ntame mejor!

  constexpr int DESPLAZAMIENTO = 5;
  constexpr int TECLADO_FLECHA_ARRIBA = 2490368;
  constexpr int TECLADO_FLECHA_ABAJO = 2621440;
  constexpr int TECLADO_FLECHA_IZQUIERDA = 2424832;
  constexpr int TECLADO_FLECHA_DERECHA = 2555904;

  switch (k) {
  case TECLADO_FLECHA_ARRIBA:
    glb::desplazamientoOrigen.y -= DESPLAZAMIENTO;
    break;
  case TECLADO_FLECHA_ABAJO:
    glb::desplazamientoOrigen.y += DESPLAZAMIENTO;
    break;
  case TECLADO_FLECHA_DERECHA:
    glb::desplazamientoOrigen.x += DESPLAZAMIENTO;
    break;
  case TECLADO_FLECHA_IZQUIERDA:
    glb::desplazamientoOrigen.x -= DESPLAZAMIENTO;
    break;
  case 114: //r (ojo, R tiene su propia clave
    puntoOrigenobjeto = puntoActualMouse + glb::desplazamientoOrigen;
    puntoFinobjeto = puntoOrigenobjeto;
    b_dibujando_objeto = true;
    break;
  case 13: //tecla enter
    if(b_dibujando_objeto)
    {
      b_dibujando_objeto = false;
      crear_objeto(puntoOrigenobjeto, puntoFinobjeto);
    }
    break;
  case 103: //g de guardar
    guardar_todo();
    break;
  case 108: //l de load(cargar)
    cargar_todo();
    break;
  case 50: //debug
    cout << "valor global: " << glb::llave_objeto_highlight << endl;
    push_funptr(&foo_gui);
    //push_funptr(&ventana_cuenta_nueva);
    break;
  case 51:
    empujar_queue_saliente("ftp proceso_maestro.exe"); //al parecer no puedes declarar variables aqu� -fpermissive
    break;
  case 100: //d de debug
    cout << "obj sel: " << glb::llave_objeto_seleccionado << " obj hgl: " << glb::llave_objeto_highlight << endl;
    for(auto& ob : glb::objetos)
      cout << ob.first << "," << ob.second.id() << " | " << endl;
    for(auto& rel : glb::relaciones)
      cout << rel.first << "," << rel.second.id() << " | " << endl;
    cout << "\ntodas las relaciones:" << endl;
    for(auto& rel : glb::relaciones)
      cout << "relacion " << rel.second.id() << ": "
           << rel.second.get_objetos().first << ',' << rel.second.get_objetos().second << endl;
    if(glb::llave_objeto_seleccionado > 0)
    {
      cout << "relaciones del objeto " << glb::llave_objeto_seleccionado << ":" << endl;
      for(auto id : glb::objetos.at(glb::llave_objeto_seleccionado).get_relaciones())
        cout << id << endl;
    }
    break;

  case 43: //+ ensanchamos el diagrama
    if(region.cols < 2000)
    {
      region = cv::Mat(region.rows, region.cols+15, CV_8UC3, cv::Scalar(200,200,200)); //debe tener un scope global
      mat_panel = region.colRange(region.cols - 100, region.cols);
      mat_header = region.colRange(0, region.cols - 100).rowRange(0,30);
    }
    break;

  case 45: //- adelgazamos el diagrama
    if(region.cols > 200)
    {
      region = cv::Mat(region.rows, region.cols-15, CV_8UC3, cv::Scalar(200,200,200)); //debe tener un scope global
      mat_panel = region.colRange(region.cols - 100, region.cols);
      mat_header = region.colRange(0, region.cols - 100).rowRange(0,30);
    }
    break;

  case 3014656: //suprimir, borrar objeto
    if(glb::llave_objeto_seleccionado > 0)
    {
      destruir_objeto(glb::llave_objeto_seleccionado);
      ubicacion::determinar_propiedades_ubicacion(puntoActualMouse); //para actualizar highlight
    }
    break;

  }
    //cout << glb::desplazamientoOrigen << endl;
}

/**Este callback se invoca cada vez que hay un evento de mouse en la ventana a la cual se attache� el callback.
  Podr�as tener varias ventanas con diferentes funciones que manejen el mouse
  La l�gica de este callback debe estar encapsulada, y debe ser legible*/
void manejarInputMouse(int event, int x, int y, int flags, void*)
{
  cv::Point& despl = glb::desplazamientoOrigen; //Alias, using s�lo sirve para tipos y no para variables

  puntoActualMouse = cv::Point(x,y); //esta variable siempre lleva el rastro de d�nde est� el mouse
  //cout << (flags & CV_EVENT_FLAG_CTRLKEY) << endl;

  if(event == CV_EVENT_RBUTTONDOWN) //no necesita propiedades_ubicacion, es para panning
  {
    botonMouseDerechoAbajo = true;
    puntoClickMouseDerecho = puntoActualMouse;
    puntoInicioDesplazamiento = despl;
  }

  if(event == CV_EVENT_RBUTTONUP)
    botonMouseDerechoAbajo = false;

  if(event == CV_EVENT_LBUTTONDOWN)
  {
    /*Si est�bamos dibujando un objeto y dimos click, lo insertamos y no hacemos nada m�s*/
    if(b_dibujando_objeto)
    {
      b_dibujando_objeto = false;
      crear_objeto(puntoOrigenobjeto, puntoFinobjeto);

      /**solicitamos las propiedades del nuevo objeto a crear*/
      /***/
    }

    /*no est�bamos dibujando un objeto, evaluamos el punto y establecemos condiciones para la selecci�n y el arrastre*/
    else
    {
      botonMouseIzquierdoAbajo = true;
      auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + despl);

      if(props.first > 0 ) //props.first es el id
      {
        if(glb::llave_objeto_seleccionado > 0) //si hab�a otro brother seleccionado antes...
          glb::objetos.at(glb::llave_objeto_seleccionado).seleccionar(false); //des-seleccionamos al anterior

        glb::objetos.at(props.first).seleccionar(true); //seleccionamos al brother
        glb::llave_objeto_seleccionado = props.first; //actualizamos al seleccionado

        if(flags & CV_EVENT_FLAG_CTRLKEY) //vamos a dibujar flecha, no a arrastrar
        {
          b_dibujando_flecha = true; //a�adir condici�n
          puntoInicioFlecha = puntoActualMouse + despl; //a�adir condici�n
          puntoTerminoFlecha = puntoInicioFlecha; //"reseteamos" la flecha;
        }
        else //de lo contrario, arrastramos o resizeamos. Usamos las mismas variables
        {
          glb::ptInicioArrastre = puntoActualMouse + despl;
          glb::ptFinArrastre = glb::ptInicioArrastre;
          b_dibujando_flecha = false;
          if(glb::objetos.at(props.first).es_esquina(puntoActualMouse))
          {
            glb::b_resize = true;
          }
          else
          {
            glb::b_drag = true;
          }
        }
        //hay espacio para alt y shift. Afortunadamente drag y dibujar flecha son mutuamente excluyentes
      }

      else if(glb::llave_objeto_seleccionado > 0) //no caimos en nadie, pero hab�a un brother seleccionado
      {
        glb::objetos.at(glb::llave_objeto_seleccionado).seleccionar(false); //lo des-seleccionamos
        glb::llave_objeto_seleccionado=-1; //y reseteamos el id de selecci�n
      }
    }
  }

  if(event == CV_EVENT_LBUTTONUP)
  {
    botonMouseIzquierdoAbajo = false;
    glb::b_drag = false; //terminan las condiciones de arrastre y resize
    glb::b_resize = false;

    if(b_dibujando_flecha) //esto se va a revampear
    {
      //flechas.push_back(flecha(puntoInicioFlecha, cv::Point(x,y) + despl));
      auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + despl);
      if(props.first > 0 && props.first != glb::llave_objeto_seleccionado)
        glb::relaciones.emplace(relacion::sid - 1, relacion(glb::llave_objeto_seleccionado, props.first));

      b_dibujando_flecha = false;
    }
  }

  if(event == CV_EVENT_MOUSEMOVE)
  {
    if(botonMouseDerechoAbajo) //Panning. Movi�ndonos con click derecho apretado
    {
      despl.x = puntoInicioDesplazamiento.x + puntoClickMouseDerecho.x - x;
      despl.y = puntoInicioDesplazamiento.y + puntoClickMouseDerecho.y - y;
    }

    if(botonMouseIzquierdoAbajo) //Flechas. Dragging. Resizing. Moviendo el cursor con click izquierdo apretado.
    {
      //propiedades ubicacion, highlightear destino de flecha, posible drag n drop
      if(glb::b_drag)
      {
        cv::Point pt = puntoActualMouse + despl;
        cv::Point dif = pt - glb::ptInicioArrastre;
        glb::objetos.at(glb::llave_objeto_seleccionado).arrastrar(dif);
        glb::ptInicioArrastre = pt;
      }
      else if(glb::b_resize)
      {
        cv::Point pt = puntoActualMouse + despl;
        cv::Point dif = pt - glb::ptInicioArrastre;
        glb::objetos.at(glb::llave_objeto_seleccionado).resizear(dif);
        glb::ptInicioArrastre = pt;
      }
      //...

      if(b_dibujando_flecha)  //dibujando flecha temporal
      {
        /*props se va a usar despu�s para tener feedback con el highlight*/
        /*auto props = */ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + despl); //para highlightear el destino
        puntoTerminoFlecha = puntoActualMouse + despl; //la flecha es temporal, no se a�ade sino hasta que LBUTTONUP
      }
    }

    if(!botonMouseDerechoAbajo && !botonMouseIzquierdoAbajo)
      ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + despl);

    if(b_dibujando_objeto)
      puntoFinobjeto = cv::Point(x,y) + despl;

  } //CV_EVENT_MOUSEMOVE

} //manejarInputMouse