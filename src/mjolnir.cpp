#include <iostream>
#include <chrono> //para profiles
#include <ctime> //para profiles
#include <fstream>
#include <algorithm>
#include <bitset> //borrame si no debuggeas
#include <future>
#include <thread>
#include <atomic>
#include <mutex>

#include <opencv2/opencv.hpp>

#include "elemento_diagrama.h" /**mjolnir no incluye su propio header?!*/
#include "mjolnir.hpp"
#include "zonas.hpp"

#include "gui.h"
#include "../cuenta_nueva.h"

using namespace std;
using namespace cv;

extern int paleta_colores();
extern void procesar_queue_cntrl();
extern const char* nombreDiagrama;

/* No intentar tus ideas es la forma más triste de no verlas tener éxito*/
/* no intentarlo es 100% probabilidad de fracaso */

const Scalar BLANCO(255,255,255);
const Scalar CAFE(0,51,102);
const Scalar GRIS(200,200,200);
const Scalar AZUL_PALIDO(240,200,200);
const Scalar Bckgnd(46,169,230);

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

bool b_dibujando_circulo;           //completar esta interfaz
Point puntoOrigenCirculo(0,0);

bool b_dibujando_objeto; //objeto temporal
Point puntoOrigenobjeto(0,0); //objeto temporal
Point puntoFinobjeto(0,0); //objeto temporal

Point puntoActualMouse(0,0); //se actualiza en cada evento del mouse

Point despl(4000,1300); //originalmente desplazamientoOrigen

vector<unique_ptr<objeto>>::iterator itr_seleccionado = objetos.end();
vector<unique_ptr<objeto>>::iterator itr_highlight = objetos.end();
bool b_drag=false;
bool b_resize=false;
Point ptInicioArrastre(0,0);
Point ptFinArrastre(0,0);
vector<unique_ptr<objeto>> objetos;

int ancho_region; //w = 2dx
int altura_region; //h = 2dy
Point dxy; // (w/2, h/2)
int zoom(32);
bool b_dibujar_nombres=false; //es en funcion del zoom
int tamanio_texto;
int ancho_texto;

string mensaje_derecho_superior;
mutex mtx_mensaje_derecho_superior;

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
La manera en la que se renderiza el diagrama es tomando los puntos absolutos (coordenadas) de los objetos o elementos a dibujar,
y aplicándoles una transformación para poder hacer zoom out. Esta transformación es dependiente del ancho del diagrama w, pues
al zoomoutear, la mitad del diagrama permanece en el centro.

p' = f(p) = dx + (p - dx - d)/z
dx = w/2, es decir el ancho de diagrama entre dos.
d = desplazamiento del origen
z es el factor zoom */

/**transforma los puntos absolutos a puntos relativos renderizables
p' = f(p) = dx + (p - dx - d)/z*/
Point transformar(const cv::Point p)
{
  Point pp = dxy + (p - dxy - despl)/zoom; //dxy es la mitad del tamaño del diagrama
  return pp;
}

/*magia*/

/**transforma un punto relativo a un punto absoluto
p = g(p') = z*(p' - dx) + dx + d */
Point transformacion_inversa(const cv::Point pp)
{
  Point p = zoom*(pp - dxy) + dxy + despl;
  return p;
}

