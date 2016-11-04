#ifndef MJOLNIRHPP
#define MJOLNIRHPP

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>

#include "elemento_diagrama.h"

void renderizarDiagrama(cv::Mat& matriz);
void manejarInputTeclado(cv::Mat& matriz, int k);
void manejarInputMouse(int event, int x, int y, int flags, void*);

//la idea es que haya un vector de unique_ptr<objeto> y ya
std::vector<flecha> flechas;

#endif // MJOLNIRHPP
