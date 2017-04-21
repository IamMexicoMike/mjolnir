-- postgres tiene varias categorias de tipos fundamentales:
-- booleanos, numericos, string, bitstring, datetime, timespan, geometric, network, y user-defined.

CREATE SCHEMA "public";

CREATE TABLE Usuario(
	username varchar(24) PRIMARY KEY,
	email varchar(128) UNIQUE NOT NULL,
	password varchar(32) NOT NULL,
	nombre varchar(32) NOT NULL,
	apellido varchar(32) NOT NULL
);

CREATE TABLE Cliente(
	id varchar(64) PRIMARY KEY,
	nombre varchar(32) NOT NULL,
	apellido varchar(32) NOT NULL,
	email varchar(128),
	phone varchar(16) NOT NULL
);

CREATE TABLE LogCliente(
  id_log SERIAL PRIMARY KEY,
	id_cliente varchar(64) NOT NULL references Cliente(id) ON DELETE RESTRICT,
  fecha timestamp NOT NULL,
  log_text text NOT NULL
);

CREATE TABLE Producto(
  numero int4 PRIMARY KEY,
  nombre varchar(16) NOT NULL,
  peso numeric,
  precio numeric,
  descripcion text
);

CREATE TABLE Almacen(
  id int4 PRIMARY KEY

);

CREATE TABLE Inventario(
  id_producto integer references Producto(numero),
  id_almacen int4 REFERENCES Almacen(id),
  cantidad integer NOT NULL
);

CREATE TABLE MovimientoInventario(
  id_mov SERIAL PRIMARY KEY,
  id_alm_orig int4 REFERENCES Almacen(id),
  id_alm_dest int4 REFERENCES Almacen(id) CHECK (id_alm_orig != id_alm_dest),
  fecha_mov timestamp NOT NULL
);

CREATE TABLE Objeto(
  id int4 PRIMARY KEY,

)

INSERT INTO Usuario(username, email, password, nombre, apellido) VALUES
('emp1', 'emp1@gmail.com', 'here123', 'Migue', 'Lorenzo'),
('emp2', 'emp2@gmail.com', 'here123', 'Maru', 'Pujol'),
('emp3', 'emp3@gmail.com', 'here123', 'Martin', 'Rojas')
;

INSERT INTO Almacen(id) values
(1)
,(2)
,(100)
;

INSERT INTO Producto(numero,nombre, peso, precio, descripcion) VALUES
(1,'Hotelera', .550, 24.00, 'Toalla hotelera de lujo')
,(2,'Bata1', .450, 24.00, 'Bata para baño modelo 1')
;

INSERT INTO Inventario(id_producto, id_almacen, cantidad) VALUES
(1, 1, 400)
,(1, 2, 300)
,(1, 100, 6000)
,(2, 1, 70)
,(2, 2, 800)
,(2, 100, 3000)
;
