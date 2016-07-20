#include <iostream>

#include <opencv2/opencv.hpp>

#include "datos.hpp"
#include "elemento_diagrama.h"

using namespace std;
using namespace cv;

void determinarPropiedadesUbicacion(Point p){}

inline void renderizarDiagrama(Mat& matriz) //No hay pedo si tratamos de dibujar una región que no pertenece a matriz
{
    matriz = Mat(ALTURA_REGION, ANCHO_REGION, CV_8UC3, Scalar(200,200,200));
    for(int i=15-(desplazamientoOrigen.x % 15); i<matriz.cols; i+=15) //"generamos" un efecto de desplazamiento de la cuadrícula
        line(matriz, Point(i,0), Point(i,matriz.rows), Scalar(255,255,255), 1, 4, 0); //cuadrícula, vertical
    for(int i=15-(desplazamientoOrigen.y % 15); i<matriz.rows; i+=15) //Mat::cols es int, no uint
        line(matriz, Point(0,i), Point(matriz.cols,i), Scalar(255,255,255), 1, 4, 0); //cuadrícula, horizontal

    Rect rect(Point(300-desplazamientoOrigen.x, -100-desplazamientoOrigen.y), Point(400-desplazamientoOrigen.x,400-desplazamientoOrigen.y));
    rectangle(matriz, rect, Scalar(123,211,43), 2, CV_AA);
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
        cout << "renderizando rect" << endl;
    }


    //dibujamos todas als flechas
    for_each(flechas.begin(), flechas.end(), [&](flecha& f) {f.dibujarse(matriz, desplazamientoOrigen);});
    for_each(rectangulos.begin(), rectangulos.end(), [&](rectangulo& r) {r.dibujarse(matriz, desplazamientoOrigen);});

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

    }
    //cout << desplazamientoOrigen << endl;

    if(b_renderizar)
        renderizarDiagrama(matriz);
}

inline void manejarInputMouse(int event, int x, int y, int, void*)
{
    bool b_renderizar = false;
    cout << x <<","<< y << " evento: " << event << endl;
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
        else {
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

    determinarPropiedadesUbicacion(Point(x,y)); //esta función aún no la implementas

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