void efecto_cuadricula(cv::Mat& matriz)
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
    tamanio_texto = 4/zoom;
    if(tamanio_texto==0)
      tamanio_texto=1;
    ancho_texto=4-zoom;
    if(ancho_texto<0)
      ancho_texto = 1;

    b_dibujar_nombres = true;
  }
  else
    b_dibujar_nombres = false;


  //dibujamos todos los objetos
  for(auto& p : objetos)
  {
    p->dibujarse(matriz);
    if(b_dibujar_nombres)
      p->dibujar_nombre(matriz);
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

  if(b_dibujando_objeto) //dibujamos un rectángulo temporal
    rectangle(matriz, Rect(transformar(puntoOrigenobjeto), transformar(puntoFinobjeto)),
              COLOR_RECT_DIBUJANDO, 2, CV_AA);

  mat_header = BLANCO;

  if(itr_seleccionado>=objetos.begin() && itr_seleccionado!=objetos.end())
    putText(matriz, string("" + (*itr_seleccionado)->nombre()),
            HEADER2, FONT_HERSHEY_PLAIN, 1, COLOR_SELECCION, 1, CV_AA);

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
void manejarInputTeclado(Mat& matriz, int k)
{
  cout << k << "!\n"; //borrame si no debugeas, o coméntame mejor!

  constexpr int DESPLAZAMIENTO = 1500;
  constexpr int TECLADO_FLECHA_ARRIBA = 2490368;
  constexpr int TECLADO_FLECHA_ABAJO = 2621440;
  constexpr int TECLADO_FLECHA_IZQUIERDA = 2424832;
  constexpr int TECLADO_FLECHA_DERECHA = 2555904;

  switch (k) {
  case TECLADO_FLECHA_ARRIBA:
    despl.y -= DESPLAZAMIENTO;
    break;
  case TECLADO_FLECHA_ABAJO:
    despl.y += DESPLAZAMIENTO;
    break;
  case TECLADO_FLECHA_DERECHA:
    despl.x += DESPLAZAMIENTO;
    break;
  case TECLADO_FLECHA_IZQUIERDA:
    despl.x -= DESPLAZAMIENTO;
    break;

  case 13: //tecla enter
    if(b_dibujando_objeto)
    {
      b_dibujando_objeto = false;
      rectangulo r(puntoOrigenobjeto, puntoFinobjeto);
      crear_objeto(r); //deben ser p y no p'
    }
    break;

  case 43: //+ zoom in
    if(zoom!=1)
      zoom = zoom/2;
    break;
  case 45: //- zoom out
    if(zoom!=1024) //64 es razonable
      zoom = zoom*2;
    break;

  case 48:
    //empujar_queue_cntrl("reboot");
  case 50: //debug
    push_funptr(&foo_gui);
    //push_funptr(&ventana_cuenta_nueva);
    break;
  case 51:
    establecer_mensaje("Cincuenta y uno");
    break;
  case 52:
    establecer_mensaje("Arbitrario");
    break;
  case 53:
    establecer_mensaje("");
    break;

  case 100: //d de debug
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

  case 101:
    cout << "\tobjetos.begin()\tobjetos.end()\titr_highlight\titr_seleccion\n";
    cout << '\t' << &*objetos.begin() << '\t' << &*objetos.end() << '\t' << &*itr_highlight << '\t' << &*itr_seleccionado << '\n';
    for(auto itr=objetos.begin(); itr!=objetos.end(); ++itr)
    {
      cout << &*itr << '\t' << (*itr)->id() << '\n';
    }

  case 103: //g de guardar

    break;
  case 108: //l de load(cargar)

    break;
  //case 110: //n - cerrar redes
    //iosvc.stop();
    //break;
  case 111: //o - ordenar
    ordenar_objetos();
    establecer_mensaje("objetos ordenados");
    break;
  case 112: //p - paleta de colores
    push_funptr(&paleta_colores);
    break;

  case 114: //r (ojo, R tiene su propia clave
    puntoOrigenobjeto = transformacion_inversa(puntoActualMouse); //convertimos p' en p
    puntoFinobjeto = puntoOrigenobjeto;
    b_dibujando_objeto = true;
    break;

  case 115: //s - simulacion
  {
    Point p = transformacion_inversa(puntoActualMouse);
    for(int i=0; i<100; ++i)
    {
      for(int j=0; j<100; ++j)
      {
        Point p1(p.x + i*(-10000), p.y + j*(-10000));
        Point p2(p.x + i*(-10000)-5000, p.y + j*(-10000)-5000);
        rectangulo r(p1,p2);
        crear_objeto(r);
      }
    }
  }
    determinar_propiedades_ubicacion(puntoActualMouse); //para actualizar highlight
    break;

  case 3014656: //suprimir, borrar objeto
    if(itr_seleccionado>=objetos.begin() && itr_seleccionado != objetos.end())
    {
      destruir_objeto_seleccionado();
      determinar_propiedades_ubicacion(puntoActualMouse); //para actualizar highlight
    }
    break;

  }
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
    /*Si estábamos dibujando un objeto y dimos click, lo insertamos y no hacemos nada más*/
    if(b_dibujando_objeto)
    {
      b_dibujando_objeto = false;
      rectangulo r(puntoOrigenobjeto, puntoFinobjeto);
      crear_objeto(r);

      /**solicitamos las propiedades del nuevo objeto a crear*/
      /***/
    }

    /*no estábamos dibujando un objeto, evaluamos el punto y establecemos condiciones para la selección y el arrastre*/
    else
    {
      botonMouseIzquierdoAbajo = true;
      auto itr = determinar_propiedades_ubicacion(transformacion_inversa(puntoActualMouse));

      if(itr!=objetos.end())
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
          /*if((*itr)->es_esquina(transformacion_inversa(puntoActualMouse)))
          {
            b_resize = true;
          }
          else*/
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
    b_resize = false;

    if(b_dibujando_flecha) //esto se va a revampear
    {
      //flechas.push_back(flecha(puntoInicioFlecha, cv::Point(x,y) + despl));
      auto itr = determinar_propiedades_ubicacion(transformacion_inversa(puntoActualMouse));
      if(itr_seleccionado!=objetos.end() && itr != itr_seleccionado)
      {
        /**Se dibujó una flecha de un objeto a otro, originalmente se entablaba relación*/
        cout << "interaccion?\tentre " << (*itr)->id() << " y " << (*itr_seleccionado)->id() << "\n";
      }
      b_dibujando_flecha = false;
    }
  }

  if(event == CV_EVENT_MOUSEMOVE)
  {
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
      /*else if(b_resize)
      {
        Point pt = transformacion_inversa(puntoActualMouse);
        Point dif = pt - ptInicioArrastre;
        (*itr_seleccionado)->resizear(dif);
        ptInicioArrastre = pt;
      }*/
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
  if(event==CV_EVENT_LBUTTONDBLCLK)
  {
    cout << "DBL CLICK\n";
  }

} //manejarInputMouse


/**Debe determinar propiedades del punto en función de la dimensión en la que está.
 No debe determinar si debe dibujarse. Actualmente highlightea... y muestra x,y del mouse*/
vector<unique_ptr<objeto>>::iterator determinar_propiedades_ubicacion(cv::Point p)
{
  /*Este lambda podría generalizarse si recibiera como argumentos el tipo de operación y la categoría del contenedor. Nubloso*/
  auto encontrarItrHighlight = [&]() -> vector<unique_ptr<objeto>>::iterator
  {
    for(auto itr=objetos.begin(); itr!=objetos.end(); ++itr)
      if((*itr)->pertenece_a_area(p)) //si el punto cae dentro del área de un objeto...
        return itr;
    return objetos.end();
  };

  vector<unique_ptr<objeto>>::iterator itr = encontrarItrHighlight(); //obtenemos un apuntador al objeto que es dueño de esa área

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

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
  case WM_CREATE:
    break;

  case WM_TIMER:
    //convertimos la matriz a un bitmap
    renderizarDiagrama(region); //actualizamos el contenido de la matriz
    imshow(nombreDiagrama, region);
    procesar_queue_cntrl();
    break;

  case WM_KEYDOWN:
    {
      bitset<32> bs = (int)wParam;
        cout << bs << '\n';
        return 0;
    }


  case WM_RBUTTONDOWN:
    {

    }
    break;

  case WM_RBUTTONUP:

    break;

  case WM_LBUTTONDOWN:

    break; ///LBUTTONDOWN

  case WM_LBUTTONUP:

    break;

  case WM_MOUSEMOVE:

    break;

  case WM_RBUTTONDBLCLK:

    break;

  case WM_CLOSE:
    DestroyWindow(hwnd);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}

void matrizAControl(HWND& hwnd, Mat& img)
{
  if (img.empty())
    return;

  int bpp = 8 * img.elemSize();
  assert((bpp == 8 || bpp == 24 || bpp == 32));

  //Get DC of your win control
  HDC hdc = GetDC(hwnd);

  // This is the rectangle where the control is defined
  // and where the image will appear
  RECT rr;
  GetWindowRect(hwnd, &rr);
  //rr.top AND rr.left are always 0
  int rectWidth = rr.right;
  int rectHeight = rr.bottom;

  /// DWORD ALIGNMENT AND CONTINOUS MEMORY
  /// The image must be padded 4bytes and must be continuous

  int padding = 0;
  //32 bit image is always DWORD aligned because each pixel requires 4 bytes
  if (bpp < 32)
    padding = 4 - (img.cols % 4);

  if(padding==4)
    padding = 0;

  cv::Mat tmpImg;
  if (padding > 0 || img.isContinuous() == false)
  {
    // Adding needed columns on the right (max 3 px)
    cv::copyMakeBorder(img, tmpImg, 0, 0, 0, padding, cv::BORDER_CONSTANT, 0);
  }
  else
  {
    tmpImg = img;
  }

  /// PREPARE BITMAP HEADER
  /// The header defines format and shape of the source bitmap in memory

  // extra memory space for palette -> 256*4
  uchar buffer[sizeof(BITMAPINFO) + 256*4];
  BITMAPINFO* bmi = (BITMAPINFO*)buffer;
  BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);
  memset(bmih, 0, sizeof(*bmih));
  bmih->biSize = sizeof(BITMAPINFOHEADER);
  bmih->biWidth = tmpImg.cols;
  bmih->biHeight = -tmpImg.rows;// DIB are bottom ->top -
  bmih->biPlanes = 1;
  bmih->biBitCount = bpp;
  bmih->biCompression = BI_RGB;

  if (bpp == 8)
  {
    RGBQUAD* palette = bmi->bmiColors;
    for (int i = 0; i < 256; i++)
    {
      palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
      palette[i].rgbReserved = 0;
    }
  }
  /// Draw to DC
    // tranfer memory block
    // NOTE: the padding border will be shown here. Anyway it will be max 3px width

    SetDIBitsToDevice(hdc,
      //destination rectangle
      0, 0, rectWidth, rectHeight,
      0, 0, 0, tmpImg.rows,
      tmpImg.data, bmi, DIB_RGB_COLORS);


  DeleteDC(hdc);
}

