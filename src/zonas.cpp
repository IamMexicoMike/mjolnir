#include "zonas.hpp"

#define ofrasx 40
#define ofrasy -13
#define ofex 40
#define ofax ofex+60
#define P1(x,y) Point(x*100,y*100)
#define PTEL(x,y) Point(100*(x+ofex),100*(y))
#define PACA(x,y) Point(100*(x+ofax),100*(y))
#define PRAS(x,y) Point(100*(x+ofrasx),100*(y+ofrasy))
#define MACRO_RASURADO(x1,y1,x2,y2,x3,y3,x4,y4) {PRAS(x1,y1), PRAS(x2,y2), PRAS(x3,y3), PRAS(x4,y4)}
#define offset_planta2x -2000
#define offset_planta2y -10000
#define P2(x,y) Point(offset_planta2x + x*100, offset_planta2y + y*100)


using namespace std;
using namespace cv;

extern const Scalar Bckgnd(46,169,230);

const Scalar CAFE(0,51,102);
const Scalar COLOR_ZONA(125,189,200);
const Scalar COLOR_ZONA2(145,210,220);
const Scalar COLOR_AREA(145,209,220);
const Scalar COLOR_MAQ(102,255,255);
const Scalar AZUL_PALIDO(240,200,200);
const Scalar COLOR_OFICINA(0,76,153);
const Scalar COLOR_WC(120,110,50);
const Scalar COLOR_ENGOMADO(250,150,150);
const Scalar COLOR_METAL(200,200,200);
const Scalar COLOR_URDIDO(220,210,250);
const Scalar COLOR_SALIDA(255,255,255);
const Scalar COLOR_PT(25,120,255);
const Scalar COLOR_MAQUINILLA(51,255,255);
const Scalar COLOR_JACQUARD(102,0,204);
const Scalar COLOR_TALLER(102,102,0);
const Scalar COLOR_TENIDORA(151,26,196);
const Scalar COLOR_CORINO(54,43,200);
const Scalar COLOR_RAMA(123,0,51);
const Scalar COLOR_TUMBLER(81,167,234);
const Scalar COLOR_TABLERO(15,225,25);
const Scalar COLOR_RACK(100,100,100);
const Scalar AMARILLO(0,255,255);
const Scalar COLOR_RASURADO(233,124,60);
const Scalar COLORES[] = {Scalar(200,50,50), Scalar(50,200,50), Scalar(50,50,200), Scalar(200,50,200), Scalar(200,200,50),
                          Scalar(50,200,200), Scalar(250,200,150), Scalar(150,250,200), Scalar(200,150,250)};

vector<zona> superzonas(
{
  zona({P1(0,0), P1(40,0), P1(40,27.5), P1(0,27.5)}, COLOR_ZONA, "SALON DE URDIDO Y ENGOMADO"),
  zona({P1(0,-13), P1(40,-13), P1(40,0), P1(0,0)}, COLOR_ZONA, "Puerto1"),
  zona({PTEL(0,0), PTEL(60,0), PTEL(60,27.5), PTEL(0,27.5)}, COLOR_ZONA, ""/*SALON DE TELARES*/),
  zona({PACA(0,0), PACA(34.4,0), PACA(40,5.8), PACA(40,27.5), PACA(0,27.5)}, COLOR_ZONA, ""/*"SALON DE ACABADO"*/),
  zona({PRAS(0,0),PRAS(81.655,0),PRAS(86,4.5), PRAS(86,13),PRAS(0,13)}, COLOR_ZONA, "SALON DE CORTE, RASURADO Y ALMACEN CRUDO"),
  zona({P2(0,0), P2(7.6,0), P2(7.6,-1.10), P2(129.44,-1.10), P2(130,-6), P2(133.5,-11), P2(142.5,-11), P2(136.5,7), P2(147,43), P2(150,47.28), P2(142.4,51.68), P2(23,51.68), P2(23,64.23), P2(17,64.23), P2(17,76.63), P2(7,76.63), P2(7,106.63), P2(-3,106.63), P2(-3,76.63), P2(0,76.63)}, COLOR_ZONA, "Planta 2"),
  zona({P2(7.6,1.9), P2(127.48,1.9), P2(127.48,22.9), P2(7.6,22.9)}, COLOR_ZONA, "AREA DE PRODUCCION"),
  zona({P2(7.6,27.1), P2(127.48,27.1), P2(127.48,46.76), P2(7.6,46.76)}, COLOR_ZONA, ""),
});

