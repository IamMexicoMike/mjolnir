#include <iostream>
#include <chrono> //para profiles
#include <ctime> //para profiles
#include <algorithm>
#include <bitset> //borrame si no debuggeas

#include <opencv2/opencv.hpp>

#include "redes.h"
#include "elemento_diagrama.h"
#include "mjolnir.hpp"
#include "zonas.hpp"
#include "puerto_serial.h"
#include "dialogos.h"

extern void mensaje(std::string, std::string);

using namespace std;
using namespace cv;

void iniciar_creacion_objeto(Objetos); //enum representado el tipo del objeto a crear
void terminar_creacion_objeto();
void dibujar_objeto_temporal();

const char* nombreDiagrama="diagrama de planta";

const Scalar BLANCO(255,255,255);
const Scalar CAFE(0,51,102);
const Scalar GRIS(200,200,200);
const Scalar AZUL_PALIDO(240,200,200);
//const Scalar Bckgnd(46,169,230);
Scalar Bckgnd(30,100,20);

class zona;
extern vector<zona> superzonas;

Mat region;
Mat mat_header;
Point HEADER_MSG;

bool botonMouseIzquierdoAbajo=false; //flechas, drag, drag n drop
bool botonMouseDerechoAbajo=false; //panning
Point puntoClickMouseDerecho(0,0); //panning
Point puntoInicioDesplazamiento(0,0); //panning

bool b_dibujando_flecha; //flechas temporales
Point puntoInicioFlecha(0,0); //flechas temporales
Point puntoTerminoFlecha(0,0); //flechas temporales

bool b_dibujando_objeto; //objeto temporal
Point puntoOrigenobjeto(0,0); //objeto temporal
Point puntoFinobjeto(0,0); //objeto temporal

Point puntoActualMouse(0,0); //se actualiza en cada evento del mouse

Point despl(4000,1300); //originalmente desplazamientoOrigen

Apuntador itr_seleccionado = objetos.end();
Apuntador itr_highlight = objetos.end();
bool b_drag=false;
Point ptInicioArrastre(0,0);
Point ptFinArrastre(0,0);
vector<unique_ptr<objeto>> objetos;
vector<unique_ptr<objeto>> objetos_invisibles;

int ancho_region; //w = 2dx
int altura_region; //h = 2dy
Point dxy; // (w/2, h/2)
int zoom(32);
bool b_dibujar_nombres=false; //es en funcion del zoom
int tamanio_texto;
int ancho_texto;

string mensaje_derecho_superior;
mutex mtx_mensaje_derecho_superior;

atomic<bool> b_cache_valida{false};
atomic<bool> b_puntos_relativos_validos{false};

Objetos Tipo_Objeto_Dibujando; //para qué era esto? un global conteniendo el tipo de objeto dibujando verdad?

Apuntador encontrar_itr_area(cv::Point& p)
{
  for(auto itr=objetos.begin(); itr!=objetos.end(); ++itr)
    if((*itr)->pertenece_a_area(p)) //si el punto cae dentro del área de un objeto...
      return itr;
  return objetos.end();
};

Point operator/(Point p, const int d)
{
  return Point(p.x/d, p.y/d);
}

string obtener_mensaje() /*Mensaje informativo en esquina superior derecha del diagrama*/
{
  lock_guard<mutex> lck(mtx_mensaje_derecho_superior);
  string s = mensaje_derecho_superior;
  return s;
}

void establecer_mensaje(string m) /*Mensaje informativo en esquina superior derecha del diagrama*/
{
  lock_guard<mutex> lck(mtx_mensaje_derecho_superior);
  mensaje_derecho_superior = m;
}

/**
La manera en la que se renderiza el diagrama es tomando los puntos absolutos (coordenadas) de los objetos
o elementos a dibujar, y aplicándoles una transformación lineal de desplazamiento y zoom.
Esta transformación es dependiente del ancho del diagrama w, pues
al zoomoutear, la mitad del diagrama permanece en el centro.
p' = f(p) = dx + (p - dx - d)/z
dx = w/2, es decir el ancho de diagrama entre dos.
d = desplazamiento del origen
z es el factor zoom */

Point transformar(const Point p)
{
  Point pp = dxy + (p - dxy - despl)/zoom; //dxy es la mitad del tamaño del diagrama
  return pp;
}

Point transformacion_inversa(const Point pp) /*magia*/
{
  Point p = zoom*(pp - dxy) + dxy + despl;
  return p;
}

