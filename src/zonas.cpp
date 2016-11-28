#include "zonas.hpp"

cv::Point operator/(cv::Point p, const int d)
{
  return cv::Point(p.x/d, p.y/d);
}

using namespace std;
using namespace cv;

const Scalar AZUL_PALIDO(240,200,200);
const Scalar COLOR_BANIOS(250,150,50);
const Scalar COLOR_ENGOMADO(120,110,50);
const Scalar COLOR_URDIDO(220,210,250);


zona z1({Point(0,0), Point(40000,0), Point(40000,27500), Point(0,27500)}, Scalar(125,189,200), "SALON DE URDIDO Y ENGOMADO");
zona z2({Point(1000,0), Point(5700,0), Point(5700,4000), Point(1000,4000)}, Scalar(200,200,200), "Comedor");
zona z3({Point(5700,0), Point(11000,0),  Point(11000,4000), Point(5700,4000)}, Scalar(100,100,200), "Caldera");
zona z4({Point(11000,0), Point(15800,0), Point(15800,3200), Point(11000,3200)}, Scalar(155,125,225), "Coccion");
zona z5({Point(15800,0), Point(22800,0), Point(22800,1000), Point(15800,1000)}, Scalar(15,225,25), "Tableros");
zona z6({Point(22800,0), Point(26300,0), Point(26300,500), Point(22800,500)}, Scalar(255,255,255), "Salida");
zona z7({Point(26300,0), Point(31300,0), Point(31300,5200), Point(26300,5200)}, AZUL_PALIDO, "Supervision");
zona z8({Point(31300,0),Point(35000,0), Point(35000,5200),  Point(31300,5200)}, COLOR_BANIOS, "Sanitarios");
zona z9({Point(35000,0), Point(40000,0), Point(40000,5200), Point(35000,5200)}, AZUL_PALIDO, "Jefatura");
zona z10({Point(1000,6000), Point(13200,6000), Point(13200,9300), Point(1000,9300)}, COLOR_ENGOMADO, "BANCADA DE JULIOS");
zona z11({Point(13500,4900), Point(25000,4900), Point(25000,10400), Point(13500,10400)}, COLOR_ENGOMADO, "AREA DE ENGOMADO");
zona z12({Point(1000,12000), Point(29700,12000), Point(29700,16600), Point(1000,16600)}, COLOR_ENGOMADO, "AREA DE FILETAS Y URDIDO 1");
zona z13({Point(4000,17000), Point(32700,17000), Point(32700,21500), Point(4000,21500)}, COLOR_ENGOMADO, "AREA DE FILETAS Y URDIDO 2");


zona z100({Point(15800,0), Point(-10000,1000), Point(-6000,1000), Point(-5000,-20000)}, Scalar(155,225,25), "Almacen gigante");

vector<zona> zonas({z1, z2, z3, z4, z5, z6, z7, z8, z9, z10, z11, z12, z13});
