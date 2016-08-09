#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <bitset> //borrame si no debuggeas
#include <future>
#include <thread>
#include "nana_main.h"

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

    //matriz = Mat(ALTURA_REGION, ANCHO_REGION, CV_8UC3, GRIS); //inicializamos la matriz que renderizará al color gris
    matriz = GRIS;
    for(int i=15-(global::desplazamientoOrigen.x % 15); i<matriz.cols; i+=15) //"generamos" un efecto de desplazamiento de la cuadrícula
        line(matriz, Point(i,0), Point(i,matriz.rows), BLANCO, 1, 4, 0); //cuadrícula, vertical
    for(int i=15-(global::desplazamientoOrigen.y % 15); i<matriz.rows; i+=15) //Mat::cols es int, no uint
        line(matriz, Point(0,i), Point(matriz.cols,i), BLANCO, 1, 4, 0); //cuadrícula, horizontal

    if(b_dibujando_flecha) //dibujamos una flecha temporal
        arrowedLine(matriz, Point(puntoInicioFlecha.x - global::desplazamientoOrigen.x, puntoInicioFlecha.y - global::desplazamientoOrigen.y),
                    Point(puntoTerminoFlecha.x - global::desplazamientoOrigen.x,puntoTerminoFlecha.y - global::desplazamientoOrigen.y),
                    COLOR_FLECHA_DIBUJANDO, 2, CV_AA);

    if(b_dibujando_circulo) //dibujamos un circulo temporal
        ;

    if(b_dibujando_objeto) //dibujamos un rectángulo temporal
    {
        rectangle(matriz, Rect(puntoOrigenobjeto - global::desplazamientoOrigen, puntoFinobjeto - global::desplazamientoOrigen),
                  COLOR_RECT_DIBUJANDO, 2, CV_AA);
    }


    //dibujamos todas als flechas - actualmente no hace nada
    for_each(flechas.begin(), flechas.end(), [&](flecha& f) {f.dibujarse(matriz, global::desplazamientoOrigen);});

    //dibujamos todos los objetos
    for(auto& rec : global::objetos)
        rec.second.dibujarse(matriz, global::desplazamientoOrigen);

    //dibujamos todas las relaciones
    for(auto& rel : global::relaciones)
        rel.second.dibujarse(matriz, global::desplazamientoOrigen);

    mat_panel = AZUL_PALIDO;
    mat_header = BLANCO;

    if(global::llave_objeto_seleccionado > 0)
        putText(matriz, std::string("Seleccionado: " + std::to_string(global::llave_objeto_seleccionado)),
            header::HEADER1, FONT_HERSHEY_PLAIN, 1, Scalar(230,100,0));


    // medimos tiempo
    t_final = std::chrono::system_clock::now();
    std::chrono::duration<double> t_renderizar = t_final - t_inicial;
    cout << t_renderizar.count() << "s\n";

    imshow("Mjolnir", matriz); //actualizamos el diagrama
}