int transformar_escalar(int i)
{
  return (i)/zoom;
}

int transformar_escalar_inverso(int i) { return i*zoom; }

void efecto_cuadricula(Mat& matriz)
{
  static vector<chrono::duration<double>> tiempos(100);
  static int cnt;
  int h = matriz.cols;
  int v = matriz.rows;
  Point pt1, pt2; //probablemente esto ayude a que la caché esté caliente con este valor durante estos ciclos
  chrono::time_point<chrono::system_clock> t_inicial, t_final; //empezamos a medir tiempo
  t_inicial = chrono::system_clock::now();

  if(zoom<=32)
  {
    const int szlado = 800/zoom;
    //cout << "szlado == " << szlado << '\n';
    for(int i=szlado-(despl.x/zoom)%szlado; i<h; i+=szlado) //"generamos" un efecto de desplazamiento de la cuadrícula
    {
      pt1.x=i, pt1.y=0; //Esto lo hacemos con la intención de que no se creen Puntos temporales en cada iteración
      pt2.x=i, pt2.y=v; //y que de esta manera las llamadas a malloc(si es que existen) desaparezcan
      line(matriz, pt1, pt2, BLANCO, 1, 4, 0);
      //line(matriz, Point(i,0), Point(i,v), BLANCO, 1, 4, 0); //cuadrícula, vertical
    }

    for(int i=szlado-(despl.y/zoom)%szlado; i<v; i+=szlado) //Mat::cols es int, no uint
    {
      pt1.x=0, pt1.y=i;
      pt2.x=h, pt2.y=i;
      line(matriz, pt1, pt2, BLANCO, 1, 4, 0);
      //line(matriz, Point(0,i), Point(h,i), BLANCO, 1, 4, 0); //cuadrícula, horizontal
    }
  }
  t_final = chrono::system_clock::now();
  chrono::duration<double> t_renderizar = t_final - t_inicial;
  tiempos[cnt] = t_renderizar;
  cnt++;
  if(cnt==100)
  {
    cnt--;
    chrono::duration<double> promedio;
    for(int i=0; i<cnt; ++i)
      promedio+=tiempos[i];
    promedio = promedio/cnt;
    cnt=0;
    //cout << "Promedio cuadricula: " << promedio.count() << "s\n";
  }

}


//demasiados magic numbers
void renderizarDiagrama(Mat& matriz) //No hay pedo si tratamos de dibujar una región que no pertenece a matriz
{
  chrono::time_point<chrono::system_clock> t_inicial, t_final; //empezamos a medir tiempo
  t_inicial = chrono::system_clock::now();

  Point pabs = transformacion_inversa(puntoActualMouse);

  matriz = Bckgnd;

  for(auto& zz : superzonas) //relleno de superzonas
  {
    vector<Point> ps = zz.puntos_desplazados();
    fillConvexPoly(matriz, ps.data(), ps.size(), zz.color());
  }

  //efecto_cuadricula(matriz);

  if(zoom<=8)
  {
    tamanio_texto=1;
    b_dibujar_nombres = true;
  }
  else
    b_dibujar_nombres = false;


  //dibujamos todos los objetos
  for(auto p = objetos.end()-1; p>=objetos.begin(); --p)
  {
    (*p)->dibujarse(matriz);
    if(b_dibujar_nombres)
      (*p)->dibujar_nombre(matriz);
  }


  for(auto& zz : superzonas) //contorno y nombre de superzonas
  {
    auto ps = zz.puntos_desplazados();
    //polylines(matriz, ps.data(), ps.size(), 1, true, BLANCO);
    polylines(matriz, ps, true, BLANCO, 1, CV_AA); //selección
    if(b_dibujar_nombres)
      putText(matriz, zz.nombre(), ps[0], FONT_HERSHEY_PLAIN, tamanio_texto, Scalar(0,0,0), ancho_texto, CV_AA);
  }

  if(b_dibujando_flecha) //dibujamos una flecha temporal
    arrowedLine(matriz, transformar(puntoInicioFlecha),
                transformar(puntoTerminoFlecha),
                COLOR_FLECHA_DIBUJANDO, 2, CV_AA);

  if(b_dibujando_objeto) //dibujamos un objeto temporal
    dibujar_objeto_temporal();

  mat_header = BLANCO;

  if(itr_seleccionado>=objetos.begin() && itr_seleccionado!=objetos.end())
    putText(matriz, string("" + (*itr_seleccionado)->nombre()),
            HEADER2, FONT_HERSHEY_PLAIN, 1, COLOR_NEGRO, 1, CV_AA);

  string spabs = '(' + to_string(pabs.x) + ',' + to_string(pabs.y) + ')';
  putText(matriz, spabs, HEADER0, FONT_HERSHEY_PLAIN, 1, Scalar(230,100,0), 1, CV_AA); //PLAIN es más pequeña que SIMPLEX

  putText(matriz, obtener_mensaje(), HEADER_MSG, FONT_HERSHEY_PLAIN, 1, Scalar(100,0,255), 1, CV_AA);

  //string sprueba = "kanban urdido";
  //putText(matriz, sprueba, HEADER1, FONT_HERSHEY_PLAIN, 1, Scalar(230,100,0), 1, CV_AA);


  // medimos tiempo
  t_final = chrono::system_clock::now();
  chrono::duration<double> t_renderizar = t_final - t_inicial;
  //cout << t_renderizar.count() << "s\n";
  //static VideoCapture cap(0);
  //cap >> region;

  //imshow("Mjolnir"); //actualizamos el diagrama
}