vector<zona> zonas(
{
  zona({P1(1,0), P1(5.7,0), P1(5.7,4), P1(1,4)}, Scalar(200,200,200), "Comedor"),
  zona({P1(5.7,0), P1(11,0),  P1(11,4), P1(5.7,4)}, Scalar(100,100,200), "Caldera"),
  zona({P1(11,0), P1(15.8,0), P1(15.8,3.2), P1(11,3.2)}, Scalar(155,125,225), "Coccion"),
  zona({P1(15.8,0), P1(22.8,0), P1(22.8,1), P1(15.8,1)}, COLOR_TABLERO, "Tableros"),
  zona({P1(22.8,0), P1(26.3,0), P1(26.3,.3), P1(22.8,.3)}, COLOR_SALIDA, ""),
  zona({P1(26.3,0), P1(31.3,0), P1(31.3,5.2), P1(26.3,5.2)}, AZUL_PALIDO, "Supervision"),
  zona({P1(31.3,0),P1(35,0), P1(35,5.2),  P1(31.3,5.2)}, COLOR_WC, "W.C."),
  zona({P1(35,0), P1(40,0), P1(40,5.2), P1(35,5.2)}, AZUL_PALIDO, "Jefatura"),
  zona({P1(1,6), P1(13.2,6), P1(13.2,9.3), P1(1,9.3)}, COLOR_ENGOMADO, "BANCADA DE JULIOS"),
  zona({P1(13.5,4.9), P1(25,4.9), P1(25,10.4), P1(13.5,10.4)}, COLOR_ENGOMADO, "AREA DE ENGOMADO"),
  zona({P1(28,6), P1(38,6), P1(38,8.8), P1(28,8.8)}, COLOR_AREA, "ALMACEN JULIOS VACIOS"),
  zona({P1(1,12), P1(29.7,12), P1(29.7,16.6), P1(1,16.6)}, COLOR_ENGOMADO, "AREA DE FILETAS Y URDIDO 1"),
  zona({P1(32,12), P1(38,12), P1(38,16.6), P1(32,16.6)}, COLOR_AREA, "KANBAN URDIDO"),
  zona({P1(4,17), P1(32.7,17), P1(32.7,21.5), P1(4,21.5)}, COLOR_ENGOMADO, "AREA DE FILETAS Y URDIDO 2"),
  zona({P1(34,17), P1(39.5,17), P1(39.5,25), P1(34,25)}, COLOR_AREA, "KANBAN URDIDO"),
  zona({P1(.4,22), P1(9.8,22), P1(9.8,27.5), P1(.4,27.5)}, COLOR_AREA, "ALMACEN VARIOS"),
  zona({P1(10,22), P1(19.8,22), P1(19.8,27.5), P1(10,27.5)}, COLOR_AREA, "KANBAN MERMA"),
  zona({P1(20,24), P1(25,24), P1(25,27.5), P1(20,27.5)}, COLOR_MAQ, "CONERA2"),
  zona({P1(25.2,24), P1(30.2,24), P1(30.2,27.5), P1(25.2,27.5)}, COLOR_MAQ, "CONERA1"),
  zona({P1(0,8), P1(.3,8), P1(.3,14), P1(0,14)}, COLOR_SALIDA, ""),
  zona({P1(39.7,12.5), P1(40,12.5), P1(40,15), P1(39.7,15)}, COLOR_SALIDA, ""),
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  zona({P1(1,-12), P1(39,-12), P1(39,-1), P1(1,-1)}, COLOR_ZONA2, "Muelle1"),
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  zona({PTEL(0,12.5), PTEL(.5,12.5), PTEL(.5,15), PTEL(0,15)}, COLOR_SALIDA, ""),
  zona({PTEL(0 + 59.5,12.5), PTEL(60,12.5), PTEL(60,15), PTEL(59.5,15)}, COLOR_SALIDA, ""),
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  zona({PACA(0,0), PACA(3.7,0), PACA(3.7,9.5), PACA(0,9.5)}, COLOR_TALLER, "TALLER HERRERIA"),
  zona({PACA(3.7,0), PACA(7.7,0), PACA(7.7,.5), PACA(3.7,.5)}, COLOR_SALIDA, ""),
  zona({PACA(7.7,0), PACA(12.4,0), PACA(12.4,4.7), PACA(7.7,4.7)}, COLOR_OFICINA, "OFICINAS"),
  zona({PACA(12.4,0), PACA(21.4,0), PACA(21.4,6.7), PACA(12.4,6.7)}, COLOR_TENIDORA, "TENIDORA THIES"),
  zona({PACA(23.4,0), PACA(26.6,0), PACA(26.6,6.7), PACA(23.4,6.7)}, COLOR_TENIDORA, "JET 250"),
  zona({PACA(26.8,0), PACA(34.4,0), PACA(34.4,6.7), PACA(26.8,6.7)}, COLOR_TENIDORA, "JET 750"),
  zona({PACA(28,9), PACA(35.2,9), PACA(35.2,14), PACA(28,14)}, COLOR_TENIDORA, "JET 500"),
  zona({PACA(4,5), PACA(12,5), PACA(12,14), PACA(4,14)}, COLOR_AREA, "ALMACEN CRUDO"),
  zona({PACA(14,19.8), PACA(37.4,19.8), PACA(37.4,25), PACA(14,25)}, COLOR_RAMA, "RAMA"),
  zona({PACA(14,15.1), PACA(30.3,15.1), PACA(30.3,18.8), PACA(14,18.8)}, COLOR_TUMBLER, "TUMBLER"),
  zona({PACA(14,8), PACA(19,8), PACA(20,10), PACA(22,10), PACA(22,12), PACA(20,12), PACA(19,14), PACA(14,14)}, COLOR_CORINO, "CORINO"), //segun mariano su h vale 7.0, le puse 6.0
  zona({PACA(22,13), PACA(25,13), PACA(25,14), PACA(22,14)}, COLOR_TABLERO, ""),
  zona({PACA(30,26.5), PACA(37,26.5), PACA(37,27.5), PACA(30,27.5)}, COLOR_TABLERO, "Tableros de control"),
  zona({PACA(.5,20), PACA(12,20), PACA(12,27), PACA(.5,27)}, COLOR_AREA, "CARGAS DE TENIDO"),
  zona({PACA(1,15), PACA(6,15), PACA(6,19), PACA(1,19)}, COLOR_MAQ, "PLEGADORA"),
  zona({PACA(0,12.5), PACA(.5,12.5), PACA(.5,15), PACA(0,15)}, COLOR_SALIDA, ""),
  zona({PACA(39.5,12.5), PACA(40,12.5), PACA(40,15), PACA(39.5,15)}, COLOR_SALIDA, ""),
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  zona(MACRO_RASURADO(0,0,9.2,0,9.2,2.45,0,2.45), COLOR_OFICINA, "OFICINA COMPRAS"),
  zona(MACRO_RASURADO(0,2.45,.3,2.45,.3,6.29,0,6.29), COLOR_SALIDA, ""),
  zona(MACRO_RASURADO(0,6.29,5.57,6.29,5.57,13,0,13), COLOR_OFICINA, ""),
  zona(MACRO_RASURADO(7,6,13,6,13,8,7,8), COLOR_AREA, "ALMACEN CRUDO/HILO"),
  zona(MACRO_RASURADO(7,10,13,10,13,13,7,13), COLOR_AREA, "ALMACEN ACEITES"),
  zona(MACRO_RASURADO(58.3,3.5,63.7,3.5,63.7,13,58.3,13), AMARILLO, "Lab. Desarrollo"),
  zona(MACRO_RASURADO(18,3.5,38,3.5,38,5.5,18,5.5), COLOR_AREA, "ALMACEN CRUDO/RASURADO"),
  zona(MACRO_RASURADO(38.3,3.5,58.3,3.5,58.3,5.5,38.3,5.5), COLOR_AREA, "ALMACEN RASURADO"),
  zona(MACRO_RASURADO(52,6,57,6,57,12,52,12), COLOR_RASURADO, "Rasuradora2"),
  zona(MACRO_RASURADO(46,6,51,6,51,12,46,12), COLOR_RASURADO, "Rasuradora1"),
  zona(MACRO_RASURADO(26,10,42,10,42,12,26,12), COLOR_RASURADO, "MESA DE CORTE"),
  //zona(MACRO_RASURADO(38000,3500,48000,3500,48000,5500,38000,5500), COLOR_AREA, "ALMACEN RASURADO"),

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  zona({P2(0,0), P2(7.6,0), P2(7.6,17.9), P2(0,17.9)},COLORES[0], "Corte"),
  zona({P2(0,17.9), P2(7.6,17.9), P2(7.6,27.2), P2(0,27.2)},COLOR_TALLER, "Residuos"),
  zona({P2(7.6,1.9), P2(12.6,1.9), P2(12.6,7), P2(7.6,7)},COLORES[1], "Especialidades"),
  zona({P2(7.6,7), P2(12.6,7), P2(12.6,13), P2(7.6,13)},COLORES[3], "Mantenimiento"),
  zona({P2(7.6,13), P2(12.6,13), P2(12.6,17.9), P2(7.6,17.9)},COLORES[2], "Etiquetas"),
  zona({P2(12.6,1.9), P2(20,1.9), P2(20,22.9), P2(7.6,22.9), P2(7.6,17.9), P2(12.6,17.9)},COLORES[4], "Corte longitudinal"),
  zona({P2(20,1.9), P2(28.33,1.9), P2(28.33,22.9), P2(20,22.9) }, COLOR_MAQ, "Costura longitudinal"),
  zona({P2(28.33,1.9), P2(36.66,1.9), P2(36.66,22.9), P2(28.33,22.9) }, COLORES[7], "Corte transversal"),
  zona({P2(36.66,1.9), P2(45,1.9), P2(45,22.9), P2(36.66,22.9) }, COLORES[5], "Empaque"),
  zona({P2(45,1.9), P2(60,1.9), P2(60,5), P2(45,5) }, COLORES[1], "Desarrollo de muestras"),
  zona({P2(60,1.9), P2(60,5), P2(45,5), P2(45,22.9), P2(95,22.9), P2(95,14), P2(93,14), P2(93,1.9)}, COLORES[8], "Costura"),
  zona({P2(95,14), P2(110,14), P2(110,22.9), P2(95,22.9) }, COLORES[5], "Plotters"),
  zona({P2(93,1.9), P2(110,1.9), P2(110,11), P2(93,11) }, COLORES[6], "Transfer"),
  zona({P2(116,18), P2(125,18), P2(125,22.9), P2(116,22.9) }, COLOR_OFICINA, "Ofc. Calidad"),
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  zona({P2(0,46.7),P2(7.6,46.7),P2(7.6,67),P2(0,67)}, COLOR_ZONA2, "Muelle2"),
  zona({P2(0,27.2),P2(7.6,27.2), P2(7.6,46.7), P2(0,46.7),}, COLORES[7], "Recepcion de rollos"),
  zona({P2(7.6,27.2),P2(38,27.2), P2(38,46.7), P2(7.6,46.7),}, COLOR_TUMBLER, "Embarques"),
  zona({P2(38,27.2),P2(127.4,27.2), P2(127.4,46.7), P2(38,46.7),}, COLOR_PT, "P.T."),
});

