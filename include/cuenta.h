#ifndef CUENTA_H
#define CUENTA_H

#include <vector>

/**Clase que herede de "Drawable" y de "Cuenta" podría servir en el diagrama
Posiblemente "Cuenta" sea una base abstracta, a partir de la cual subclases implementen su interfaz
De igual manera "Drawable" puede que tenga subclases como círculo o cuadrado*/
/*Activos. Representan todos los bienes y derechos que posee una empresa.
Estos pueden se físicos (tangibles), como el dinero, un terreno, un edificio,
etc. También pueden ser activos intangibles como la propiedad intelectual, una marca, entre otros.
*Pasivos. Los pasivos son todas las obligaciones que tiene un negocio con
alguna entidad. e.g si alguna empresa solicita un préstamo para poder continuar
con sus actividades, esta se compromete a pagar dicho préstamo en un tiempo
determinado. Esa obligación de pagar el préstamo es a lo que se llama pasivo.
*Capital. El capital contable representa las aportaciones de los propios dueños
del negocio, las aportaciones que hacen los inversores externos o el propio
capital ganado. Al invertir en un negocio los inversores adquieren derecho
sobre los activos de una empresa.
Cuentas de capital más frecuentes:
Capital contribuido - monto de dinero que aportan los dueños del negocio
Ganancias retenidas - ganancias de una empresa desde el momento de su creación
Dividendos - el dinero pagado a los accionistas. Estos se consideran como un valor
negativo y disminuyen el capital de una empresa.
*Ingresos. Representan el dinero que entra a una empresa debido a las actividades
económicas de la misma. Si una empresa comercial vende mercancías al contado se
contabiliza la operación dándole un débito a efectivo (activo) y un crédito a
"venta de mercancías" (el ingreso).
*Costos. Es todo desembolso de dinero que se hace con el objetivo de adquirir un activo
que generará beneficios económicos al negocio. Además del precio del activo que se adquiere,
también se considera un costo a todo el proceso que conlleva poner dicho activo en funcionamiento.
e.g. si se adquiere mercancías, se debe registrar la compra de mercancías como un costo y también
el flete.
*Gastos. Salida de dinero que queda consumida al instante. e.g. salarios, pago de la luz, pago del
transporte para la venta (es diferente que el flete para adquirir un producto(-_-))*/
/*Cuentas Reales{Activos, Pasivos, Capital}
Se usan en el <Estado de Situación Financiera> y permanecen abiertas por más de un período contable
 *Cuentas Nominales{Ingresos, Costos, Gastos}
Se usan en el <Estado de Resultados> (i.e Estado de Ganancias y/o Pérdidas)
Se les llama nominales porque cierran tras finalizar cada periodo contable, es decir:
Se resetean después de cierto período (literal comienzan en cero)*/
enum class TipoDeCuenta{Activos, Pasivos, Capital, Ingresos, Costos, Gastos};

/*Libro Diario General. Es uno de los más básicos que se usan en contabilidad en el día a día.
Se registran de manera cronológica todas las operaciones económicas que se realizan en un negocio
para su posterior pase al mayor. Al momento de registrar un asiento contable (?) en el libro diario
es necesario entender qué es lo que vamos a registrar y siempre recordar que la suma de todos los
débitos debe ser igual a la suma de todos los créditos para cumplir con el principio de partida doble.
Esto quiere decir que cada vez que demos un débito a una cuénta, también hay que darle un crédito por el
mismo valor a la otra cuenta afectada.
{Fecha de la transacción, Código de la cuenta contable que se está modificando, nombre de la cuenta y una
pequeña explicación, columa de cuenta auxiliar, columna del débito, columna del crédito.
Asientos simples y asientos compuestos.
Uno simple sólo tiene un débito y un crédito (póliza con un sólo movimiento?)
Uno complejo tiene varios*/
enum class TipoDePoliza{Diario, Ingreso, Egreso}; //por eso sirve que TipoDeCuenta y TipoDePoliza pertenecen a diferentes scopes

class poliza;

//patrón observer: hay varias maneras de representar una cuenta
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

/*las pólizas se pueden compartir entre cuentas(shared_ptr?), de hecho se deben compartir entre cuentas, mínimo un par*/
class poliza
{
public:
    poliza(); //Asignar tipo de póliza, número consecutivo
    unsigned obtener_consecutivo(); //acceder a esto vía mutex
private:
    TipoDePoliza _tipo;
};

/*Un movimiento pertenece a una póliza (unique_ptr?), e involucra dos cuentas*/
class movimiento
{

};



#endif // CUENTA_H