//falta agregar los equivalentes con bloq mayus activadas
void manejarInputTeclado(int k)
{
  cout << k << "!\n"; //borrame si no debugeas, o coméntame mejor!

  const int DESPLAZAMIENTO=1500;

  switch (k) {
  case 37: //<-
    despl.x -= DESPLAZAMIENTO;
  case 38: //flecha arriba
    despl.y -= DESPLAZAMIENTO;
    break;
  case 39: //->
    despl.y += DESPLAZAMIENTO;
    break;
  case 40: //flecha abajo
    despl.x += DESPLAZAMIENTO;
    break;

    break;

  case 13: //tecla enter
    if(b_dibujando_objeto)
    {
      terminar_creacion_objeto();
    }
    break;

  case 187: //+ zoom in
    if(zoom!=1)
      zoom = zoom/2;
    break;
  case 189: //- zoom out
    if(zoom!=1024) //64 es razonable
      zoom = zoom*2;
    break;

  case 48:
    empujar_queue_cntrl("reboot");
  case 50: //debug
    //push_funptr(&foo_gui);
    //push_funptr(&ventana_cuenta_nueva);
    break;
  case 51:
    establecer_mensaje("Cincuenta y uno");
    break;
  case 52:
    establecer_mensaje("Arbitrario.");
    break;
  case 53:
    establecer_mensaje("");
    break;

  case 67: //c de circulo
    iniciar_creacion_objeto(Objetos::Circulo);
    break;

  case 68: //d de debug
    cout << "\nDEBUG:\n";
    cout << "objetos.size() == " << objetos.size() << '\n';
    if(itr_seleccionado>=objetos.begin() && itr_seleccionado!=objetos.end())
      cout << "seleccionado=" << (*itr_seleccionado)->id();
    if(itr_highlight>=objetos.begin() && itr_highlight!=objetos.end())
      cout << "\thighlighteado=" << (*itr_highlight)->id() << '\n';
    cout << '\n';
    for(auto& p : objetos) {
      cout << p->id() << " : ";
      p->imprimir_datos();
      cout << '\n';
    }
    break;

  case 69:
    cout << "\tobjetos.begin()\tobjetos.end()\titr_highlight\titr_seleccion\n";
    cout << '\t' << &*objetos.begin() << '\t' << &*objetos.end() << '\t' << &*itr_highlight << '\t' << &*itr_seleccionado << '\n';
    for(auto itr=objetos.begin(); itr!=objetos.end(); ++itr)
    {
      cout << &*itr << '\t' << (*itr)->id() << '\n';
    }

  case 71: //g de guardar, pero no de ganar.
    {
      ofstream ofs("objetos.txt");
      for(auto& o : objetos)
        o->guardar(ofs);
    }

    break;

  case 73: // i
    iniciar_creacion_objeto(Objetos::Cuadrado_Isometrico);
    break;

  case 76: //l
    iniciar_creacion_objeto(Objetos::Linea);
    break;

  case 78: //n
  {
    if(itr_seleccionado >= objetos.begin() && itr_seleccionado < objetos.end() )
    {
      objeto* psel = (*itr_seleccionado).get();
      if (psel != nullptr)
        crear_dialogo_objeto(psel);
    }

    break;
  }
  case 79: //o - ordenar
    ordenar_objetos(); //es segura
    establecer_mensaje("objetos ordenados");
    break;
  case 80: //p - puerto serial
    iniciar_creacion_objeto(Objetos::Puerto_Serial);
    break;

  case 81: //q - query
    dialogo_query();
    break;

  case 82: //r
    iniciar_creacion_objeto(Objetos::Rectangulo);
    break;

  case 83: //s - simulacion
    iniciar_creacion_objeto(Objetos::Sincronizado);
    break;

  case 46: //suprimir, borrar objeto
    if(itr_seleccionado>=objetos.begin() && itr_seleccionado != objetos.end())
    {
      const type_info& tipo_zona = typeid(zona);
      const type_info& tipo_objeto = typeid(*(*itr_seleccionado));
      if(tipo_zona.hash_code() == tipo_objeto.hash_code()) //no queremos borrar zonas
        break;
      destruir_objeto_seleccionado();
      determinar_propiedades_ubicacion(puntoActualMouse); //para actualizar highlight
    }
    break;

  }
  b_cache_valida=false;
    //cout << desplazamientoOrigen << endl;
}

