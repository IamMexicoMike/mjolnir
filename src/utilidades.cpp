#include "utilidades.hpp"

int global::llave_rectangulo_highlight=-1;
int global::llave_rectangulo_seleccionado=-1;
bool global::b_drag=false;
cv::Point global::ptInicioArrastre(0,0);
cv::Point global::ptFinArrastre(0,0);

//Debe determinar propiedades, no determinar si debe dibujarse. Actualmente highlightea...
std::pair<int, ubicacion::Flags> ubicacion::determinar_propiedades_ubicacion(cv::Point p,   //No creo que el pase de p por valor sea muy costoso
    std::vector<flecha>& flechas, std::map<int, rectangulo>& rectangulos)
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
        for(auto& rec : rectangulos)
            if(rec.second.pertenece_a_area(p))
                return rec.first;
        return -1;
    };

    int llave = encontrarIdHighlight(); //asignamos el id del rectángulo que cumplió las condiciones, o -1.

    /**El caso más frecuente: no hay cambios en lo que se highlighteo*/
    if(llave == global::llave_rectangulo_highlight) //no hay cambios, no renderizamos
        return std::pair<int, ubicacion::Flags>(llave, ubicacion::Flags::SinCambios);

    /**Segundo caso: Había algo highlighteado pero ya no*/
    if(llave < 0)
    {
        rectangulos.at(global::llave_rectangulo_highlight).highlightear(false);//des-highlighteamos el anterior
        global::llave_rectangulo_highlight = -1;
        return std::pair<int, ubicacion::Flags>(llave, ubicacion::Flags::Vacia);
    }

    /**Tercer caso: hay algo highlighteable*/
    rectangulos.at(llave).highlightear(true); //highlighteamos al nuevo

    /*Si es que había otro highlighteado antes lo deshighlighteamos*/
    if(global::llave_rectangulo_highlight > 0)
        rectangulos.at(global::llave_rectangulo_highlight).highlightear(false);

    global::llave_rectangulo_highlight = llave; //actualizamos la llave highlight

    return std::pair<int, ubicacion::Flags>(llave, ubicacion::Flags::Objeto);
}
