#include <iostream>
#include <chrono> //para profiles
#include <ctime> //para profiles
#include <fstream>
#include <algorithm>
#include <bitset> //borrame si no debuggeas
#include <future>
#include <thread>

#include <opencv2/opencv.hpp>

#include "datos.hpp"
#include "elemento_diagrama.h"
#include "utilidades.hpp"
#include "redes.h"
#include "gui.h"

using namespace std;
using namespace cv;

/* La forma más rápida de fracasar es no intentarlo */
/* Querer es poder */
/* El que busca encuentra */

//demasiados magic numbers
inline void renderizarDiagrama(Mat& matriz) //No hay pedo si tratamos de dibujar una región que no pertenece a matriz
{
    std::chrono::time_point<std::chrono::system_clock> t_inicial, t_final; //empezamos a medir tiempo
    t_inicial = std::chrono::system_clock::now();

    /**Esto es para el "efecto cuadrícula", que simula una matriz infinita*/
    matriz = GRIS;
    for(int i=15-(glb::desplazamientoOrigen.x % 15); i<matriz.cols; i+=15) //"generamos" un efecto de desplazamiento de la cuadrícula
        line(matriz, Point(i,0), Point(i,matriz.rows), BLANCO, 1, 4, 0); //cuadrícula, vertical
    for(int i=15-(glb::desplazamientoOrigen.y % 15); i<matriz.rows; i+=15) //Mat::cols es int, no uint
        line(matriz, Point(0,i), Point(matriz.cols,i), BLANCO, 1, 4, 0); //cuadrícula, horizontal

    if(b_dibujando_flecha) //dibujamos una flecha temporal
        arrowedLine(matriz, Point(puntoInicioFlecha.x - glb::desplazamientoOrigen.x, puntoInicioFlecha.y - glb::desplazamientoOrigen.y),
                    Point(puntoTerminoFlecha.x - glb::desplazamientoOrigen.x,puntoTerminoFlecha.y - glb::desplazamientoOrigen.y),
                    COLOR_FLECHA_DIBUJANDO, 2, CV_AA);

    if(b_dibujando_objeto) //dibujamos un rectángulo temporal
    {
        rectangle(matriz, Rect(puntoOrigenobjeto - glb::desplazamientoOrigen, puntoFinobjeto - glb::desplazamientoOrigen),
                  COLOR_RECT_DIBUJANDO, 2, CV_AA);
    }


    //dibujamos todas als flechas - actualmente no hace nada
    //for_each(flechas.begin(), flechas.end(), [&](flecha& f) {f.dibujarse(matriz, glb::desplazamientoOrigen);});

    //dibujamos todos los objetos
    for(auto& rec : glb::objetos)
        rec.second.dibujarse(matriz, glb::desplazamientoOrigen);

    //dibujamos todas las relaciones
    for(auto& rel : glb::relaciones)
        rel.second.dibujarse(matriz, glb::desplazamientoOrigen);

    mat_panel = AZUL_PALIDO;
    mat_header = BLANCO;

    if(glb::llave_objeto_seleccionado > 0)
        putText(matriz, std::string("Seleccionado: " + std::to_string(glb::llave_objeto_seleccionado)),
            header::HEADER1, FONT_HERSHEY_PLAIN, 1, Scalar(230,100,0));


    // medimos tiempo
    t_final = std::chrono::system_clock::now();
    std::chrono::duration<double> t_renderizar = t_final - t_inicial;
    //cout << t_renderizar.count() << "s\n";

    imshow("Mjolnir", matriz); //actualizamos el diagrama
}

