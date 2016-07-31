#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <bitset> //borrame si no debuggeas

#include <opencv2/opencv.hpp>

#include "datos.hpp"
#include "elemento_diagrama.h"
#include "utilidades.hpp"

using namespace std;
using namespace cv;

/*Te quedaste en lo siguiente: Definiste un nuevo constructor para flechas, a partir de rectángulos
Falta determinar cómo los rectángulos avisarán a las flechas suscritas a ellos que cambien sus coordenadas
venga mike*/

inline void renderizarDiagrama(cv::Mat& matriz); //prototipo


inline void renderizarDiagrama(Mat& matriz) //No hay pedo si tratamos de dibujar una región que no pertenece a matriz
{
    std::chrono::time_point<std::chrono::system_clock> t_inicial, t_final; //empezamos a medir tiempo
    t_inicial = std::chrono::system_clock::now();

    matriz = Mat(ALTURA_REGION, ANCHO_REGION, CV_8UC3, GRIS); //inicializamos la matriz que renderizará al color gris
    for(int i=15-(desplazamientoOrigen.x % 15); i<matriz.cols; i+=15) //"generamos" un efecto de desplazamiento de la cuadrícula
        line(matriz, Point(i,0), Point(i,matriz.rows), BLANCO, 1, 4, 0); //cuadrícula, vertical
    for(int i=15-(desplazamientoOrigen.y % 15); i<matriz.rows; i+=15) //Mat::cols es int, no uint
        line(matriz, Point(0,i), Point(matriz.cols,i), BLANCO, 1, 4, 0); //cuadrícula, horizontal

    if(b_dibujando_flecha) //dibujamos una flecha temporal
        arrowedLine(matriz, Point(puntoInicioFlecha.x - desplazamientoOrigen.x, puntoInicioFlecha.y - desplazamientoOrigen.y),
                    Point(puntoTerminoFlecha.x - desplazamientoOrigen.x,puntoTerminoFlecha.y - desplazamientoOrigen.y),
                    COLOR_FLECHA_DIBUJANDO, 2, CV_AA);

    if(b_dibujando_circulo) //dibujamos un circulo temporal
        ;

    if(b_dibujando_rectangulo) //dibujamos un rectángulo temporal
    {
        rectangle(matriz, Rect(puntoOrigenRectangulo - desplazamientoOrigen, puntoFinRectangulo - desplazamientoOrigen),
                  COLOR_RECT_DIBUJANDO, 2, CV_AA);
    }


    //dibujamos todas als flechas
    for_each(flechas.begin(), flechas.end(), [&](flecha& f) {f.dibujarse(matriz, desplazamientoOrigen);});

    //dibujamos todos los rectángulos
    for(auto& rec : rectangulos)
        rec.second.dibujarse(matriz, desplazamientoOrigen);

    // medimos tiempo
    t_final = std::chrono::system_clock::now();
    std::chrono::duration<double> t_renderizar = t_final - t_inicial;
    //cout << t_renderizar.count() << "s\n";

    imshow("Diagrama", matriz); //actualizamos el diagrama
}

