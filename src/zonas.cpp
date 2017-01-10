#include "zonas.hpp"

#define offset_rasuradox 40000
#define offset_rasuradoy -13000
#define MACRO_RASURADO(x1,y1,x2,y2,x3,y3,x4,y4) {Point(offset_rasuradox + (x1),offset_rasuradoy + (y1)), Point(offset_rasuradox + (x2),offset_rasuradoy + (y2)), Point(offset_rasuradox + (x3),offset_rasuradoy + (y3)), Point(offset_rasuradox + (x4),offset_rasuradoy + (y4))}
#define offset_planta1x

using namespace std;
using namespace cv;

Point operator/(Point p, const int d)
{
  return Point(p.x/d, p.y/d);
}

extern const Scalar Bckgnd(46,169,230);

const Scalar COLOR_ZONA(125,189,200);
const Scalar COLOR_AREA(145,209,220);
const Scalar COLOR_MAQ(102,255,255);
const Scalar AZUL_PALIDO(240,200,200);
const Scalar COLOR_OFICINA(0,76,153);
const Scalar COLOR_WC(120,110,50);
const Scalar COLOR_ENGOMADO(250,150,150);
const Scalar COLOR_METAL(200,200,200);
const Scalar COLOR_URDIDO(220,210,250);
const Scalar COLOR_SALIDA(255,255,255);

const int offset_telares=40000;
const int offset_acabado=offset_telares+60000;
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

vector<zona> superzonas(
{
  zona({Point(0,0), Point(40000,0), Point(40000,27500), Point(0,27500)}, COLOR_ZONA, "SALON DE URDIDO Y ENGOMADO"),
  zona({Point(offset_telares,0), Point(offset_telares + 60000,0), Point(offset_telares + 60000,27500), Point(offset_telares,27500)}, COLOR_ZONA, ""/*SALON DE TELARES*/),
  zona({Point(offset_acabado,0), Point(offset_acabado + 34400,0), Point(offset_acabado + 40000,5800), Point(offset_acabado + 40000,27500), Point(offset_acabado,27500)}, COLOR_ZONA, ""/*"SALON DE ACABADO"*/),
  zona({Point(offset_rasuradox,offset_rasuradoy),Point(offset_rasuradox + 81655, offset_rasuradoy),Point(offset_rasuradox+86000,offset_rasuradoy+4500), Point(offset_rasuradox+86000,offset_rasuradoy+13000),Point(offset_rasuradox,offset_rasuradoy+13000)}, COLOR_ZONA, "SALON DE CORTE, RASURADO Y ALMACEN CRUDO"),

});