void manejarInputMouse(int event, int x, int y, int flags, void*)
{
  puntoActualMouse = cv::Point(x,y); //esta variable siempre lleva el rastro de dónde está el mouse
  //cout << (flags & CV_EVENT_FLAG_CTRLKEY) << endl;
  //cout << event << " " << flags << endl;

  if(event == CV_EVENT_RBUTTONDOWN) //panning
  {
    botonMouseDerechoAbajo = true;
    puntoClickMouseDerecho = puntoActualMouse;
    puntoInicioDesplazamiento = despl; //XXXXXXXXXXXXXXXX????????????
  }

  if(event == CV_EVENT_RBUTTONUP)
    botonMouseDerechoAbajo = false;

  if(event == CV_EVENT_LBUTTONDOWN)
  {
    botonMouseIzquierdoAbajo = true;

    /*Si estábamos dibujando un objeto y dimos click, lo insertamos y no hacemos nada más*/
    if(b_dibujando_objeto)
    {
      terminar_creacion_objeto();
      /**solicitamos las propiedades del nuevo objeto a crear*/
    }

    /*no estábamos dibujando un objeto, evaluamos el punto y establecemos condiciones para la selección y el arrastre*/
    else
    {
      auto itr = determinar_propiedades_ubicacion(transformacion_inversa(puntoActualMouse));

      if(itr>= objetos.begin() and itr!=objetos.end()) //estamos dentro del área de un objeto y dimos click
      {
        if(itr_seleccionado>objetos.begin() && itr_seleccionado!=objetos.end()) //si había otro brother seleccionado antes...
          (*itr_seleccionado)->seleccionar(false); //des-seleccionamos al anterior

        (*itr)->seleccionar(true); //seleccionamos al brother
        itr_seleccionado = itr; //actualizamos al seleccionado

        if(flags & CV_EVENT_FLAG_CTRLKEY) //vamos a dibujar flecha, no a arrastrar
        {
          b_dibujando_flecha = true; //añadir condición
          puntoInicioFlecha = transformacion_inversa(puntoActualMouse); //añadir condición
          puntoTerminoFlecha = puntoInicioFlecha; //"reseteamos" la flecha;
        }
        else //de lo contrario, arrastramos o resizeamos. Usamos las mismas variables
        {
          ptInicioArrastre = transformacion_inversa(puntoActualMouse);
          ptFinArrastre = ptInicioArrastre;
          b_dibujando_flecha = false;

          {
            b_drag = true;
          }
        }
        //hay espacio para alt y shift. Afortunadamente drag y dibujar flecha son mutuamente excluyentes
      }

      else if(itr_seleccionado!=objetos.end()) //no caimos en nadie, pero había un brother seleccionado
      {
        (*itr_seleccionado)->seleccionar(false); //lo des-seleccionamos
        itr_seleccionado=objetos.end(); //y reseteamos el id de selección
      }
    }
  }

  if(event == CV_EVENT_LBUTTONUP)
  {
    botonMouseIzquierdoAbajo = false;
    b_drag = false; //terminan las condiciones de arrastre y resize

    //si estaba resizeando una linea y caí en otro objeto, hago el centro de ese objeto el vértice de la línea
    auto itr = determinar_propiedades_ubicacion(transformacion_inversa(puntoActualMouse));


    if(b_dibujando_flecha) //esto se va a revampear
    {
      if(itr_seleccionado!=objetos.end() && itr != itr_seleccionado && itr!=objetos.end())
      {
        /**Se entabla una relación entre dos objetos*/
        cout << "interaccion entre " << (*itr)->id() << " y " << (*itr_seleccionado)->id() << "\n";
        crear_relacion(itr->get(), itr_seleccionado->get());
      }
      b_dibujando_flecha = false;
    }
  }

  if(event == CV_EVENT_MOUSEMOVE)
  {
    b_cache_valida=false;
    if(botonMouseDerechoAbajo) //Panning. Moviéndonos con click derecho apretado
    {
      despl = puntoInicioDesplazamiento + zoom*(puntoClickMouseDerecho - puntoActualMouse);
    }

    if(botonMouseIzquierdoAbajo) //Flechas. Dragging. Resizing. Moviendo el cursor con click izquierdo apretado.
    {
      //propiedades ubicacion, highlightear destino de flecha, posible drag n drop
      if(b_drag)
      {
        Point pt = transformacion_inversa(puntoActualMouse);
        Point dif = pt - ptInicioArrastre;
        (*itr_seleccionado)->arrastrar(dif);
        ptInicioArrastre = pt;
      }

      //...

      if(b_dibujando_flecha)  //dibujando flecha temporal
      {
        /*itr se va a usar después para tener feedback con el highlight*/
        /*auto itr = */determinar_propiedades_ubicacion(transformacion_inversa(puntoActualMouse)); //para highlightear el destino
        puntoTerminoFlecha = transformacion_inversa(puntoActualMouse); //la flecha es temporal, no se añade sino hasta que LBUTTONUP
      }
    }

    if(!botonMouseDerechoAbajo && !botonMouseIzquierdoAbajo)
      determinar_propiedades_ubicacion(transformacion_inversa(puntoActualMouse));

    if(b_dibujando_objeto)
      puntoFinobjeto = transformacion_inversa(puntoActualMouse);

  } //CV_EVENT_MOUSEMOVE

  if(event==CV_EVENT_LBUTTONDBLCLK) //doble_click -> vemos informacion del objeto
  {
    if(itr_seleccionado >= objetos.begin() && itr_seleccionado < objetos.end() )
    {
      objeto* psel = (*itr_seleccionado).get();
      if (psel != nullptr)
        psel->dialogo_objeto();
    }
  }

} //manejarInputMouse


