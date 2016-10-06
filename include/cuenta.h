#ifndef CUENTA_H
#define CUENTA_H

#include <vector>

/**Clase que herede de "Drawable" y de "Cuenta" podr�a servir en el diagrama
Posiblemente "Cuenta" sea una base abstracta, a partir de la cual subclases implementen su interfaz
De igual manera "Drawable" puede que tenga subclases como c�rculo o cuadrado*/
/*Activos. Representan todos los bienes y derechos que posee una empresa.
Estos pueden se f�sicos (tangibles), como el dinero, un terreno, un edificio,
etc. Tambi�n pueden ser activos intangibles como la propiedad intelectual, una marca, entre otros.
*Pasivos. Los pasivos son todas las obligaciones que tiene un negocio con
alguna entidad. e.g si alguna empresa solicita un pr�stamo para poder continuar
con sus actividades, esta se compromete a pagar dicho pr�stamo en un tiempo
determinado. Esa obligaci�n de pagar el pr�stamo es a lo que se llama pasivo.
*Capital. El capital contable representa las aportaciones de los propios due�os
del negocio, las aportaciones que hacen los inversores externos o el propio
capital ganado. Al invertir en un negocio los inversores adquieren derecho
sobre los activos de una empresa.
Cuentas de capital m�s frecuentes:
Capital contribuido - monto de dinero que aportan los due�os del negocio
Ganancias retenidas - ganancias de una empresa desde el momento de su creaci�n
Dividendos - el dinero pagado a los accionistas. Estos se consideran como un valor
negativo y disminuyen el capital de una empresa.
*Ingresos. Representan el dinero que entra a una empresa debido a las actividades
econ�micas de la misma. Si una empresa comercial vende mercanc�as al contado se
contabiliza la operaci�n d�ndole un d�bito a efectivo (activo) y un cr�dito a
"venta de mercanc�as" (el ingreso).
*Costos. Es todo desembolso de dinero que se hace con el objetivo de adquirir un activo
que generar� beneficios econ�micos al negocio. Adem�s del precio del activo que se adquiere,
tambi�n se considera un costo a todo el proceso que conlleva poner dicho activo en funcionamiento.
e.g. si se adquiere mercanc�as, se debe registrar la compra de mercanc�as como un costo y tambi�n
el flete.
*Gastos. Salida de dinero que queda consumida al instante. e.g. salarios, pago de la luz, pago del
transporte para la venta (es diferente que el flete para adquirir un producto(-_-))*/
/*Cuentas Reales{Activos, Pasivos, Capital}
Se usan en el <Estado de Situaci�n Financiera> y permanecen abiertas por m�s de un per�odo contable
 *Cuentas Nominales{Ingresos, Costos, Gastos}
Se usan en el <Estado de Resultados> (i.e Estado de Ganancias y/o P�rdidas)
Se les llama nominales porque cierran tras finalizar cada periodo contable, es decir:
Se resetean despu�s de cierto per�odo (literal comienzan en cero)*/
enum class TipoDeCuenta{Activos, Pasivos, Capital, Ingresos, Costos, Gastos};

/*Libro Diario General. Es uno de los m�s b�sicos que se usan en contabilidad en el d�a a d�a.
Se registran de manera cronol�gica todas las operaciones econ�micas que se realizan en un negocio
para su posterior pase al mayor. Al momento de registrar un asiento contable (?) en el libro diario
es necesario entender qu� es lo que vamos a registrar y siempre recordar que la suma de todos los
d�bitos debe ser igual a la suma de todos los cr�ditos para cumplir con el principio de partida doble.
Esto quiere decir que cada vez que demos un d�bito a una cu�nta, tambi�n hay que darle un cr�dito por el
mismo valor a la otra cuenta afectada.
{Fecha de la transacci�n, C�digo de la cuenta contable que se est� modificando, nombre de la cuenta y una
peque�a explicaci�n, columa de cuenta auxiliar, columna del d�bito, columna del cr�dito.
Asientos simples y asientos compuestos.
Uno simple s�lo tiene un d�bito y un cr�dito (p�liza con un s�lo movimiento?)
Uno complejo tiene varios*/
enum class TipoDePoliza{Diario, Ingreso, Egreso}; //por eso sirve que TipoDeCuenta y TipoDePoliza pertenecen a diferentes scopes

class poliza;

//patr�n observer: hay varias maneras de representar una cuenta
class cuenta
{
public:
    cuenta(); //Asignar tipo de cuenta, nombre de cuenta
    ~cuenta();

protected:

private:
    TipoDeCuenta _tipo;
    std::vector<poliza> polizas;
};

/*las p�lizas se pueden compartir entre cuentas(shared_ptr?), de hecho se deben compartir entre cuentas, m�nimo un par*/
class poliza
{
public:
    poliza(); //Asignar tipo de p�liza, n�mero consecutivo
    unsigned obtener_consecutivo(); //acceder a esto v�a mutex
private:
    TipoDePoliza _tipo;
};

/*Un movimiento pertenece a una p�liza (unique_ptr?), e involucra dos cuentas*/
class movimiento
{

};



#endif // CUENTA_H
