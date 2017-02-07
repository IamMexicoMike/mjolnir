#ifndef MJOLNIRHPP
#define MJOLNIRHPP

#include <vector>
#include <memory>

#include <opencv2/opencv.hpp>
#include <windows.h>
#include <windowsx.h>

class objeto;

extern cv::Mat region;
extern cv::Mat mat_header;

extern int ancho_region; //w = 2dx
extern int altura_region; //h = 2dy
extern cv::Point dxy; // (w/2, h/2)

/*se ven feos*/
const cv::Point HEADER0(5,20);
const cv::Point HEADER1(100,20);
const cv::Point HEADER2(300,20);
extern cv::Point HEADER_MSG;

extern std::vector<std::unique_ptr<objeto>>::iterator itr_seleccionado;
extern std::vector<std::unique_ptr<objeto>>::iterator itr_highlight;
extern bool b_drag;
extern bool b_resize;
extern cv::Point ptInicioArrastre;
extern cv::Point ptFinArrastre;
extern std::vector<std::unique_ptr<objeto>> objetos;

enum class Flags {Vacia, Objeto, SinCambios}; //no me convence
std::vector<std::unique_ptr<objeto>>::iterator determinar_propiedades_ubicacion(cv::Point);

void renderizarDiagrama(cv::Mat& matriz);
void manejarInputTeclado(cv::Mat& matriz, int k);
void manejarInputMouse(int event, int x, int y, int flags, void*);

extern cv::Point operator/(cv::Point p, const int d);
extern cv::Point transformar(const cv::Point p);
extern cv::Point transformacion_inversa(const cv::Point pp);

extern void rellenar_zona_telares(); //zonas
extern void anexar_zonas();

/*****************************/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void matrizAControl(HWND& hwnd, cv::Mat& img);


#endif // MJOLNIRHPP
