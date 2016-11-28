#include "zonas.hpp"

cv::Point operator/(cv::Point p, const int d)
{
  return cv::Point(p.x/d, p.y/d);
}

using namespace std;
using namespace cv;

const Scalar COLOR_ZONA(125,189,200);
const Scalar COLOR_AREA(0,51,102);
const Scalar COLOR_MAQ(102,255,255);
const Scalar AZUL_PALIDO(240,200,200);
const Scalar COLOR_WC(120,110,50);
const Scalar COLOR_ENGOMADO(250,150,150);
const Scalar COLOR_METAL(200,200,200);
const Scalar COLOR_URDIDO(220,210,250);
const Scalar COLOR_SALIDA(255,255,255);
extern const Scalar Bckgnd(46,169,230);

vector<zona> superzonas(
{
  zona({Point(0,0), Point(40000,0), Point(40000,27500), Point(0,27500)}, COLOR_ZONA, "SALON DE URDIDO Y ENGOMADO")
});

vector<zona> zonas(
{
  zona({Point(1000,0), Point(5700,0), Point(5700,4000), Point(1000,4000)}, Scalar(200,200,200), "Comedor"),
  zona({Point(5700,0), Point(11000,0),  Point(11000,4000), Point(5700,4000)}, Scalar(100,100,200), "Caldera"),
  zona({Point(11000,0), Point(15800,0), Point(15800,3200), Point(11000,3200)}, Scalar(155,125,225), "Coccion"),
  zona({Point(15800,0), Point(22800,0), Point(22800,1000), Point(15800,1000)}, Scalar(15,225,25), "Tableros"),
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
  zona({Point(39700,9000), Point(40000,9000), Point(40000,15000), Point(39700,15000)}, COLOR_SALIDA, "")
});


void dibujar_zonas(Mat& matriz)
{

}