/**Debe determinar propiedades del punto en función de la dimensión en la que está.
 No debe determinar si debe dibujarse. Actualmente highlightea... y muestra x,y del mouse*/
Apuntador determinar_propiedades_ubicacion(cv::Point p)
{
  if(itr_seleccionado>=objetos.begin() and itr_seleccionado<objetos.end())
  {
    //checar si es un punto clave
    if( (*itr_seleccionado)->pertenece_a_punto_clave(p) ) //esto es mal diseño porque esta función modifica a los objetos
      return (itr_seleccionado);

  }

  Apuntador itr = encontrar_itr_area(p); //obtenemos un apuntador al objeto que es dueño de esa área

  if(itr == itr_highlight) //no hacemos cambios, seguimos hovereando dentro del area del mismo objeto
    return itr;

  /*Segundo caso: Había algo highlighteado pero ya no*/
  if(itr==objetos.end())
  {
    if(itr_highlight>=objetos.begin() && itr_highlight!=objetos.end()) //al inicio del programa vale 0
      (*itr_highlight)->highlightear(false);//des-highlighteamos el anterior
    itr_highlight = objetos.end();
    return itr;
  }

  /*Tercer caso: hay algo highlighteable*/
  (*itr)->highlightear(true); //highlighteamos al nuevo

  /*Si es que había otro highlighteado antes lo deshighlighteamos*/
  if(itr_highlight!=objetos.end())
    (*itr_highlight)->highlightear(false);

  itr_highlight = itr; //actualizamos la llave highlight

  return itr;
}

void iniciar_creacion_objeto(Objetos o)
{
  b_dibujando_objeto = true;
  Tipo_Objeto_Dibujando = o;
  puntoOrigenobjeto = transformacion_inversa(puntoActualMouse); //convertimos p' en p
  puntoFinobjeto = puntoOrigenobjeto;
  switch (o) //aunque este switch no haga nada, nos da opciones para agregar comportamiento opcional
  {
  case(Objetos::Rectangulo):
    break;
  case(Objetos::Circulo):
    break;
  case(Objetos::Zona):
    break;
  case(Objetos::Linea):
    break;
  case(Objetos::Cuadrado_Isometrico):
    break;
  case(Objetos::Puerto_Serial):
    break;
  case(Objetos::Sincronizado):
    break;
  }

}

