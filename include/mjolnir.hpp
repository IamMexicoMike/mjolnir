#ifndef MJOLNIRHPP
#define MJOLNIRHPP

#include "elemento_diagrama.h"

#include <vector>
#include <memory>
#include <atomic>

#include <opencv2/opencv.hpp>



class objeto;
class zona;
enum class Objetos;

class Mjolnir{
public:

  Mjolnir(const char* nombre):
    nombre_(nombre)
  {
    objetos.reserve(1024); //realmente no es necesario, pero evita memallocs en startup
  }

  const char* nombre_;
  cv::Mat diagrama_;
  cv::Mat encabezado_;
  cv::Scalar bckgnd_;

  int ancho_region;  //w = 2dx
  int altura_region; //h = 2dy
  cv::Point dxy;     // (w/2, h/2)

  const cv::Scalar COLOR_BLANCO{255,255,255};
  /*se ven feos*/
  const cv::Point HEADER0{5,20};
  const cv::Point HEADER1{100,20};
  const cv::Point HEADER2{300,20};
  cv::Point HEADER_MSG;

  cv::Point puntoClickMouseDerecho{0,0}; //panning
  cv::Point puntoInicioDesplazamiento{0,0}; //panning

  bool b_dibujando_flecha{false}; //flechas temporales
  cv::Point puntoInicioFlecha{(0,0)}; //flechas temporales
  cv::Point puntoTerminoFlecha{(0,0)}; //flechas temporales

  bool b_dibujando_objeto{false}; //objeto temporal
  cv::Point puntoOrigenobjeto{0,0}; //objeto temporal
  cv::Point puntoFinobjeto{0,0}; //objeto temporal

  cv::Point puntoActualMouse{0,0}; //se actualiza en cada evento del mouse

  cv::Point despl{(4000,1300)}; //originalmente desplazamientoOrigen

  objeto* ptr_seleccionado{nullptr};
  objeto* ptr_highlight{nullptr};

  bool b_drag{false};
  cv::Point ptInicioArrastre{0,0};
  cv::Point ptFinArrastre{0,0};

  int zoom{32};
  bool b_dibujar_nombres{false}; //es en funcion del zoom
  int tamanio_texto; //hay que considerar meter estos tres valores a 'objeto'
  int ancho_texto;

  std::string mensaje_derecho_superior;
  std::mutex mtx_mensaje_derecho_superior;
  std::mutex mtx_objetos_;

  std::atomic<bool> b_cache_valida{false};
  std::atomic<bool> b_puntos_relativos_validos{false};

  Objetos Tipo_Objeto_Dibujando; //para qué era esto? el tipo de objeto dibujando verdad?

  std::vector<std::unique_ptr<objeto>> objetos;
  std::vector<std::unique_ptr<objeto>> objetos_invisibles;
  std::vector<zona> superzonas;

  /**transforma los puntos absolutos a puntos relativos renderizables
  p' = f(p) = dx + (p - dx - d)/z*/
  cv::Point transformar(const cv::Point p);

  /**transforma un punto relativo a un punto absoluto
  p = g(p') = z*(p' - dx) + dx + d */
  cv::Point transformacion_inversa(const cv::Point pp);

  int transformar_escalar(int i);
  int transformar_escalar_inverso(int i);

  void rellenar_zona_telares(); //zonas
  void anexar_zonas();

  objeto* encontrar_ptr_area(cv::Point& p)
  {
    for(const auto& uptr : objetos)
      if(uptr->pertenece_a_area(p)) //si el punto cae dentro del área de un objeto...
        return uptr.get();
    return nullptr;
  };

  void iniciar_creacion_objeto(Objetos); //enum representado el tipo del objeto a crear
  void terminar_creacion_objeto();
  void dibujar_objeto_temporal();

  template <typename T> T* crear_objeto(T& t);
  template <typename T> void crear_objeto_delicado(std::unique_ptr<T>&& pt)
};

/** Crea un unique_ptr del objeto y se lo pasa al vector de apuntadores a objetos del diagrama*/
template <typename T>
T* Mjolnir::crear_objeto(T& t)
{
  std::string nombre_tipo = typeid(t).name();
  std::lock_guard<std::mutex> lck(mtx_objetos_);
  //std::string paq = "objeto " + nombre_tipo + " creado, con id==" + std::to_string(t.id());
  if(ptr_seleccionado != nullptr)
    ptr_seleccionado->seleccionar(false);
  std::unique_ptr<T> po = std::make_unique<T>(t);
  T* ptr = po.get();
  po->actualizar_pointers(); //los pointers a los miembros apuntaban a la stack si los actualizabas en el ctor
  objetos.emplace_back(std::move(po));
  ptr_highlight=ptr_seleccionado=nullptr;
  return ptr;
  //cout << paq << '\n';
  //empujar_queue_saliente(paq); //dentro de una función lockeada llamas a otra que usa locks. aguas
}

template <typename T>
void Mjolnir::crear_objeto_delicado(std::unique_ptr<T>&& pt)
{
  std::string nombre_tipo = typeid(*pt).name();
  std::lock_guard<std::mutex> lck(mtx_objetos_);
  std::string paq = "objeto " + nombre_tipo + " creado, con id==" + std::to_string(pt->id());
  if(ptr_seleccionado != nullptr)
    ptr_seleccionado->seleccionar(false);
  pt->actualizar_pointers(); //los pointers a los miembros apuntaban a la stack si los actualizabas en el ctor
  objetos.emplace_back(std::move(pt));
  ptr_highlight=nullptr;
  ptr_seleccionado=nullptr;
  //cout << paq << '\n';
  //empujar_queue_saliente(paq); //dentro de una función lockeada llamas a otra que usa locks. aguas
}

enum class Flags {Vacia, Objeto, SinCambios}; //no me convence
objeto* determinar_propiedades_ubicacion(cv::Point);

void efecto_cuadricula(cv::Mat& matriz);
void renderizarDiagrama(cv::Mat& matriz);
void manejarInputTeclado(int k);
void manejarInputMouse(int event, int x, int y, int flags, void*);
cv::Point operator/(cv::Point p, const int d);

#endif // MJOLNIRHPP