inline void manejarInputTeclado(Mat& matriz, int k) //k no incluye ni ctrl, ni shift, ni alt por mencionar algunas
{
    cout << k << "!\n"; //borrame si no debugeas, o coméntame mejor!

    constexpr int DESPLAZAMIENTO = 5;
    constexpr int TECLADO_FLECHA_ARRIBA = 2490368;
    constexpr int TECLADO_FLECHA_ABAJO = 2621440;
    constexpr int TECLADO_FLECHA_IZQUIERDA = 2424832;
    constexpr int TECLADO_FLECHA_DERECHA = 2555904;

    switch (k) {
    case TECLADO_FLECHA_ARRIBA:
        glb::desplazamientoOrigen.y -= DESPLAZAMIENTO;
        break;
    case TECLADO_FLECHA_ABAJO:
        glb::desplazamientoOrigen.y += DESPLAZAMIENTO;
        break;
    case TECLADO_FLECHA_DERECHA:
        glb::desplazamientoOrigen.x += DESPLAZAMIENTO;
        break;
    case TECLADO_FLECHA_IZQUIERDA:
        glb::desplazamientoOrigen.x -= DESPLAZAMIENTO;
        break;
    case 114: //r (ojo, R tiene su propia clave
        puntoOrigenobjeto = puntoActualMouse + glb::desplazamientoOrigen;
        puntoFinobjeto = puntoOrigenobjeto;
        b_dibujando_objeto = true;
        break;
    case 13: //tecla enter
        if(b_dibujando_objeto)
        {
            b_dibujando_objeto = false;
            glb::objetos.emplace(objeto::sid - 1, objeto(puntoOrigenobjeto, puntoFinobjeto));
        }
        break;
    case 103: //g de guardar
        //fstream fs("diagrama") //necesitas definir el operador << para tus clases
        break;
    case 50: //debug
        cout << "valor global: " << glb::llave_objeto_highlight << endl;
        //push_funptr(&foobar);
        system("Pause");
        break;
    case 100: //d de debug
        cout << "obj sel: " << glb::llave_objeto_seleccionado << " obj hgl: " << glb::llave_objeto_highlight << endl;
        for(auto& ob : glb::objetos)
            cout << ob.first << "," << ob.second.id() << " | " << endl;
        for(auto& rel : glb::relaciones)
            cout << rel.first << "," << rel.second.id() << " | " << endl;
        cout << "\ntodas las relaciones:" << endl;
        for(auto& rel : glb::relaciones)
            cout << "relacion " << rel.second.id() << ": "
                 << rel.second.get_objetos().first << ',' << rel.second.get_objetos().second << endl;
        if(glb::llave_objeto_seleccionado > 0)
        {
            cout << "relaciones del objeto " << glb::llave_objeto_seleccionado << ":" << endl;
            for(auto id : glb::objetos.at(glb::llave_objeto_seleccionado).get_relaciones())
                cout << id << endl;
        }
        break;

    case 43: //+ ensanchamos el diagrama
        if(region.cols < 2000)
        {
            region = cv::Mat(region.rows, region.cols+15, CV_8UC3, cv::Scalar(200,200,200)); //debe tener un scope global
            mat_panel = region.colRange(region.cols - 100, region.cols);
            mat_header = region.colRange(0, region.cols - 100).rowRange(0,30);
        }
        break;

    case 45: //- adelgazamos el diagrama
        if(region.cols > 200)
        {
            region = cv::Mat(region.rows, region.cols-15, CV_8UC3, cv::Scalar(200,200,200)); //debe tener un scope global
            mat_panel = region.colRange(region.cols - 100, region.cols);
            mat_header = region.colRange(0, region.cols - 100).rowRange(0,30);
        }
        break;

    case 3014656: //suprimir, borrar objeto
        if(glb::llave_objeto_seleccionado > 0)
        {
            glb::objetos.erase(glb::llave_objeto_seleccionado);
            glb::llave_objeto_seleccionado = -1;
            glb::llave_objeto_highlight = -1;
            ubicacion::determinar_propiedades_ubicacion(puntoActualMouse); //para actualizar highlight
        }
        break;

    }
    //cout << glb::desplazamientoOrigen << endl;
}

/**Este callback se invoca cada vez que hay un evento de mouse en la ventana a la cual se attacheó el callback.
  Podrías tener varias ventanas con diferentes funciones que manejen el mouse
  La lógica de este callback debe estar encapsulada, y debe ser legible*/