void terminar_creacion_objeto()
{
  b_dibujando_objeto = false;
  switch(Tipo_Objeto_Dibujando)
  {
  case(Objetos::Rectangulo):
    {
      rectangulo r(puntoOrigenobjeto, puntoFinobjeto);
      crear_objeto(r); //deben ser p y no p'
    }
    break;
  case (Objetos::Puerto_Serial):
    {
      vector<string> puertos_disponibles;
      for(int i=0; i<255; ++i)
      {
        DWORD prueba=0;
        char buf[256];
        string nomcom = "COM"+to_string(i);
        prueba = QueryDosDevice(nomcom.c_str(),buf,256); //al parecer buf no es usado pero es útil? ok
        if(prueba!=0)
        {
          puertos_disponibles.push_back(nomcom);
        }

        /* impresoras */
        nomcom = "LPT"+to_string(i);
        prueba = QueryDosDevice(nomcom.c_str(),buf,256); //al parecer buf no es usado pero es útil? ok
        if(prueba!=0)
        {
          puertos_disponibles.push_back(nomcom);
        }
      }

      //for(auto& s : puertos_disponibles)
        //cout << s << endl;
      if(crear_dialogo_serial(&puertos_disponibles))
      {
        try {
        auto ps = make_unique<puerto_serial>(puntoOrigenobjeto, puntoFinobjeto, iosvc, puerto_serial::puerto_temporal_, puerto_serial::baudios_temporales_);
        crear_objeto_delicado(std::move(ps));
        } catch (std::exception& e)
        {
          mensaje(e.what(), "Error al crear puerto serie");
        }
      }

    }
    break;

  case(Objetos::Circulo):
    {
      Point hipot = puntoFinobjeto - puntoOrigenobjeto;
      int radio = hypot(hipot.x, hipot.y);
      circulo c(puntoOrigenobjeto, radio);
      crear_objeto(c);
    }
    break;
  case(Objetos::Zona):
    break;
  case(Objetos::Linea):
    {
      linea l(puntoOrigenobjeto, puntoFinobjeto);
      crear_objeto(l);
    }
    break;
  case(Objetos::Cuadrado_Isometrico):
    {
      cuadrado_isometrico ci(puntoOrigenobjeto, puntoFinobjeto);
      crear_objeto(ci);
    }
  }

}

void dibujar_objeto_temporal()
{
  switch(Tipo_Objeto_Dibujando)
  {
  case(Objetos::Sincronizado):
  case(Objetos::Puerto_Serial):
  case(Objetos::Rectangulo):
    rectangle(region, Rect(transformar(puntoOrigenobjeto), transformar(puntoFinobjeto)),
              COLOR_RECT_DIBUJANDO, 2, CV_AA);
    break;
  case(Objetos::Circulo):
    {
      Point hipot = puntoFinobjeto - puntoOrigenobjeto;
      int radio = hypot(hipot.x, hipot.y);
      cv::circle(region, transformar(puntoOrigenobjeto), transformar_escalar(radio),
               COLOR_RECT_DIBUJANDO, 2, CV_AA);
    }

    break;
  case(Objetos::Zona):
    break;
  case(Objetos::Linea):
    line(region, transformar(puntoOrigenobjeto), transformar(puntoFinobjeto), COLOR_BLANCO, 2, CV_AA);
    break;
  case (Objetos::Cuadrado_Isometrico):
    {
      cuadrado_isometrico ci(puntoOrigenobjeto, puntoFinobjeto);
      ci.dibujarse(region);
      break;
    }

  }

}

void preparar_memoria() //muy sospechoso
{
  objetos.reserve(1024);
}

void simulacion()
{
  Point p = transformacion_inversa(puntoActualMouse);
  for(int i=0; i<100; ++i)
  {
    for(int j=0; j<100; ++j)
    {
      Point p1(p.x + i*(-10000), p.y + j*(-10000));
      Point p2(p.x + i*(-10000)-5000, p.y + j*(-10000)-5000);
      circulo c(p1, (p2.x-p1.x)/2);
      crear_objeto(c);
    }
  }
  determinar_propiedades_ubicacion(puntoActualMouse); //para actualizar highlight
}
