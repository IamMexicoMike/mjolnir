#ifndef POSTGRES_FUNCIONES_H
#define POSTGRES_FUNCIONES_H

#include "pg/libpq-fe.h"

void conectar_db();
void prueba_db();
void entablar_escuchador_db();
void checar_input_db();

#endif // POSTGRES_FUNCIONES_H