vector<zona> zonas(
{
  zona({Point(1000,0), Point(5700,0), Point(5700,4000), Point(1000,4000)}, Scalar(200,200,200), "Comedor"),
  zona({Point(5700,0), Point(11000,0),  Point(11000,4000), Point(5700,4000)}, Scalar(100,100,200), "Caldera"),
  zona({Point(11000,0), Point(15800,0), Point(15800,3200), Point(11000,3200)}, Scalar(155,125,225), "Coccion"),
  zona({Point(15800,0), Point(22800,0), Point(22800,1000), Point(15800,1000)}, COLOR_TABLERO, "Tableros"),
  zona({Point(22800,0), Point(26300,0), Point(26300,300), Point(22800,300)}, COLOR_SALIDA, ""),
  zona({Point(26300,0), Point(31300,0), Point(31300,5200), Point(26300,5200)}, AZUL_PALIDO, "Supervision"),
  zona({Point(31300,0),Point(35000,0), Point(35000,5200),  Point(31300,5200)}, COLOR_WC, "W.C."),
  zona({Point(35000,0), Point(40000,0), Point(40000,5200), Point(35000,5200)}, AZUL_PALIDO, "Jefatura"),
  zona({Point(1000,6000), Point(13200,6000), Point(13200,9300), Point(1000,9300)}, COLOR_ENGOMADO, "BANCADA DE JULIOS"),
  zona({Point(13500,4900), Point(25000,4900), Point(25000,10400), Point(13500,10400)}, COLOR_ENGOMADO, "AREA DE ENGOMADO"),
  zona({Point(28000,6000), Point(38000,6000), Point(38000,8800), Point(28000,8800)}, COLOR_AREA, "ALMACEN JULIOS VACIOS"),
  zona({Point(1000,12000), Point(29700,12000), Point(29700,16600), Point(1000,16600)}, COLOR_ENGOMADO, "AREA DE FILETAS Y URDIDO 1"),
  zona({Point(32000,12000), Point(38000,12000), Point(38000,16600), Point(32000,16600)}, COLOR_AREA, "KANBAN URDIDO"),
  zona({Point(4000,17000), Point(32700,17000), Point(32700,21500), Point(4000,21500)}, COLOR_ENGOMADO, "AREA DE FILETAS Y URDIDO 2"),
  zona({Point(34000,17000), Point(39500,17000), Point(39500,25000), Point(34000,25000)}, COLOR_AREA, "KANBAN URDIDO"),
  zona({Point(400,22000), Point(9800,22000), Point(9800,27500), Point(400,27500)}, COLOR_AREA, "ALMACEN VARIOS"),
  zona({Point(10000,22000), Point(19800,22000), Point(19800,27500), Point(10000,27500)}, COLOR_AREA, "KANBAN MERMA"),
  zona({Point(20000,24000), Point(25000,24000), Point(25000,27500), Point(20000,27500)}, COLOR_MAQ, "CONERA2"),
  zona({Point(25200,24000), Point(30200,24000), Point(30200,27500), Point(25200,27500)}, COLOR_MAQ, "CONERA1"),
  zona({Point(0,8000), Point(300,8000), Point(300,14000), Point(0,14000)}, COLOR_SALIDA, ""),
  zona({Point(39700,12500), Point(40000,12500), Point(40000,15000), Point(39700,15000)}, COLOR_SALIDA, ""),
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  zona({Point(offset_telares,12500), Point(offset_telares+500,12500), Point(offset_telares+500,15000), Point(offset_telares,15000)}, COLOR_SALIDA, ""),
  zona({Point(offset_telares + 59500,12500), Point(offset_telares + 60000,12500), Point(offset_telares + 60000,15000), Point(offset_telares + 59500,15000)}, COLOR_SALIDA, ""),
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  zona({Point(offset_acabado,0), Point(offset_acabado+3700,0), Point(offset_acabado+3700,9500), Point(offset_acabado,9500)}, COLOR_TALLER, "TALLER HERRERIA"),
  zona({Point(offset_acabado+3700,0), Point(offset_acabado+7700,0), Point(offset_acabado+7700,500), Point(offset_acabado+3700,500)}, COLOR_SALIDA, ""),
  zona({Point(offset_acabado+7700,0), Point(offset_acabado+12400,0), Point(offset_acabado+12400,4700), Point(offset_acabado+7700,4700)}, COLOR_OFICINA, "OFICINAS"),
  zona({Point(offset_acabado+12400,0), Point(offset_acabado+21400,0), Point(offset_acabado+21400,6700), Point(offset_acabado+12400,6700)}, COLOR_TENIDORA, "TENIDORA THIES"),
  zona({Point(offset_acabado+23400,0), Point(offset_acabado+26600,0), Point(offset_acabado+26600,6700), Point(offset_acabado+23400,6700)}, COLOR_TENIDORA, "JET 250"),
  zona({Point(offset_acabado+26800,0), Point(offset_acabado+34400,0), Point(offset_acabado+34400,6700), Point(offset_acabado+26800,6700)}, COLOR_TENIDORA, "JET 750"),
  zona({Point(offset_acabado+28000,9000), Point(offset_acabado+35200,9000), Point(offset_acabado+35200,14000), Point(offset_acabado+28000,14000)}, COLOR_TENIDORA, "JET 500"),
  zona({Point(offset_acabado+4000,5000), Point(offset_acabado+12000,5000), Point(offset_acabado+12000,14000), Point(offset_acabado+4000,14000)}, COLOR_AREA, "ALMACEN CRUDO"),
  zona({Point(offset_acabado+14000,19800), Point(offset_acabado+37400,19800), Point(offset_acabado+37400,25000), Point(offset_acabado+14000,25000)}, COLOR_RAMA, "RAMA"),
  zona({Point(offset_acabado+14000,15100), Point(offset_acabado+30300,15100), Point(offset_acabado+30300,18800), Point(offset_acabado+14000,18800)}, COLOR_TUMBLER, "TUMBLER"),
  zona({Point(offset_acabado+14000,8000), Point(offset_acabado+19000,8000), Point(offset_acabado+20000,10000), Point(offset_acabado+22000,10000), Point(offset_acabado+22000,12000), Point(offset_acabado+20000,12000), Point(offset_acabado+19000,14000), Point(offset_acabado+14000,14000)}, COLOR_CORINO, "CORINO"), //segun mariano su h vale 7.0, le puse 6.0
  zona({Point(offset_acabado+22000,13000), Point(offset_acabado+25000,13000), Point(offset_acabado+25000,14000), Point(offset_acabado+22000,14000)}, COLOR_TABLERO, ""),
  zona({Point(offset_acabado+30000,26500), Point(offset_acabado+37000,26500), Point(offset_acabado+37000,27500), Point(offset_acabado+30000,27500)}, COLOR_TABLERO, "Tableros de control"),
  zona({Point(offset_acabado+500,20000), Point(offset_acabado+12000,20000), Point(offset_acabado+12000,27000), Point(offset_acabado+500,27000)}, COLOR_AREA, "CARGAS DE TENIDO"),
  zona({Point(offset_acabado+1000,15000), Point(offset_acabado+6000,15000), Point(offset_acabado+6000,19000), Point(offset_acabado+1000,19000)}, COLOR_MAQ, "PLEGADORA"),
  zona({Point(offset_acabado,12500), Point(offset_acabado+500,12500), Point(offset_acabado+500,15000), Point(offset_acabado,15000)}, COLOR_SALIDA, ""),
  zona({Point(offset_acabado+39500,12500), Point(offset_acabado+40000,12500), Point(offset_acabado+40000,15000), Point(offset_acabado+39500,15000)}, COLOR_SALIDA, ""),
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  zona(MACRO_RASURADO(0,0,9200,0,9200,2450,0,2450), COLOR_OFICINA, "OFICINA COMPRAS"),
  zona(MACRO_RASURADO(0,2450,300,2450,300,6290,0,6290), COLOR_SALIDA, ""),
  zona(MACRO_RASURADO(0,6290,5570,6290,5570,13000,0,13000), COLOR_OFICINA, ""),
  zona(MACRO_RASURADO(7000,6000,13000,6000,13000,8000,7000,8000), COLOR_AREA, "ALMACEN CRUDO/HILO"),
  zona(MACRO_RASURADO(7000,10000,13000,10000,13000,13000,7000,13000), COLOR_AREA, "ALMACEN ACEITES"),
  zona(MACRO_RASURADO(58300,3500,63700,3500,63700,13000,58300,13000), AMARILLO, "Lab. Desarrollo"),
  zona(MACRO_RASURADO(38300,3500,58300,3500,58300,5500,38300,5500), COLOR_AREA, "ALMACEN RASURADO"),
  zona(MACRO_RASURADO(18000,3500,48000,3500,48000,5500,18000,5500), COLOR_AREA, "ALMACEN CRUDO/RASURADO"),
  zona(MACRO_RASURADO(52000,6000,57000,6000,57000,12000,52000,12000), COLOR_RASURADO, "R2"),
  zona(MACRO_RASURADO(46000,6000,51000,6000,51000,12000,46000,12000), COLOR_RASURADO, "R1"),
  zona(MACRO_RASURADO(26000,10000,42000,10000,42000,12000,26000,12000), COLOR_RASURADO, "MESA DE CORTE"),
  //zona(MACRO_RASURADO(38000,3500,48000,3500,48000,5500,38000,5500), COLOR_AREA, "ALMACEN RASURADO"),

});