inline void manejarInputTeclado(Mat& matriz, int k) //k no incluye ni ctrl, ni shift, ni alt por mencionar algunas
{
    cout << k << "!\n"; //borrame si no debugeas, o coméntame mejor!

    bool b_renderizar = false;

    constexpr unsigned DESPLAZAMIENTO = 5; //sospechoso contexpr. Lo estás haciendo bien?
    switch (k) {
    case TECLADO_FLECHA_ARRIBA:
        global::desplazamientoOrigen.y -= DESPLAZAMIENTO;
        b_renderizar = true;
        break;
    case TECLADO_FLECHA_ABAJO:
        global::desplazamientoOrigen.y += DESPLAZAMIENTO;
        b_renderizar = true;
        break;
    case TECLADO_FLECHA_DERECHA:
        global::desplazamientoOrigen.x += DESPLAZAMIENTO;
        b_renderizar = true;
        break;
    case TECLADO_FLECHA_IZQUIERDA:
        global::desplazamientoOrigen.x -= DESPLAZAMIENTO;
        b_renderizar = true;
        break;
    case 114: //r (ojo, R tiene su propia clave
        puntoOrigenobjeto = puntoActualMouse + global::desplazamientoOrigen;
        b_dibujando_objeto = true;
        break;
    case 13: //tecla enter
        if(b_dibujando_objeto)
        {
            b_dibujando_objeto = false;
            global::objetos.emplace(objeto::id_ - 1, objeto(puntoOrigenobjeto, puntoFinobjeto));
            b_renderizar = true;
        }
        break;
    case 103: //g de guardar
        //fstream fs("diagrama") //necesitas definir el operador << para tus clases
        break;
    case 50: //debug, 3
        cout << "valor global: " << global::llave_objeto_highlight << endl;
        break;
    case 100: //d de debug
        cout << "obj sel: " << global::llave_objeto_seleccionado << " obj hgl: " << global::llave_objeto_highlight << endl;
        for(auto& ob : global::objetos)
            cout << ob.first << "," << ob.second.get_id() << " | " << endl;
        for(auto& rel : global::relaciones)
            cout << rel.first << "," << rel.second.get_id() << " | " << endl;
        cout << "\ntodas las relaciones:" << endl;
        for(auto& rel : global::relaciones)
            cout << "relacion " << rel.second.get_id() << ": "
                 << rel.second.get_objetos().first << ',' << rel.second.get_objetos().second << endl;
        if(global::llave_objeto_seleccionado > 0)
        {
            cout << "relaciones del objeto " << global::llave_objeto_seleccionado << ":" << endl;
            for(auto id : global::objetos.at(global::llave_objeto_seleccionado).get_relaciones())
                cout << id << endl;
        }
        break;

    case 43: //+ ensanchamos el diagrama
        if(region.cols < 2000)
        {
            region = cv::Mat(region.rows, region.cols+15, CV_8UC3, cv::Scalar(200,200,200)); //debe tener un scope global
            mat_panel = region.colRange(region.cols - 100, region.cols);
            mat_header = region.colRange(0, region.cols - 100).rowRange(0,30);
            b_renderizar = true;
        }
        break;

    case 45: //- adelgazamos el diagrama
        if(region.cols > 200)
        {
            region = cv::Mat(region.rows, region.cols-15, CV_8UC3, cv::Scalar(200,200,200)); //debe tener un scope global
            mat_panel = region.colRange(region.cols - 100, region.cols);
            mat_header = region.colRange(0, region.cols - 100).rowRange(0,30);
            b_renderizar = true;
        }
        break;

    case 3014656: //suprimir, borrar objeto
        if(global::llave_objeto_seleccionado > 0)
        {
            global::objetos.erase(global::llave_objeto_seleccionado);
            global::llave_objeto_seleccionado = -1;
            global::llave_objeto_highlight = -1;
            ubicacion::determinar_propiedades_ubicacion(puntoActualMouse, flechas, global::objetos); //para actualizar highlight
            b_renderizar = true;
        }
        break;


    }
    //cout << global::desplazamientoOrigen << endl;

    if(b_renderizar)
        renderizarDiagrama(matriz);
}