inline void manejarInputTeclado(Mat& matriz, int k) //k no incluye ni ctrl, ni shift, ni alt por mencionar algunas
{
    cout << k << "!\n"; //borrame si no debugeas, o coméntame mejor!

    bool b_renderizar = false;

    constexpr unsigned DESPLAZAMIENTO = 5; //sospechoso contexpr. Lo estás haciendo bien?
    switch (k) {
    case TECLADO_FLECHA_ARRIBA:
        desplazamientoOrigen.y -= DESPLAZAMIENTO;
        b_renderizar = true;
        break;
    case TECLADO_FLECHA_ABAJO:
        desplazamientoOrigen.y += DESPLAZAMIENTO;
        b_renderizar = true;
        break;
    case TECLADO_FLECHA_DERECHA:
        desplazamientoOrigen.x += DESPLAZAMIENTO;
        b_renderizar = true;
        break;
    case TECLADO_FLECHA_IZQUIERDA:
        desplazamientoOrigen.x -= DESPLAZAMIENTO;
        b_renderizar = true;
        break;
    case 114: //r (ojo, R tiene su propia clave
        puntoOrigenRectangulo = puntoActualMouse + desplazamientoOrigen;
        b_dibujando_rectangulo = true;
        break;
    case 13: //tecla enter
        if(b_dibujando_rectangulo)
        {
            b_dibujando_rectangulo = false;
            rectangulos.insert({rectangulo::consecutivo(), rectangulo(puntoOrigenRectangulo, puntoFinRectangulo)});
            b_renderizar = true;
        }
        break;
    case 103: //g de guardar
        //fstream fs("diagrama") //necesitas definir el operador << para tus clases
        break;
    case 50: //debug, 3
        cout << "valor global: " << global::llave_rectangulo_highlight << endl;
        break;
    case 3014656: //supr
        if(global::llave_rectangulo_seleccionado > 0)
        {
            rectangulos.erase(global::llave_rectangulo_seleccionado);
            global::llave_rectangulo_seleccionado = -1;
            global::llave_rectangulo_highlight = -1;
            ubicacion::determinar_propiedades_ubicacion(puntoActualMouse, flechas, rectangulos); //para actualizar highlight
            b_renderizar = true;
        }
        break;


    }
    //cout << desplazamientoOrigen << endl;

    if(b_renderizar)
        renderizarDiagrama(matriz);
}

