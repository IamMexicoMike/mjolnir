#ifndef MJOLNIRHPP
#define MJOLNIRHPP


#define WIN32_LEAN_AND_MEAN

#include "windows.h"
#include "elemento_diagrama.h"
#include "zonas.hpp"

#include <vector>
#include <memory>
#include <atomic>

#include <opencv2/opencv.hpp>

class ventana;
class objeto;
class zona;
enum class Objetos;

const cv::Scalar COLOR_BLANCO{255,255,255};

class Mjolnir : public std::enable_shared_from_this<Mjolnir>
{
public:

  Mjolnir(const char* nombre, ventana* padre_):
    nombre_(nombre)
  {
    objetos.reserve(1024); //realmente no es necesario, pero evita memallocs en startup
  }

  inline void iniciar_callbacks()
  {
    cv::setMouseCallback(nombre_, Mjolnir::callback_dispatcher_mouse, this);
    cv::setKeyboardCallback(nombre_, Mjolnir::callback_dispatcher_tecl, this);
  }

  inline static void callback_dispatcher_tecl(int k, void* data)
  {
    Mjolnir* m=static_cast<Mjolnir*>(data);
    m->manejarInputTeclado(k);
  }

  inline static void callback_dispatcher_mouse(int event, int x, int y, int flags, void* data)
  {
    Mjolnir* m=static_cast<Mjolnir*>(data);
    m->manejarInputMouse(event, x, y, flags);
  }

  const char* nombre_;
  ventana* padre_;

  cv::Mat diagrama_;
  cv::Mat encabezado_;
  cv::Scalar bckgnd_;

  int ancho_region;  //w = 2dx
  int altura_region; //h = 2dy
  cv::Point dxy;     // (w/2, h/2)

  /*se ven feos*/
  const cv::Point HEADER0{5,20};
  const cv::Point HEADER1{100,20};
  const cv::Point HEADER2{300,20};
  cv::Point HEADER_MSG;

  cv::Point puntoClickMouseDerecho{0,0}; //panning
  cv::Point puntoInicioDesplazamiento{0,0}; //panning

  bool b_dibujando_flecha{false}; //flechas temporales
  cv::Point puntoInicioFlecha{0,0}; //flechas temporales
  cv::Point puntoTerminoFlecha{0,0}; //flechas temporales

  bool b_dibujando_objeto{false}; //objeto temporal
  cv::Point puntoOrigenobjeto{0,0}; //objeto temporal
  cv::Point puntoFinobjeto{0,0}; //objeto temporal

  cv::Point puntoActualMouse{0,0}; //se actualiza en cada evento del mouse

  cv::Point despl{4000,1300}; //originalmente desplazamientoOrigen

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
  //std::mutex mtx_objetos_;

  //std::atomic<bool> b_cache_valida{false};
  bool b_cache_valida{false};
  //std::atomic<bool> b_puntos_relativos_validos{false};
  bool b_puntos_relativos_validos{false};

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

  objeto* encontrar_ptr_area(cv::Point& p)
  {
    for(const auto& uptr : objetos)
    {
      if(uptr==nullptr)
        return nullptr;
      if(uptr->pertenece_a_area(p)) //si el punto cae dentro del área de un objeto...
        return uptr.get();
    }

    return nullptr;
  };

  void iniciar_creacion_objeto(Objetos); //enum representado el tipo del objeto a crear
  void terminar_creacion_objeto();
  void dibujar_objeto_temporal();

  template <typename T> T* crear_objeto(T& t);
  template <typename T> void crear_objeto_delicado(std::unique_ptr<T>&& pt);
  void destruir_objeto_seleccionado();
  void destruir_objeto(const int id);
  //template<typename T> T* crear_sincronizado(T& t);
  void ordenar_objetos();
  void simulacion();
  objeto* determinar_propiedades_ubicacion(cv::Point);
  std::string obtener_mensaje();
  void establecer_mensaje(std::string m);
  void efecto_cuadricula();
  void renderizarDiagrama();
  void manejarInputTeclado(int k);
  void manejarInputMouse(int event, int x, int y, int flags);
  void crear_dialogo_objeto(objeto* pobj);
  void mensaje(std::string msg, std::string titulo);
  HWND padre() const;
};

/** Crea un unique_ptr del objeto y se lo pasa al vector de apuntadores a objetos del diagrama*/
template <typename T>
T* Mjolnir::crear_objeto(T& t)
{
  std::string nombre_tipo = typeid(t).name();
  //std::lock_guard<std::mutex> lck(mtx_objetos_);
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
  //std::lock_guard<std::mutex> lck(mtx_objetos_);
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

/*
template<typename T>
T* Mjolnir::crear_sincronizado(T& t)
{
  std::string nombre_tipo = typeid(t).name();
  std::lock_guard<std::mutex> lck(mtx_objetos);
  std::string paq = "objeto syncronizado " + nombre_tipo + " creado, con id==" + std::to_string(t.id());
  if(ptr_seleccionado != nullptr)
    ptr_seleccionado->seleccionar(false);
  std::unique_ptr<T> po = std::make_unique<T>(t);
  T* ptr = po.get();
  po->actualizar_pointers(); //los pointers a los miembros apuntaban a la stack si los actualizabas en el ctor
  std::pair<std::string,int> tabla_y_id(T::nombreclase, po->id() );
  sync::objetos_sincronizados[tabla_y_id]=ptr;
  objetos.emplace_back(std::move(po));
  ptr_highlight=ptr_seleccionado=nullptr;
  return ptr;
}*/

enum class Flags {Vacia, Objeto, SinCambios}; //no me convence

cv::Point operator/(cv::Point p, const int d);

#endif // MJOLNIRHPP