void anexar_zonas()
{
  for(auto& z : zonas)
    crear_objeto(z);
  zonas.clear();
}

void rellenar_zona_telares() //está de lujo esta función
{
  for(int j=1; j<5; ++j)
    for(int i=1; i<8; ++i)
    {
      Scalar c;
      if(i==5 || (j==3 && (i==1 || i==2)) || (j==4 && i==2))
        c = COLOR_MAQUINILLA;
      else
        c = COLOR_JACQUARD;


      zona z({Point(1000*i + 7000*(i-1) + 3000 + offset_telares, 1000*j + 5000*(j-1) + (j/3)*2500),
                               Point(1000*i + 7000*(i) + 3000 + offset_telares, 1000*j + 5000*(j-1) + (j/3)*2500),
                               Point(1000*i + 7000*(i) + 3000 + offset_telares, 1000*j + 5000*(j) + (j/3)*2500),
                               Point(1000*i + 7000*(i-1) + 3000 + offset_telares, 1000*j + 5000*(j) + (j/3)*2500)},
                               c, to_string(j*10 + i));
      crear_objeto(z);
    }
}

void zona::dibujarse(cv::Mat& m)
{
  vector<Point> ps = puntos_desplazados();
  fillConvexPoly(m, ps.data(), ps.size(), color());
  if(b_seleccionado_)
    polylines(m, ps, true, COLOR_SELECCION, 3, CV_AA); //selección

  if(b_highlighteado_)
    polylines(m, ps, true, COLOR_HIGHLIGHT_, 1, CV_AA); //highlight
}

//se llama continuamente cuando haces drag, no sólo una vez
void zona::arrastrar(const Point pt) //no es realmente un punto, sino una diferencia entre dos puntos. Debe ser absoluto
{
    for(auto p : puntos_)
      p += pt;
}

bool zona::pertenece_a_area(const cv::Point pt)
{
  if(pointPolygonTest(puntos_, pt, false) > 0)
    return true;
  return false;
}
