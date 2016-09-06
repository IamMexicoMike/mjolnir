#include "elemento_diagrama.h"
#include "utilidades.hpp"

#include <iostream>

int objeto::id_ = 1; //hasta donde sabes debe definirse fuera de la clase, y no en el header
int relacion::id_ = 1;
const cv::Scalar objeto::_color_inicial(100, 65, 150);

elemento_diagrama::elemento_diagrama()
{
    //ctor
}

elemento_diagrama::~elemento_diagrama()
{
    //dtor
}

flecha::flecha(int llave_origen, int llave_destino, std::map<int, objeto>& contenedor)
{
    _inicio = contenedor.at(llave_origen)._centro;
    _fin = contenedor.at(llave_destino)._centro;
    _centro = cv::Point(_inicio.x + (_fin.x - _inicio.x)/2, _inicio.y + (_fin.y - _inicio.y)/2);
    _b_seleccionado = false;
}


void flecha::dibujarse(cv::Mat& m, cv::Point despl) //deberíamos inlinear?
{
    arrowedLine(m, cv::Point(_inicio.x - despl.x, _inicio.y - despl.y), //despl es desplazamientoOrigen
                cv::Point(_fin.x - despl.x, _fin.y - despl.y), cv::Scalar(205,155,25), 2, CV_AA);
}



void objeto::dibujarse(cv::Mat& m, cv::Point despl)
{
    cv::rectangle(m, cv::Rect(_inicio - despl, _fin - despl), _color, 2, CV_AA);
    cv::rectangle(m, cv::Rect(cv::Point(_centro.x - despl.x - 4, _centro.y - despl.y - 4), cv::Size(8,8)),
                  cv::Scalar(150, 165, 250), 1, CV_AA);

    if(_b_seleccionado)
        cv::rectangle(m, cv::Rect(_inicio - despl, _fin - despl), COLOR_SELECCION, 3, CV_AA); //selección

    if(_b_highlighteado)
        cv::rectangle(m, cv::Rect(_inicio - despl, _fin - despl), COLOR_HIGHLIGHT_, 1, CV_AA); //highlight
}

/*Es crítico que esta función esté optimizada*/
/*Dos condiciones por dimensión para no pertenecer:
 Si pt es menor o mayor que tanto inicio como fin */
bool objeto::pertenece_a_area(const cv::Point pt)
{
    return !((pt.x > _inicio.x && pt.x > _fin.x) || (pt.x < _inicio.x && pt.x < _fin.x) ||
        (pt.y > _inicio.y && pt.y > _fin.y) || (pt.y < _inicio.y && pt.y < _fin.y));
}

void objeto::imprimir_datos()
{
    std::cout << _ancho << ", " << _altura << " | A=" <<  _altura*_ancho << std::endl;
}

//se llama continuamente cuando haces drag, no sólo una vez
void objeto::arrastrar(const cv::Point pt) //no es realmente un punto, sino una diferencia entre dos puntos
{
    _fin += pt;
    _inicio += pt;
    _centro += pt;
    notificar(Notificacion::Movimiento);
}

void objeto::relocalizar(const cv::Point pt) //el punto tiene una referencia absoluta
{

}

void objeto::notificar(Notificacion noti)
{
    using namespace std;
    //std::cout << "global::relaciones contiene " << global::relaciones.size() << "elementos, sus ids: ";
    //for(auto i : global::relaciones)
        //std::cout << i.first << ',';

    //std::cout << "\n\nel objeto local contiene " << _relaciones.size() << " elementos, y queremos acceder a: ";
    //for(auto id : _relaciones)
        //cout << id << endl;

    if(!_relaciones.empty())
        for(auto id_rel : _relaciones) //para toda relación del objeto...
        {
            try{
                //std::cout << " | obj " << _id  << " trato de notificar a rel " << id_rel << std::endl;
                global::relaciones.at(id_rel).recibir_notificacion(_id, noti); //les envía una notificación
            }   catch(std::out_of_range e) {}
        }

}

void objeto::recibir_notificacion(int id, Notificacion noti)
{
    if(Notificacion::Destruccion == noti) //borramos el indice de esa notificacion
        quitar_relacion(id);
}

objeto::~objeto() //luego lo optimizas
{
    std::cout << "soy el destructor de objetos[" << _id << "]\n";
    notificar(Notificacion::Destruccion); //notifica a todas sus relaciones que será destruido
    for(auto id : _relaciones)
        global::relaciones.erase(id); //y luego las borra
}

relacion::relacion(int id_ob1, int id_ob2)
{

    _id = id_++; //el valor presente se guarda, y luego id_ se incrementa
    ids = {id_ob1, id_ob2};
    pt1 = global::objetos.at(id_ob1).anadir_relacion(_id);
    pt2 = global::objetos.at(id_ob2).anadir_relacion(_id);
    std::cout << "soy el constructor de relacion[" << _id << ']' << "y vinculo " << id_ob1 << ',' << id_ob2 << std::endl;
};

void relacion::notificar(int id, Notificacion noti)
{
    global::objetos.at(id).recibir_notificacion(_id, noti);
}

void relacion::recibir_notificacion(int id, Notificacion noti)
{
    //std::cout << " | rel " << _id  << " recibi notificacion " << " de obj " << id << std::endl;
    if(Notificacion::Movimiento == noti)
    {
        if(id == ids.first)
            pt1 = global::objetos.at(id).get_centro();
        else if(id == ids.second)
            pt2 = global::objetos.at(id).get_centro();
    }
    else if(Notificacion::Destruccion == noti)
    {
        //El stack debe desenvolverse por lo que el que notificó de destrucción debe ser el que se haga responsable de la relacion
        if(id == ids.first)
            notificar(ids.second, Notificacion::Destruccion); //el objeto que reciba esto simplemente borra el id de su vector
        else if(id == ids.second)
            notificar(ids.first, Notificacion::Destruccion);
    }
}

void relacion::dibujarse(cv::Mat& m, cv::Point despl)
{
    //std::cout << "soy la relacion " << _id << " y me estoy dibujando" << std::endl;
    line(m, cv::Point(pt1.x - despl.x, pt1.y - despl.y), //despl es desplazamientoOrigen
                cv::Point(pt2.x - despl.x, pt2.y - despl.y), cv::Scalar(205,155,125), 2, CV_AA);
}