void anexar_zonas()
{
  for(auto& z : zonas)
    crear_objeto(z);
  zonas.clear();
}

void rellenar_zona_telares() //está de lujo esta función. bien hechiza
{
  for(int j=1; j<5; ++j)
    for(int i=1; i<8; ++i)
    {
      Scalar c;
      if(i==5 || (j==3 && (i==1 || i==2)) || (j==4 && i==2))
        c = COLOR_MAQUINILLA;
      else
        c = COLOR_JACQUARD;


      zona z({Point(100*i + 700*(i-1) + 300 + ofex*100, 100*j + 500*(j-1) + (j/3)*250),
                               Point(100*i + 700*(i) + 300 + ofex*100, 100*j + 500*(j-1) + (j/3)*250),
                               Point(100*i + 700*(i) + 300 + ofex*100, 100*j + 500*(j) + (j/3)*250),
                               Point(100*i + 700*(i-1) + 300 + ofex*100, 100*j + 500*(j) + (j/3)*250)},
                               c, 'T' + to_string(j*10 + i));
      crear_objeto(z);
    }
}

void zona::dibujarse(cv::Mat& m) const
{
  vector<Point> ps = puntos_desplazados();
  fillConvexPoly(m, ps.data(), ps.size(), color());
  if(b_seleccionado_)
    polylines(m, ps, true, COLOR_SELECCION, 3, CV_AA); //selección

  if(b_highlighteado_)
    polylines(m, ps, true, COLOR_HIGHLIGHT_, 2, CV_AA); //highlight
}

void zona::arrastrar(const Point pt) //no es realmente un punto, sino una diferencia entre dos puntos. Debe ser absoluto
{
    for(auto p : puntos_)
      p += pt;
}

bool zona::pertenece_a_area(const cv::Point pt) const
{
  if(pointPolygonTest(puntos_, pt, false) > 0)
    return true;
  return false;
}

void zona::imprimir_datos() const
{
  cout << nombre() << " : " << id() << '\t';
  for(auto p : puntos_)
    cout << p;
  cout << '\n';
}

#undef P2
#undef P1
