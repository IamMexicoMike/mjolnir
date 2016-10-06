#define ANCHO_REGION 1000
#define ANCHO_MENU 200
#define ALTURA_REGION 600

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

#include "elemento_diagrama.h"

const cv::Scalar BLANCO(255,255,255); //no se pueden constexpresear
const cv::Scalar GRIS(200,200,200);
const cv::Scalar AZUL_PALIDO(240,200,200);

cv::Mat region(ALTURA_REGION, ANCHO_REGION, CV_8UC3, cv::Scalar(200,200,200)); //debe tener un scope global
cv::Mat mat_panel = region.colRange(region.cols - 100, region.cols);
cv::Mat mat_header = region.colRange(0, region.cols - 100).rowRange(0,30);

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

//la idea es que haya un vector de unique_ptr<objeto> y ya
std::vector<flecha> flechas;
