#include "utilidades.hpp"

int global::llave_objeto_highlight=-1;
int global::llave_objeto_seleccionado=-1;
bool global::b_drag=false;
cv::Point global::ptInicioArrastre(0,0);
cv::Point global::ptFinArrastre(0,0);
std::map<int, relacion> global::relaciones;
std::map<int, objeto> global::objetos;
cv::Point global::desplazamientoOrigen(0,0);

//Debe determinar propiedades, no determinar si debe dibujarse. Actualmente highlightea...
std::pair<int, ubicacion::Flags> ubicacion::determinar_propiedades_ubicacion(cv::Point p,   //No creo que el pase de p por valor sea muy costoso
    std::vector<flecha>& flechas, std::map<int, objeto>& objetos)
{
    //considerar std::all_of, std::any_of, std::none_of en lugar de for_each //por qué?
    //así como std::find, std::find_if, std::find_if_not
    for_each(flechas.begin(), flechas.end(), [&](flecha& f)
    {
        //hacer algo
    });

    /*Este lambda podría generalizarse si recibiera como argumentos el tipo de operación y la categoría del contenedor. Nubloso*/
    auto encontrarIdHighlight = [&]() -> int
    {
        for(auto& ob : objetos)
            if(ob.second.pertenece_a_area(p))
                return ob.first;
        return -1;
    };

    int llave = encontrarIdHighlight(); //asignamos el id del rectángulo que cumplió las condiciones, o -1.

    /**El caso más frecuente: no hay cambios en lo que se highlighteo*/
    if(llave == global::llave_objeto_highlight) //no hay cambios, no renderizamos
        return std::pair<int, ubicacion::Flags>(llave, ubicacion::Flags::SinCambios);

    /**Segundo caso: Había algo highlighteado pero ya no*/
    if(llave < 0)
    {
        objetos.at(global::llave_objeto_highlight).highlightear(false);//des-highlighteamos el anterior
        global::llave_objeto_highlight = -1;
        return std::pair<int, ubicacion::Flags>(llave, ubicacion::Flags::Vacia);
    }

    /**Tercer caso: hay algo highlighteable*/
    objetos.at(llave).highlightear(true); //highlighteamos al nuevo

    /*Si es que había otro highlighteado antes lo deshighlighteamos*/
    if(global::llave_objeto_highlight > 0)
        objetos.at(global::llave_objeto_highlight).highlightear(false);

    global::llave_objeto_highlight = llave; //actualizamos la llave highlight

    return std::pair<int, ubicacion::Flags>(llave, ubicacion::Flags::Objeto);
}