inline void manejarInputMouse(int event, int x, int y, int flags, void*)
{
    bool b_renderizar = false;
    puntoActualMouse = cv::Point(x,y); //esta variable siempre lleva el rastro de dónde está el mouse
    cout << (flags & CV_EVENT_FLAG_CTRLKEY) << endl;

    if(event == CV_EVENT_RBUTTONDOWN) //no necesita propiedades_ubicacion, es para panning
    {
        botonMouseDerechoAbajo = true;
        puntoClickMouseDerecho = puntoActualMouse;
        puntoInicioDesplazamiento = desplazamientoOrigen;
    }

    if(event == CV_EVENT_RBUTTONUP)
        botonMouseDerechoAbajo = false;

    if(event == CV_EVENT_LBUTTONDOWN) //te falta usar SHIFT y CTRL
    {
        b_renderizar = true; //entramos aquí relativamente pocas veces por lo que no es costoso

        /*Si estábamos dibujando un rectángulo y dimos click, lo insertamos y no hacemos nada más*/
        if(b_dibujando_rectangulo)
        {
            b_dibujando_rectangulo = false;
            rectangulos.insert({rectangulo::consecutivo(), rectangulo(puntoOrigenRectangulo, puntoFinRectangulo)});
            b_renderizar = true;
        }

        //de lo contrario, evaluamos el punto y establecemos condiciones para la selección y el arrastre
        else
        {
            botonMouseIzquierdoAbajo = true;

            /*En este caso, siempre consultamos las propiedades de la ubicación*/
            auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + desplazamientoOrigen,
                                                                     flechas, rectangulos);

            //checamos si el punto actual coincide con un objeto. Si sí, lo seleccionamos.
            if(props.first > 0 )
            {
                if(global::llave_rectangulo_seleccionado > 0) //si había otro brother seleccionado antes
                    rectangulos.at(global::llave_rectangulo_seleccionado).seleccionar(false); //des-seleccionamos al anterior
                rectangulos.at(props.first).seleccionar(true); //seleccionamos al brother
                global::llave_rectangulo_seleccionado = props.first; //actualizamos al seleccionado

                if(flags & CV_EVENT_FLAG_CTRLKEY) //vamos a dibujar flecha, no a arrastrar
                {
                    b_dibujando_flecha = true; //añadir condición
                    puntoInicioFlecha = puntoActualMouse + desplazamientoOrigen; //añadir condición
                    puntoTerminoFlecha = puntoInicioFlecha; //"reseteamos" la flecha;
                }
                else //de lo contrario, arrastramos
                {
                    global::b_drag = true; //condiciones de arrastre habilitadas
                    global::ptInicioArrastre = puntoActualMouse + desplazamientoOrigen;
                    global::ptFinArrastre = global::ptInicioArrastre;
                    b_dibujando_flecha = false;
                }
                //hay espacio para alt y shift. Afortunadamente drag y dibujar flecha son mutuamente excluyentes
            }

            else if(global::llave_rectangulo_seleccionado > 0) //no caimos en nadie, pero había un brother seleccionado
            {
                rectangulos.at(global::llave_rectangulo_seleccionado).seleccionar(false); //lo des-seleccionamos
                global::llave_rectangulo_seleccionado=-1; //y reseteamos el id de selección
            }

        }
    }

    if(event == CV_EVENT_LBUTTONUP)
    {
        botonMouseIzquierdoAbajo = false;
        global::b_drag = false; //terminan las condiciones de arrastre

        if(b_dibujando_flecha) //esto se va a revampear
        {
            flechas.push_back(flecha(puntoInicioFlecha, cv::Point(x,y) + desplazamientoOrigen));
            b_dibujando_flecha = false;
            b_renderizar = true;
        }
    }

    if(event == CV_EVENT_MOUSEMOVE)
    {
        //No es correcto llamar propiedades_ubicacion para todo movimiento. e.g. cuando haces panning no lo necesitas.

        if(botonMouseDerechoAbajo) //Panning. Moviéndonos con click derecho apretado
        {
            //no necesitamos propiedades ubicacion
            desplazamientoOrigen.x = puntoInicioDesplazamiento.x + puntoClickMouseDerecho.x - x;
            desplazamientoOrigen.y = puntoInicioDesplazamiento.y + puntoClickMouseDerecho.y - y;
            b_renderizar = true;
        }

        if(botonMouseIzquierdoAbajo) //Flechas. Dragging. Moviendo el cursor con click izquierdo apretado.
        {
            //propiedades ubicacion, highlightear destino de flecha, posible drag n drop
            if(global::b_drag)
            {
                cv::Point pt = puntoActualMouse + desplazamientoOrigen;
                cv::Point dif = pt - global::ptInicioArrastre;
                rectangulos.at(global::llave_rectangulo_seleccionado).arrastrar(dif);
                global::ptInicioArrastre = pt;
                //el vector de arrasre el ptFinArrastre - ptInicioArrastre

                b_renderizar = true;
            }
            //...

            if(b_dibujando_flecha)  //dibujando flecha
            {
                auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + desplazamientoOrigen,
                                                                     flechas, rectangulos); //para highlightear el destino

                puntoTerminoFlecha = puntoActualMouse + desplazamientoOrigen; //la flecha es temporal, no se añade sino hasta que LBUTTONUP
                b_renderizar = true;
            }
        }

        if(!botonMouseDerechoAbajo && !botonMouseIzquierdoAbajo) //estamos chillin'
        {
            ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + desplazamientoOrigen,
                                                                     flechas, rectangulos);
            b_renderizar = true;
        }

        if(b_dibujando_rectangulo)
        {
            puntoFinRectangulo = cv::Point(x,y) + desplazamientoOrigen;
            b_renderizar = true;
        }

    } //CV_EVENT_MOUSEMOVE

    if(b_renderizar)
        renderizarDiagrama(region);

} //manejarInputMouse

int main()
{
    //diagrama_completo.colRange(0, region.cols) = region.colRange(0, region.cols);
    namedWindow("Diagrama");
    setMouseCallback("Diagrama", manejarInputMouse);
    renderizarDiagrama(region);

    while (true)
    {
        int k = waitKey(0); //nota que comparamos unsigned con signed, y funciona. Hmm
        if(-1 == k) //de este modo, si cierras la ventana efectivamente sales del ciclo
            break;
        manejarInputTeclado(region, k);
    }

    return 0;
}
