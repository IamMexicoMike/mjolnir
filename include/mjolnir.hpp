#ifndef MJOLNIRHPP
#define MJOLNIRHPP

#include <vector>
#include <memory>

#include <opencv2/opencv.hpp>

class objeto;

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

void establecer_resolucion(int& horizontal, int& vertical);
void inicializar_diagrama();

#endif // MJOLNIRHPP
