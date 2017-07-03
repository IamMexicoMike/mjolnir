#ifndef ZONAS_HPP
#define ZONAS_HPP

#include <initializer_list>
#include <vector>

#include <opencv2/opencv.hpp>

#include "elemento_diagrama.h"

class Mjolnir;
class objeto;

class zona : public objeto
{
public:
  zona(Mjolnir* ptrm, std::initializer_list<cv::Point> ps, const cv::Scalar& c, std::string n) :
    objeto(ptrm),
    puntos_(ps)
  {
    //centro_ = puntos_[3];
    auto M = cv::moments(puntos_, false);
    centro_ = cv::Point(int(M.m10/M.m00), int(M.m01/M.m00) );
    color_ = c;
    nombre_ = n; //los miembros de la base ya inicializados no se pueden inicializar en la subclase, pero sí asignar
    area_ = cv::contourArea(puntos_);
  }

  const cv::Scalar color() const {return color_;}
  std::string nombre() const {return nombre_;}
  cv::Point centro() const {return centro_;}
  virtual void dibujarse() override;
  virtual void arrastrar(const cv::Point) override;
  virtual bool pertenece_a_area(const cv::Point) const override;
  virtual void imprimir_datos() const override;

private:
  std::vector<cv::Point> puntos_;
};

std::vector<zona> crear_superzonas(Mjolnir* pm);
void zonas_fabrica(Mjolnir* pm);
void rellenar_zona_telares(Mjolnir* pm);

#endif // ZONAS_HPP