inline void manejarInputMouse(int event, int x, int y, int flags, void*)
{
    puntoActualMouse = cv::Point(x,y); //esta variable siempre lleva el rastro de dónde está el mouse
    //cout << (flags & CV_EVENT_FLAG_CTRLKEY) << endl;

    if(event == CV_EVENT_RBUTTONDOWN) //no necesita propiedades_ubicacion, es para panning
    {
        botonMouseDerechoAbajo = true;
        puntoClickMouseDerecho = puntoActualMouse;
        puntoInicioDesplazamiento = glb::desplazamientoOrigen;
    }

    if(event == CV_EVENT_RBUTTONUP)
        botonMouseDerechoAbajo = false;

    if(event == CV_EVENT_LBUTTONDOWN)
    {
        /*Si estábamos dibujando un objeto y dimos click, lo insertamos y no hacemos nada más*/
        if(b_dibujando_objeto)
        {
            b_dibujando_objeto = false;
            glb::objetos.emplace(objeto::sid - 1, objeto(puntoOrigenobjeto, puntoFinobjeto)); //porque -1?

            /*solicitamos las propiedades del nuevo objeto a crear*/
            //...
        }

        /*no estábamos dibujando un objeto, evaluamos el punto y establecemos condiciones para la selección y el arrastre*/
        else
        {
            botonMouseIzquierdoAbajo = true;

            /*En este caso, siempre consultamos las propiedades de la ubicación*/
            auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + glb::desplazamientoOrigen);

            //checamos si el punto actual coincide con un objeto. Si sí, lo seleccionamos.
            if(props.first > 0 ) //props.first es el id
            {
                if(glb::llave_objeto_seleccionado > 0) //si había otro brother seleccionado antes...
                    glb::objetos.at(glb::llave_objeto_seleccionado).seleccionar(false); //des-seleccionamos al anterior
                glb::objetos.at(props.first).seleccionar(true); //seleccionamos al brother
                glb::llave_objeto_seleccionado = props.first; //actualizamos al seleccionado

                if(flags & CV_EVENT_FLAG_CTRLKEY) //vamos a dibujar flecha, no a arrastrar
                {
                    b_dibujando_flecha = true; //añadir condición
                    puntoInicioFlecha = puntoActualMouse + glb::desplazamientoOrigen; //añadir condición
                    puntoTerminoFlecha = puntoInicioFlecha; //"reseteamos" la flecha;
                }
                else //de lo contrario, arrastramos
                {
                    glb::b_drag = true; //condiciones de arrastre habilitadas
                    glb::ptInicioArrastre = puntoActualMouse + glb::desplazamientoOrigen;
                    glb::ptFinArrastre = glb::ptInicioArrastre;
                    b_dibujando_flecha = false;
                }
                //hay espacio para alt y shift. Afortunadamente drag y dibujar flecha son mutuamente excluyentes
            }

            else if(glb::llave_objeto_seleccionado > 0) //no caimos en nadie, pero había un brother seleccionado
            {
                glb::objetos.at(glb::llave_objeto_seleccionado).seleccionar(false); //lo des-seleccionamos
                glb::llave_objeto_seleccionado=-1; //y reseteamos el id de selección
            }

        }
    }

    if(event == CV_EVENT_LBUTTONUP)
    {
        botonMouseIzquierdoAbajo = false;
        glb::b_drag = false; //terminan las condiciones de arrastre

        if(b_dibujando_flecha) //esto se va a revampear
        {
            //flechas.push_back(flecha(puntoInicioFlecha, cv::Point(x,y) + glb::desplazamientoOrigen));
            auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + glb::desplazamientoOrigen);
            if(props.first > 0 && props.first != glb::llave_objeto_seleccionado)
                glb::relaciones.emplace(relacion::sid - 1, relacion(glb::llave_objeto_seleccionado, props.first));

            b_dibujando_flecha = false;
        }
    }

    if(event == CV_EVENT_MOUSEMOVE)
    {
        //No es correcto llamar propiedades_ubicacion para todo movimiento. e.g. cuando haces panning no lo necesitas.

        if(botonMouseDerechoAbajo) //Panning. Moviéndonos con click derecho apretado
        {
            //no necesitamos propiedades ubicacion
            glb::desplazamientoOrigen.x = puntoInicioDesplazamiento.x + puntoClickMouseDerecho.x - x;
            glb::desplazamientoOrigen.y = puntoInicioDesplazamiento.y + puntoClickMouseDerecho.y - y;
        }

        if(botonMouseIzquierdoAbajo) //Flechas. Dragging. Moviendo el cursor con click izquierdo apretado.
        {
            //propiedades ubicacion, highlightear destino de flecha, posible drag n drop
            if(glb::b_drag)
            {
                cv::Point pt = puntoActualMouse + glb::desplazamientoOrigen;
                cv::Point dif = pt - glb::ptInicioArrastre;
                glb::objetos.at(glb::llave_objeto_seleccionado).arrastrar(dif);
                glb::ptInicioArrastre = pt;
                //el vector de arrasre el ptFinArrastre - ptInicioArrastre
            }
            //...

            if(b_dibujando_flecha)  //dibujando flecha temporal
            {
                /*props se va a usar después para tener feedback con el highlight*/
                auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + glb::desplazamientoOrigen); //para highlightear el destino

                puntoTerminoFlecha = puntoActualMouse + glb::desplazamientoOrigen; //la flecha es temporal, no se añade sino hasta que LBUTTONUP
            }
        }

        if(!botonMouseDerechoAbajo && !botonMouseIzquierdoAbajo) //estamos chillin'
        {
            auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + glb::desplazamientoOrigen);
        }

        if(b_dibujando_objeto)
        {
            puntoFinobjeto = cv::Point(x,y) + glb::desplazamientoOrigen;
        }

    } //CV_EVENT_MOUSEMOVE

} //manejarInputMouse

/**Necesitamos tres o más hilos:
1) Diagrama
2) Ciclo de eventos de asio (networking)
3) Ciclo de eventos de la gui
n) Más diagramas*/
int main()
{
  //pruebas...

    namedWindow("Mjolnir");
    setMouseCallback("Mjolnir", manejarInputMouse);
    //std::thread t1(redes_main);
    std::thread t2(main_gui);
    renderizarDiagrama(region);

    while (true)
    {
      /**Cada 30ms verificamos buffers y actualizamos la UI de acuerdo*/
        int k = waitKey(15);
        if(k == 27)
        {
          destroyWindow("Mjolnir");
          break;
        }

        if(k != -1) //-1 se retorna cuando el timer expira o cuando se cierra la ventana
            manejarInputTeclado(region, k);

        /*...*/

        /*añadir código aquí para actualizar memoria*/
      renderizarDiagrama(region);
      setMouseCallback("Mjolnir", manejarInputMouse);
    }

    //t1.join();
    t2.join();
    return 0;
}
