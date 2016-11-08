#ifndef MJOLNIRHPP
#define MJOLNIRHPP

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

#include "elemento_diagrama.h"

void renderizarDiagrama(cv::Mat& matriz);
void manejarInputTeclado(cv::Mat& matriz, int k);
void manejarInputMouse(int event, int x, int y, int flags, void*);

void establecer_resolucion(int& horizontal, int& vertical);
void inicializar_diagrama();

#endif // MJOLNIRHPP
