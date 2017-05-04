#ifndef SYNC_H
#define SYNC_H

#include "elemento_diagrama.h"
#include "postgres_funciones.h"

class sync : public rectangulo
{
  public:
    static std::set<sync*> set_modificados;
    static std::atomic<bool> b_sync_cambio;
    void actualizar_db(); //esto deberia ser virtual propio de una interfaz sync
    sync(cv::Point inicio, cv::Point fin); //para construirse a partir del diagrama y ser insertado en la base de datos
    sync(int id, cv::Point inicio, cv::Point fin); //para construirse a partir de la base de datos
    virtual ~sync();

  protected:
    virtual void arrastrar(const cv::Point pt) override;
  private:
};

#endif // SYNC_H
