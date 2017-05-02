CREATE SCHEMA "public";

CREATE TABLE Toallas(
  id INTEGER NOT NULL UNIQUE,
  nombre text,
  cabecera int4 check (cabecera > 0) NOT NULL,
  largo int4 check (largo > 0) NOT NULL,
  peso numeric check (peso > 0) NOT NULL,
  rasurado bool NOT NULL,
  fecha_creacion timestamp DEFAULT current_date,
  descripcion text,
  precio_por_kilo numeric check (precio_por_kilo > 0)
);
