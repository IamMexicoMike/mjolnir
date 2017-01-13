#ifndef ZONAS_HPP
#define ZONAS_HPP

#include <initializer_list>
#include <vector>
#include <opencv2/opencv.hpp>
#include "elemento_diagrama.h"
#include "mjolnir.hpp"

class zona : public objeto
{
public:
  zona(std::initializer_list<cv::Point> ps, const cv::Scalar& c, std::string n) :
    puntos_(ps)
  {
    centro_ = puntos_[3];
    color_ = c;
    nombre_ = n; //los miembros de la base ya inicializados no se pueden inicializar en la subclase, pero sí asignar
    area_ = cv::contourArea(puntos_);
  }

  const cv::Scalar color() const {return color_;}
  std::string nombre() const {return nombre_;}
  cv::Point centro() const {return centro_;}
  std::vector<cv::Point> puntos_desplazados() const
  {
    std::vector<cv::Point> poff; //p'
    for(auto p : puntos_)
      poff.emplace_back(transformar(p));
    return poff;
  }
  virtual void dibujarse(cv::Mat&) const override;
  virtual void arrastrar(const cv::Point) override;
  virtual bool pertenece_a_area(const cv::Point) const override;
  virtual void imprimir_datos() const override;

private:
  std::vector<cv::Point> puntos_;
};

#endif // ZONAS_HPP