inline void manejarInputMouse(int event, int x, int y, int flags, void*)
{
    bool b_renderizar = false;
    puntoActualMouse = cv::Point(x,y); //esta variable siempre lleva el rastro de dónde está el mouse
    //cout << (flags & CV_EVENT_FLAG_CTRLKEY) << endl;

    if(event == CV_EVENT_RBUTTONDOWN) //no necesita propiedades_ubicacion, es para panning
    {
        botonMouseDerechoAbajo = true;
        puntoClickMouseDerecho = puntoActualMouse;
        puntoInicioDesplazamiento = global::desplazamientoOrigen;
    }

    if(event == CV_EVENT_RBUTTONUP)
        botonMouseDerechoAbajo = false;

    if(event == CV_EVENT_LBUTTONDOWN) //te falta usar SHIFT y CTRL
    {
        b_renderizar = true; //entramos aquí relativamente pocas veces por lo que no es costoso

        /*Si estábamos dibujando un rectángulo y dimos click, lo insertamos y no hacemos nada más*/
        if(b_dibujando_objeto)
        {
            b_dibujando_objeto = false;
            global::objetos.emplace(objeto::id_ - 1, objeto(puntoOrigenobjeto, puntoFinobjeto));
            b_renderizar = true;
        }

        //de lo contrario, evaluamos el punto y establecemos condiciones para la selección y el arrastre
        else
        {
            botonMouseIzquierdoAbajo = true;

            /*En este caso, siempre consultamos las propiedades de la ubicación*/
            auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + global::desplazamientoOrigen,
                                                                     flechas, global::objetos);

            //checamos si el punto actual coincide con un objeto. Si sí, lo seleccionamos.
            if(props.first > 0 ) //props.first es el id
            {
                if(global::llave_objeto_seleccionado > 0) //si había otro brother seleccionado antes
                    global::objetos.at(global::llave_objeto_seleccionado).seleccionar(false); //des-seleccionamos al anterior
                global::objetos.at(props.first).seleccionar(true); //seleccionamos al brother
                global::llave_objeto_seleccionado = props.first; //actualizamos al seleccionado

                if(flags & CV_EVENT_FLAG_CTRLKEY) //vamos a dibujar flecha, no a arrastrar
                {
                    b_dibujando_flecha = true; //añadir condición
                    puntoInicioFlecha = puntoActualMouse + global::desplazamientoOrigen; //añadir condición
                    puntoTerminoFlecha = puntoInicioFlecha; //"reseteamos" la flecha;
                }
                else //de lo contrario, arrastramos
                {
                    global::b_drag = true; //condiciones de arrastre habilitadas
                    global::ptInicioArrastre = puntoActualMouse + global::desplazamientoOrigen;
                    global::ptFinArrastre = global::ptInicioArrastre;
                    b_dibujando_flecha = false;
                }
                //hay espacio para alt y shift. Afortunadamente drag y dibujar flecha son mutuamente excluyentes
            }

            else if(global::llave_objeto_seleccionado > 0) //no caimos en nadie, pero había un brother seleccionado
            {
                global::objetos.at(global::llave_objeto_seleccionado).seleccionar(false); //lo des-seleccionamos
                global::llave_objeto_seleccionado=-1; //y reseteamos el id de selección
            }

        }
    }

    if(event == CV_EVENT_LBUTTONUP)
    {
        botonMouseIzquierdoAbajo = false;
        global::b_drag = false; //terminan las condiciones de arrastre

        if(b_dibujando_flecha) //esto se va a revampear
        {
            //flechas.push_back(flecha(puntoInicioFlecha, cv::Point(x,y) + global::desplazamientoOrigen));
            auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + global::desplazamientoOrigen,
                                                                     flechas, global::objetos);
            if(props.first > 0)
                global::relaciones.emplace(relacion::id_ - 1, relacion(global::llave_objeto_seleccionado, props.first));

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
            global::desplazamientoOrigen.x = puntoInicioDesplazamiento.x + puntoClickMouseDerecho.x - x;
            global::desplazamientoOrigen.y = puntoInicioDesplazamiento.y + puntoClickMouseDerecho.y - y;
            b_renderizar = true;
        }

        if(botonMouseIzquierdoAbajo) //Flechas. Dragging. Moviendo el cursor con click izquierdo apretado.
        {
            //propiedades ubicacion, highlightear destino de flecha, posible drag n drop
            if(global::b_drag)
            {
                cv::Point pt = puntoActualMouse + global::desplazamientoOrigen;
                cv::Point dif = pt - global::ptInicioArrastre;
                global::objetos.at(global::llave_objeto_seleccionado).arrastrar(dif);
                global::ptInicioArrastre = pt;
                //el vector de arrasre el ptFinArrastre - ptInicioArrastre

                b_renderizar = true;
            }
            //...

            if(b_dibujando_flecha)  //dibujando flecha temporal
            {
                /*props se va a usar después para tener feedback con el highlight*/
                auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + global::desplazamientoOrigen,
                                                                     flechas, global::objetos); //para highlightear el destino

                puntoTerminoFlecha = puntoActualMouse + global::desplazamientoOrigen; //la flecha es temporal, no se añade sino hasta que LBUTTONUP
                b_renderizar = true;
            }
        }

        if(!botonMouseDerechoAbajo && !botonMouseIzquierdoAbajo) //estamos chillin'
        {
            auto props = ubicacion::determinar_propiedades_ubicacion(puntoActualMouse + global::desplazamientoOrigen,
                                                                     flechas, global::objetos);
            if(props.second == ubicacion::Flags::SinCambios)
                b_renderizar = false; //para qué renderizamos
            else
                b_renderizar = true;
        }

        if(b_dibujando_objeto)
        {
            puntoFinobjeto = cv::Point(x,y) + global::desplazamientoOrigen;
            b_renderizar = true;
        }

    } //CV_EVENT_MOUSEMOVE

    if(b_renderizar)
        renderizarDiagrama(region);

} //manejarInputMouse

int main()
{
    //diagrama_completo.colRange(0, region.cols) = region.colRange(0, region.cols);
    namedWindow("Mjolnir");
    setMouseCallback("Mjolnir", manejarInputMouse);
    renderizarDiagrama(region);
    //thread t1(nana_fun);

    while (true)
    {
        int k = waitKey(0); //nota que comparamos unsigned con signed, y funciona. Hmm
        if(-1 == k) //de este modo, si cierras la ventana efectivamente sales del ciclo
            break;
        manejarInputTeclado(region, k);
    }

    //t1.join();
    return 0;
}
