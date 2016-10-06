#include "elemento_diagrama.h"
#include "utilidades.hpp"
#include <windows.h>

#include <iostream>

int objeto::sid = 1; //hasta donde sabes debe definirse fuera de la clase, y no en el header
int relacion::sid = 1;

flecha::flecha(int llave_origen, int llave_destino, std::map<int, objeto>& contenedor)
{
    inicio_ = contenedor.at(llave_origen).centro_;
    fin_ = contenedor.at(llave_destino).centro_;
    centro_ = cv::Point(inicio_.x + (fin_.x - inicio_.x)/2, inicio_.y + (fin_.y - inicio_.y)/2);
    b_seleccionado_ = false;
}


void flecha::dibujarse(cv::Mat& m, cv::Point despl) //deberíamos inlinear?
{
    arrowedLine(m, cv::Point(inicio_.x - despl.x, inicio_.y - despl.y), //despl es desplazamientoOrigen
                cv::Point(fin_.x - despl.x, fin_.y - despl.y), cv::Scalar(205,155,25), 2, CV_AA);
}

//-----------------------------------------------------------------------------------------------------------------

void objeto::dibujarse(cv::Mat& m, cv::Point despl)
{
    cv::rectangle(m, cv::Rect(inicio_ - despl, fin_ - despl), color_, 2, CV_AA);
    cv::rectangle(m, cv::Rect(cv::Point(centro_.x - despl.x - 4, centro_.y - despl.y - 4), cv::Size(8,8)),
                  cv::Scalar(150, 165, 250), 1, CV_AA);

    if(b_seleccionado_)
        cv::rectangle(m, cv::Rect(inicio_ - despl, fin_ - despl), COLOR_SELECCION, 3, CV_AA); //selección

    if(_b_highlighteado)
        cv::rectangle(m, cv::Rect(inicio_ - despl, fin_ - despl), COLOR_HIGHLIGHT_, 1, CV_AA); //highlight
}

bool objeto::pertenece_a_area(const cv::Point pt)
{
    return !((pt.x > inicio_.x && pt.x > fin_.x) || (pt.x < inicio_.x && pt.x < fin_.x) ||
        (pt.y > inicio_.y && pt.y > fin_.y) || (pt.y < inicio_.y && pt.y < fin_.y));
}

void objeto::imprimir_datos()
{
    /*...*/
}

//se llama continuamente cuando haces drag, no sólo una vez
void objeto::arrastrar(const cv::Point pt) //no es realmente un punto, sino una diferencia entre dos puntos
{
    fin_ += pt;
    inicio_ += pt;
    centro_ += pt;
    notificar(Notificacion::Movimiento);
}

void objeto::notificar(Notificacion noti)
{
    using namespace std;
    //std::cout << "glb::relaciones contiene " << glb::relaciones.size() << "elementos, sus ids: ";
    //for(auto i : glb::relaciones)
        //std::cout << i.first << ',';

    //std::cout << "\n\nel objeto local contiene " << relaciones_.size() << " elementos, y queremos acceder a: ";
    //for(auto id : relaciones_)
        //cout << id << endl;

    if(!relaciones_.empty())
        for(auto id_rel : relaciones_) //para toda relación del objeto...
        {
            try{
                //std::cout << " | obj " << id_  << " trato de notificar a rel " << id_rel << std::endl;
                glb::relaciones.at(id_rel).recibir_notificacion(id_, noti); //les envía una notificación
            }   catch(std::out_of_range e) {cout << "Fuera de rango"<< endl;}
        }
}

void objeto::recibir_notificacion(int id_rel, Notificacion noti)
{
  if(Notificacion::Destruccion == noti) //borramos el indice de esa notificacion
  {
    auto itr = find(objeto::relaciones_.begin(), objeto::relaciones_.end(), id_rel);
    objeto::relaciones_.erase(itr);
  }
}

objeto::~objeto() //luego lo optimizas
{
    std::cout << "soy el destructor de objetos[" << id_ << "]\n";
    notificar(Notificacion::Destruccion); //notifica a todas sus relaciones que será destruido
    for(auto id : relaciones_)
        glb::relaciones.erase(id); //y luego las borra
}

//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo



void relacion::notificar(int id, Notificacion noti)
{
    glb::objetos.at(id).recibir_notificacion(id_, noti);
}

void relacion::recibir_notificacion(int id, Notificacion noti)
{
    //std::cout << " | rel " << id_  << " recibi notificacion " << " de obj " << id << std::endl;
    if(Notificacion::Movimiento == noti)
    {
        if(id == ids_.first)
            pt1_ = glb::objetos.at(id).centro();
        else if(id == ids_.second)
            pt2_ = glb::objetos.at(id).centro();
    }
    else if(Notificacion::Destruccion == noti)
    {
        //El stack debe desenvolverse por lo que el que notificó de destrucción debe ser el que se haga responsable de la relacion
        if(id == ids_.first)
            notificar(ids_.second, Notificacion::Destruccion); //el objeto que reciba esto simplemente borra el id de su vector
        else if(id == ids_.second)
            notificar(ids_.first, Notificacion::Destruccion);
    }
}

void relacion::dibujarse(cv::Mat& m, cv::Point despl)
{
    line(m, cv::Point(pt1_.x - despl.x, pt1_.y - despl.y), //despl es desplazamientoOrigen
                cv::Point(pt2_.x - despl.x, pt2_.y - despl.y), cv::Scalar(205,155,125), 2, CV_AA);
}

//rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
