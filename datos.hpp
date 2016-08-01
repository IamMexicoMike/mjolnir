#define ANCHO_REGION 800
#define ANCHO_MENU 200
#define ALTURA_REGION 500

#define TECLADO_FLECHA_ARRIBA 2490368
#define TECLADO_FLECHA_ABAJO 2621440
#define TECLADO_FLECHA_IZQUIERDA 2424832
#define TECLADO_FLECHA_DERECHA 2555904

#include <vector>
#include <map>
#include <set>

#include <opencv2/opencv.hpp>

#include "elemento_diagrama.h"

const cv::Scalar BLANCO(255,255,255); //no se pueden constexpresear
const cv::Scalar GRIS(200,200,200);

/*Esta es de las variables más importantes del diagrama. Todos los "widgets" necesitan conocer su valor
 * para ser dibujados. Ejemplo:
 * arrowedLine(matriz, Point(400-desplazamientoOrigen.x,400-desplazamientoOrigen.y),
 * Point(500-desplazamientoOrigen.x,100-desplazamientoOrigen.y), Scalar(205,155,25), 2, CV_AA);
 * Esta sintaxis genera un efecto visual de desplazamiento sobre la matriz. Realmente los objetos tienen una posición absoluta,
 * pero los redibujamos con una posición relativa diferente (a la matriz de despliegue (región))
*/
cv::Point desplazamientoOrigen(0,0);
cv::Mat region(ALTURA_REGION, ANCHO_REGION, CV_8UC3, cv::Scalar(200,200,200)); //debe tener un scope global
cv::Mat menu(ALTURA_REGION, ANCHO_MENU, CV_8UC3, cv::Scalar(200,50,150));
cv::Mat diagrama_completo(ALTURA_REGION, ANCHO_REGION + ANCHO_MENU, CV_8UC3, cv::Scalar(150,150,150));


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

//la idea es que haya un vector de elemento_diagrama* y ya
std::vector<flecha> flechas;

//poner esto en un namespace global
