#include <iostream>
#include <chrono> //para profiles
#include <ctime> //para profiles
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "mjolnir.hpp"
#include "redes.h"
#include "elemento_diagrama.h"
#include "zonas.hpp"
#include "puerto_serial.h"
#include "dialogos.h"

extern void mensaje(std::string, std::string);

using namespace std;
using namespace cv;

const Scalar BLANCO(255,255,255);
const Scalar CAFE(0,51,102);
const Scalar GRIS(200,200,200);
const Scalar AZUL_PALIDO(240,200,200);


Point operator/(Point p, const int d)
{
  return Point(p.x/d, p.y/d);
}

string Mjolnir::obtener_mensaje() /*Mensaje informativo en esquina superior derecha del diagrama*/
{
  return mensaje_derecho_superior;
}

void Mjolnir::establecer_mensaje(string m) /*Mensaje informativo en esquina superior derecha del diagrama*/
{
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

Point Mjolnir::transformar(const Point p)
{
  Point pp = dxy + (p - dxy - despl)/zoom; //dxy es la mitad del tamaño del diagrama
  return pp;
}

Point Mjolnir::transformacion_inversa(const Point pp) /*magia*/
{
  Point p = zoom*(pp - dxy) + dxy + despl;
  return p;
}

int Mjolnir::transformar_escalar(int i)
{
  return (i)/zoom;
}

int Mjolnir::transformar_escalar_inverso(int i) { return i*zoom; }

void Mjolnir::efecto_cuadricula()
{
  static vector<chrono::duration<double>> tiempos(100);
  static int cnt;
  int h = diagrama_.cols;
  int v = diagrama_.rows;
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
      line(diagrama_, pt1, pt2, BLANCO, 1, 4, 0);
      //line(diagrama_, Point(i,0), Point(i,v), BLANCO, 1, 4, 0); //cuadrícula, vertical
    }

    for(int i=szlado-(despl.y/zoom)%szlado; i<v; i+=szlado) //Mat::cols es int, no uint
    {
      pt1.x=0, pt1.y=i;
      pt2.x=h, pt2.y=i;
      line(diagrama_, pt1, pt2, BLANCO, 1, 4, 0);
      //line(diagrama_, Point(0,i), Point(h,i), BLANCO, 1, 4, 0); //cuadrícula, horizontal
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
void Mjolnir::renderizarDiagrama() //No hay pedo si tratamos de dibujar una región que no pertenece a diagrama_
{
  chrono::time_point<chrono::system_clock> t_inicial, t_final; //empezamos a medir tiempo
  t_inicial = chrono::system_clock::now();

  Point pabs = transformacion_inversa(puntoActualMouse);

  diagrama_ = bckgnd_;

  //efecto_cuadricula(diagrama_);

  if(zoom<=8)
  {
    tamanio_texto=1;
    b_dibujar_nombres = true;
  }
  else
    b_dibujar_nombres = false;

  for(auto& zz : superzonas) //relleno de superzonas
  {
    zz.dibujarse();
  }


  //dibujamos todos los objetos
  for(auto p = objetos.end()-1; p>=objetos.begin(); --p)
  {
    (*p)->dibujarse();
    if(b_dibujar_nombres)
      (*p)->dibujar_nombre();
  }

  if(b_dibujando_flecha) //dibujamos una flecha temporal
    arrowedLine(diagrama_, transformar(puntoInicioFlecha),
                transformar(puntoTerminoFlecha),
                COLOR_FLECHA_DIBUJANDO, 2, CV_AA);

  if(b_dibujando_objeto) //dibujamos un objeto temporal
    dibujar_objeto_temporal();

  encabezado_ = BLANCO;

  if(ptr_seleccionado != nullptr)
    putText(diagrama_, string("" + ptr_seleccionado->nombre()),
            HEADER2, FONT_HERSHEY_PLAIN, 1, COLOR_NEGRO, 1, CV_AA);

  string spabs = '(' + to_string(pabs.x) + ',' + to_string(pabs.y) + ')';
  putText(diagrama_, spabs, HEADER0, FONT_HERSHEY_PLAIN, 1, Scalar(230,100,0), 1, CV_AA); //PLAIN es más pequeña que SIMPLEX

  putText(diagrama_, obtener_mensaje(), HEADER_MSG, FONT_HERSHEY_PLAIN, 1, Scalar(100,0,255), 1, CV_AA);

  //string sprueba = "kanban urdido";
  //putText(diagrama_, sprueba, HEADER1, FONT_HERSHEY_PLAIN, 1, Scalar(230,100,0), 1, CV_AA);


  // medimos tiempo
  t_final = chrono::system_clock::now();
  chrono::duration<double> t_renderizar = t_final - t_inicial;
  //cout << t_renderizar.count() << "s\n";
  //static VideoCapture cap(0);
  //cap >> region;

  imshow(nombre_, diagrama_); //actualizamos el diagrama
}

//falta agregar los equivalentes con bloq mayus activadas
void Mjolnir::manejarInputTeclado(int k)
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
    if(ptr_seleccionado != nullptr)
      cout << "seleccionado=" << ptr_seleccionado->id();
    if(ptr_highlight != nullptr)
      cout << "\thighlighteado=" << ptr_highlight->id() << '\n';
    cout << '\n';
    for(auto& p : objetos) {
      cout << p->id() << " : ";
      p->imprimir_datos();
      cout << '\n';
    }
    break;

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
    if(ptr_seleccionado != nullptr)
    {
      crear_dialogo_objeto(ptr_seleccionado);
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
    dialogo_query((HWND)cvGetWindowHandle(nombre_));
    break;

  case 82: //r
    iniciar_creacion_objeto(Objetos::Rectangulo);
    break;

  case 83: //s - sincronizado
    iniciar_creacion_objeto(Objetos::Sincronizado);
    break;

  case 46: //suprimir, borrar objeto
    if(ptr_seleccionado != nullptr)
    {
      destruir_objeto_seleccionado();
      determinar_propiedades_ubicacion(puntoActualMouse); //para actualizar highlight
    }
    break;

  }
  b_cache_valida=false;
}

void Mjolnir::manejarInputMouse(int event, int x, int y, int flags, void*)
{
  static bool botonMouseIzquierdoAbajo(false); //flechas, drag, drag n drop
  static bool botonMouseDerechoAbajo(false); //panning

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
      auto ptr = determinar_propiedades_ubicacion(transformacion_inversa(puntoActualMouse));

      if(ptr != nullptr) //estamos dentro del área de un objeto y dimos click
      {
        if(ptr_seleccionado != nullptr) //si había otro brother seleccionado antes...
          ptr_seleccionado->seleccionar(false); //des-seleccionamos al anterior

        ptr->seleccionar(true); //seleccionamos al brother
        ptr_seleccionado = ptr; //actualizamos al seleccionado

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

      else if(ptr_seleccionado!=nullptr) //no caimos en nadie, pero había un brother seleccionado
      {
        ptr_seleccionado->seleccionar(false); //lo des-seleccionamos
        ptr_seleccionado=nullptr; //y reseteamos el id de selección
      }
    }
  }

  if(event == CV_EVENT_LBUTTONUP)
  {
    botonMouseIzquierdoAbajo = false;
    b_drag = false; //terminan las condiciones de arrastre y resize

    //si estaba resizeando una linea y caí en otro objeto, hago el centro de ese objeto el vértice de la línea
    auto ptr = determinar_propiedades_ubicacion(transformacion_inversa(puntoActualMouse));


    if(b_dibujando_flecha) //esto se va a revampear. 6 meses después no se ha "revampeado"
    {
      if(ptr_seleccionado!=nullptr && ptr != ptr_seleccionado && ptr!=nullptr)
      {
        /**Se entabla una relación entre dos objetos*/
        cout << "interaccion entre " << ptr->id() << " y " << ptr_seleccionado->id() << "\n";
        lock_guard<mutex> lck(mtx_objetos);
        auto ptrl = crear_relacion(this,ptr, ptr_seleccionado);
        objetos.emplace_back(std::move(ptrl));
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
        ptr_seleccionado->arrastrar(dif);
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
    if(ptr_seleccionado != nullptr)
    {
      ptr_seleccionado->dialogo_objeto();
    }
  }

} //manejarInputMouse


/**Debe determinar propiedades del punto en función de la dimensión en la que está.
 No debe determinar si debe dibujarse. Actualmente highlightea*/
objeto* Mjolnir::determinar_propiedades_ubicacion(cv::Point p)
{
  if(ptr_seleccionado != nullptr)
  {
    //checar si es un punto clave. pasamos un pointer a esta instancia de mjolnir
    if(ptr_seleccionado->pertenece_a_punto_clave(p)) //esto es mal diseño porque esta función modifica a los objetos
      return ptr_seleccionado;
  }

  objeto* ptr = encontrar_ptr_area(p); //obtenemos un apuntador al objeto que es dueño de esa área

  if(ptr == ptr_highlight) //no hacemos cambios, seguimos hovereando dentro del area del mismo objeto
    return ptr;

  /*Segundo caso: Había algo highlighteado pero ya no*/
  if(ptr==nullptr)
  {
    if(ptr_highlight!=nullptr) //al inicio del programa vale 0
      ptr_highlight->highlightear(false);//des-highlighteamos el anterior
    ptr_highlight = nullptr;
    return ptr;
  }

  /*Tercer caso: hay algo highlighteable*/
  ptr->highlightear(true); //highlighteamos al nuevo

  /*Si es que había otro highlighteado antes lo deshighlighteamos*/
  if(ptr_highlight!=nullptr)
    ptr_highlight->highlightear(false);

  ptr_highlight = ptr; //actualizamos la llave highlight

  return ptr;
}

void Mjolnir::iniciar_creacion_objeto(Objetos o)
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

void Mjolnir::terminar_creacion_objeto()
{
  b_dibujando_objeto = false;
  switch(Tipo_Objeto_Dibujando)
  {
  /*case (Objetos::Sincronizado):
  {
    Point p1=puntoOrigenobjeto;
    Point p2=puntoFinobjeto;
    sync_rect s(p1, p2);
    crear_sincronizado(s);
    break;
  }*/
  case(Objetos::Rectangulo):
    {
      rectangulo r(this, puntoOrigenobjeto, puntoFinobjeto);
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
        auto ps = make_unique<puerto_serial>(this, puntoOrigenobjeto, puntoFinobjeto,
                  iosvc, puerto_serial::puerto_temporal_, puerto_serial::baudios_temporales_);
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
      circulo c(this, puntoOrigenobjeto, radio);
      crear_objeto(c);
    }
    break;
  case(Objetos::Zona):
    break;
  case(Objetos::Linea):
    {
      linea l(this, puntoOrigenobjeto, puntoFinobjeto);
      crear_objeto(l);
    }
    break;
  case(Objetos::Cuadrado_Isometrico):
    {
      cuadrado_isometrico ci(this, puntoOrigenobjeto, puntoFinobjeto);
      crear_objeto(ci);
    }
  }

}

void Mjolnir::dibujar_objeto_temporal()
{
  switch(Tipo_Objeto_Dibujando)
  {
  case(Objetos::Sincronizado):
  case(Objetos::Puerto_Serial):
  case(Objetos::Rectangulo):
    rectangle(diagrama_, Rect(transformar(puntoOrigenobjeto), transformar(puntoFinobjeto)),
              COLOR_RECT_DIBUJANDO, 2, CV_AA);
    break;
  case(Objetos::Circulo):
    {
      Point hipot = puntoFinobjeto - puntoOrigenobjeto;
      int radio = hypot(hipot.x, hipot.y);
      cv::circle(diagrama_, transformar(puntoOrigenobjeto), transformar_escalar(radio),
               COLOR_RECT_DIBUJANDO, 2, CV_AA);
    }

    break;
  case(Objetos::Zona):
    break;
  case(Objetos::Linea):
    line(diagrama_, transformar(puntoOrigenobjeto), transformar(puntoFinobjeto), COLOR_BLANCO, 2, CV_AA);
    break;
  case (Objetos::Cuadrado_Isometrico):
    {
      cuadrado_isometrico ci(this, puntoOrigenobjeto, puntoFinobjeto);
      ci.dibujarse();
      break;
    }

  }

}

void Mjolnir::simulacion()
{
  Point p = transformacion_inversa(puntoActualMouse);
  for(int i=0; i<100; ++i)
  {
    for(int j=0; j<100; ++j)
    {
      Point p1(p.x + i*(-10000), p.y + j*(-10000));
      Point p2(p.x + i*(-10000)-5000, p.y + j*(-10000)-5000);
      circulo c(this, p1, (p2.x-p1.x)/2);
      crear_objeto(c);
    }
  }
  determinar_propiedades_ubicacion(puntoActualMouse); //para actualizar highlight
}

void Mjolnir::destruir_objeto_seleccionado()
{
  if(ptr_seleccionado!=nullptr)
  {
    std::lock_guard<std::mutex> lck(mtx_objetos);
    std::string paq = "ro" + std::to_string(ptr_seleccionado->id());
    //avisamos a las lineas que ese objeto ya no existe
    for(auto& o : objetos)
      o->avisar_objeto_destruido(ptr_seleccionado); //avisamos que el objeto con esa dirección será destruido
    auto itr_seleccionado = std::find_if(objetos.begin(), objetos.end(), [&](std::unique_ptr<objeto> const& obj)
      { return obj.get() == ptr_seleccionado; });
    objetos.erase(itr_seleccionado);
    ptr_seleccionado=ptr_highlight=nullptr;
    empujar_queue_saliente(paq);
    b_drag=false; //cuando hacias drag y suprimias terminabas con un dangling ptr
  }

}

void Mjolnir::destruir_objeto(int id)
{
  std::lock_guard<std::mutex> lck(mtx_objetos);
  auto itr = std::find_if(objetos.begin(), objetos.end(), [&](std::unique_ptr<objeto> const& obj)
    { return obj->id() == id; });
  if(itr != objetos.end())
  {
    objeto* ptr = (*itr).get();
    for(auto& o : objetos)
      o->avisar_objeto_destruido(ptr); //atento a los usos de ptr y de itr
    objetos.erase(itr); //itr solo es necesario para llamar a esta función
    ptr_seleccionado=ptr_highlight=nullptr;
    std::string paq = "ro" + std::to_string(id);
    empujar_queue_saliente(paq);
  }
}

void Mjolnir::ordenar_objetos() //debe ser llamada explícitamente por el usuario para evitar "sorpresas"
{
  std::lock_guard<std::mutex> lck(mtx_objetos);
  if(ptr_highlight != nullptr)
    ptr_highlight->highlightear(false);
  if(ptr_seleccionado != nullptr)
    ptr_seleccionado->seleccionar(false);
  std::sort(objetos.begin(),objetos.end(),[&]
            (const std::unique_ptr<objeto>& a, const std::unique_ptr<objeto>& b)
            {return a->area() < b->area();});
  ptr_seleccionado=ptr_highlight=nullptr;
  b_drag=false; //cuando hacias drag y suprimias terminabas con un dangling ptr
}
