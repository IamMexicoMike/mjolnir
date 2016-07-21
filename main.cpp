#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "datos.hpp"
#include "elemento_diagrama.h"

using namespace std;
using namespace cv;

int determinarPropiedadesUbicacion(const cv::Point p) //Sólo puede haber un objeto activo a la vez
{

    //considerar std::all_of, std::any_of, std::none_of en lugar de for_each
    //así como std::find, std::find_if, std::find_if_not
    for_each(flechas.begin(), flechas.end(), [&](flecha& f)
    {
        //hacer algo
    });

    for_each(rectangulos.begin(), rectangulos.end(), [&](rectangulo& r)
    {
        // hacer algo

    });

    vector<rectangulo>::iterator itr = find_if(rectangulos.begin(), rectangulos.end(), [&p](rectangulo& r)
    {
         return r.pertenece_a_area(p);
    });
    if(itr != rectangulos.end()) //también podemos usar std::begin(rectangulos)
        itr->seleccionar();

}

inline void renderizarDiagrama(Mat& matriz) //No hay pedo si tratamos de dibujar una región que no pertenece a matriz
{
    std::chrono::time_point<std::chrono::system_clock> t_inicial, t_final;
    t_inicial = std::chrono::system_clock::now();

    matriz = Mat(ALTURA_REGION, ANCHO_REGION, CV_8UC3, GRIS);
    for(int i=15-(desplazamientoOrigen.x % 15); i<matriz.cols; i+=15) //"generamos" un efecto de desplazamiento de la cuadrícula
        line(matriz, Point(i,0), Point(i,matriz.rows), BLANCO, 1, 4, 0); //cuadrícula, vertical
    for(int i=15-(desplazamientoOrigen.y % 15); i<matriz.rows; i+=15) //Mat::cols es int, no uint
        line(matriz, Point(0,i), Point(matriz.cols,i), BLANCO, 1, 4, 0); //cuadrícula, horizontal

    if(b_dibujando_flecha) //dibujamos una flecha temporal
        arrowedLine(matriz, Point(puntoInicioFlecha.x - desplazamientoOrigen.x, puntoInicioFlecha.y - desplazamientoOrigen.y),
                    Point(puntoTerminoFlecha.x - desplazamientoOrigen.x,puntoTerminoFlecha.y - desplazamientoOrigen.y),
                    Scalar(105,205,25), 2, CV_AA);

    if(b_dibujando_circulo) //dibujamos un circulo temporal
        ;

    if(b_dibujando_rectangulo) //dibujamos un rectángulo temporal
    {
        rectangle(matriz, Rect(puntoOrigenRectangulo - desplazamientoOrigen, puntoFinRectangulo - desplazamientoOrigen),
                  cv::Scalar(100, 65, 150), 2, CV_AA);
    }


    //dibujamos todas als flechas
    for_each(flechas.begin(), flechas.end(), [&](flecha& f) {f.dibujarse(matriz, desplazamientoOrigen);});
    for_each(rectangulos.begin(), rectangulos.end(), [&](rectangulo& r) {r.dibujarse(matriz, desplazamientoOrigen);});

    t_final = std::chrono::system_clock::now();
    std::chrono::duration<double> t_renderizar = t_final - t_inicial;
    cout << t_renderizar.count() << "s\n";

    imshow("Diagrama", matriz);
    //diagrama_completo.colRange(0, region.cols) = region.colRange(0, region.cols);
    //diagrama_completo = region;
    //imshow("Diagrama Completo", diagrama_completo);
} //actualizamos el diagrama

inline void manejarInputTeclado(Mat& matriz, int k) //k no incluye ni ctrl, ni shift, ni alt por mencionar algunas
{
    cout << k << "!\n";

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
            rectangulos.push_back(rectangulo(puntoOrigenRectangulo, puntoFinRectangulo));
            b_renderizar = true;
        }
        break;
    case 103: //g de guardar
        //fstream fs("diagrama") //necesitas definir el operador << para tus clases
        break;

    }
    //cout << desplazamientoOrigen << endl;

    if(b_renderizar)
        renderizarDiagrama(matriz);
}

inline void manejarInputMouse(int event, int x, int y, int, void*)
{
    bool b_renderizar = false;
    //cout << x <<","<< y << " evento: " << event << endl;
    puntoActualMouse = cv::Point(x,y); //esta variable es para dibujar figuras en callbacks del teclado

    if(event == CV_EVENT_RBUTTONDOWN)
    {
        botonMouseDerechoAbajo = true;
        puntoClickMouseDerecho = cv::Point(x,y);
        puntoInicioDesplazamiento = desplazamientoOrigen;
    }

    if(event == CV_EVENT_RBUTTONUP)
        botonMouseDerechoAbajo = false;

    if(event == CV_EVENT_LBUTTONDOWN) //aprende a usar SHIFT y CTRL
    {
        if(b_dibujando_rectangulo)
        {
            b_dibujando_rectangulo = false;
            rectangulos.push_back(rectangulo(puntoOrigenRectangulo, puntoFinRectangulo));
            b_renderizar = true;
        }
        else
        {
            int b_punto_clave = determinarPropiedadesUbicacion(cv::Point(x,y) + desplazamientoOrigen);

            botonMouseIzquierdoAbajo = true;
            puntoClickMouseIzquierdo = cv::Point(x,y); //necesitábamos considerar desplazamientoOrigen

            //añadir una condición para determinar si dibujamos o no la flecha
            b_dibujando_flecha = true; //añadir condición
            puntoInicioFlecha = cv::Point(x,y) + desplazamientoOrigen; //añadir condición
        }
    }

    if(event == CV_EVENT_MOUSEMOVE)
    {
        if(botonMouseDerechoAbajo) //si estamos haciendo panning
        {
            desplazamientoOrigen.x = puntoInicioDesplazamiento.x + puntoClickMouseDerecho.x - x;
            desplazamientoOrigen.y = puntoInicioDesplazamiento.y + puntoClickMouseDerecho.y - y;
            b_renderizar = true;
        } //CV_EVENT_MOUSEMOVE && botonMouseDerechoAbajo

        if(botonMouseIzquierdoAbajo) //puede que queramos diseñar más eventos que cumplan estas características
        {

            if(b_dibujando_flecha)  //dibujar flecha
            {
                puntoTerminoFlecha = cv::Point(x,y) + desplazamientoOrigen; //la flecha es temporal, no se añade sino hasta que LBUTTONUP
                b_renderizar = true;
            }

        } //CV_EVENT_MOUSEMOVE && botonMouseIzquierdoAbajo

        if(b_dibujando_rectangulo)
        {
            puntoFinRectangulo = cv::Point(x,y) + desplazamientoOrigen;
            b_renderizar = true;
        }

    } //CV_EVENT_MOUSEMOVE

    if(event == CV_EVENT_LBUTTONUP)
    {
        botonMouseIzquierdoAbajo = false;
        if(b_dibujando_flecha)
        {
            flechas.push_back(flecha(puntoInicioFlecha, cv::Point(x,y) + desplazamientoOrigen));
            b_dibujando_flecha = false;
            b_renderizar = true;
        }
    }

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
