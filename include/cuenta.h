#ifndef CUENTA_H
#define CUENTA_H

#include <vector>

/**Hay tres elementos que Hacienda necesita:
1. Cat�logo de cuentas utilizado en el per�odo
2. Balanza de comprobaci�n que incluya saldos iniciales, movimientos del periodo y
   saldos finales de cada una de las cuentas de activo, pasivo, capital y resultados
3. Informaci�n de las p�lizas generadas, incluyendo el detalle de transacci�n, cuenta,
   subcuenta y partida, as� como sus auxiliares.

   El cat�logo de cuentas debe ser enviado una sola vez y cada que sea modificado, mientras que la
   balanza de comprobaci�n se enviar� de forma mensual.*/

/**Clase que herede de "Drawable" y de "Cuenta" podr�a servir en el diagrama
Posiblemente "Cuenta" sea una base abstracta, a partir de la cual subclases implementen su interfaz.
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

/*
SMike
Diapositivas en el centro en grande
Bloat de chat arriba la derecha
Chats abajo
Video abajo a la derecha
Se les corta el video y audio
*/

/*
Presupuesto y proyecciones de rentabilidad

*reportes claros a la direcci�n en tiempo
*an�lisis con informaci�n correcta

*Baja los FDIS a excel y usa tablas din�micas, ventas por cliente, por concepto, lo cuadra con su estado de resultados
*C�mo calculas un presupuesto? Qu� busco? D�nde estoy? Tengo que solicitar financiamiento? Cu�ndo se tiene que hacer?
*PAE Procedimiento administrativo de ejecuci�n - b�sicamente el SAT te la atora
* Un error com�n es a los ingresos y gastos agregarle un 5% y ya
*Situaciones financieras normales: no tenemos liquidez, no somos rentables
*"Los impuestos son una arga financiera por su pago y el gasto de su control"
*Qu� respondes si te preguntan cu�l es tu margen de utilidad?
*Se determina en un estado de resultados?
*"el costo de ventas como basurero"?
*Que el cliente recoja su mercanc�a y doy descuento. Si mi margen era del 20% "meto en problemas a la empresa"?
*Utilidades decrecientes. Muchas veces no consideramos todos los factores
*Reglas 2.7.2
*Pago de gastos, pago de proveedores,  inversiones, impuestos, intereses, bancos
*Impuestos sobre la utilidad (ISR, PTU, IVA), sobre el flujo, sobre la renta.
*Cu�l es el flujo de efectivo de la semana? : Saldo inicial del banco, ... , cartera vencida- pago de gastos, pago
 de proveedores, pago
*Cu�l es el presupuesto de cobranzas de esta semana o mes?
*Cu�l es el margen de utilidad por producto?
*Cu�l es mi punto de equilibrio en ventas en dinero?
*Cu�l es la meta de ventas de este mes
*Cu�nto importan mis gastos fijos mensuales?

Planeaci�n.
"Lo que no se puede medir no se puede mejorar"
*Costo fiscal, costo contable, falta medir mermas, robos...
*No podemos resolver problemas actuales con herramientas del pasado
*Ataquemos el origen no el efecto
*Establecer variables pertinentes
*Operacionalizar objetivos, metas y estrategias, desarrollando un plan de utilidades
*Buscar (obviamente) reducir costos, aumentar ventas, aumentar precio de venta
*De igual modo, necesitas un plan de desarrollo de SW

Test:
Nuestra empresa maneja presupuestos?
Manejamos flujos de efectivo presupuestados?
Contamos con estados financieros mensuales confiables?
Con qu� periodicidad realizamos nuestras conciliaciones bancarias? diarias (correctamente)
  Sus cuentas por cobrar no estar�an actualizadas si no lo hicieran.
Contamos con un sistema de costos? Gob te obliga a llevar una valuaci�n de la mercanc�a por fecha, por producto...
  Qu� conceptos s� entran y cu�les no al costeo? Costo de ventas o de servicios es muy importante? Maru: costo de producci�n
  Laurita: El costo de producci�n est� dentro del costo de ventas.
Conocemos nuestro margen de utilidad? Mart�n: "S� pero no" -> Global pero no por art�culo ni familia
Manejamos tabuladores de sueldos? S� manejamos
Tenemos implementado un presupuesto de sueldos y compensaciones por departamento?
Manejamos el indicador de rotaci�n de personal? S�. Cuesta. Capacitar, echar a perder, etc.
Realizamos planeaci�n estrat�gica para 2016? No.
Conocemos nuestro break even o punto de equilibrio? S�. Actualmente 180 toneladas (Maru)

C�mo se hace un presupuesto de ventas?
Gastos fijos / margen de utilidad? Investiga

-Premisas de los presupuestos-
Implican un cambio de pensamiento y cultura. (Ojo)
Es un trabajo continuo
Es una forma de vida empresarial
Requiere del total apoyo de direcci�n
Significa el mapa en el viaje que emprende la empresa
El presupuesto es un trabajo en equipo

Bonos por cobrar? suelen no existir, pero s� hay bonos por ventas
Presupuesto maestro.
1. Conocimiento de la empresa
2. Exposici�n del plan o pol�tica (Manual del presupuesto -> bases de su elaboraci�n)
3. Coordinaci�n para la ejecuci�n de dicho plan. El gerente responsable sincroniza las actividades
4. Fijaci�n del per�odo presupuestal. Considerando la naturaleza de las operaciones
5. Direcci�n y vigilancia
6. Respaldo directivo

pto de operaci�n
Todo parte del presupuesto de ventas. Lo hace direcci�n y el director de ventas. Es el eje de los dem�s presupuestos
Pto de producci�n
Pto de compra
Pto de pagos
Pto de costo ventas
pto de inversiones
pto de costos fijos
pto de costos variables

pto financiero
pto de financiamientos
pto de inversiones
flujo de caja

estados financieros proyectados
estoadosde posici�n financiera proyectado
Estado de resultados proyectado

Presupuesto de ventas:
Los factores que lo afectan {
  Espec�ficos de ventas
  De fuerzas econ�micas generales (precio barril petr�leo eg)

}
Factores de ajuste, de cambio, corrientes de crecimiento
de ajuste: Por causas fortuitas o de fuerza mayor influyen en la predeterminaci�n de las ventas o proyectos
de cambio: modificaciones que van a efectuarse y que influir�n en las ventas:
  eg. cambio de material, de productos, presentaci�n, redise�os, etc.
  cambio de producci�n. Cambio de instalaciones
  cambio de mercados

Pv = (V+F)*E*A
V = Ventas del a�o anterior
F = Factores espec�ficos de ajuste

Grav�menes adicionales para equilibrar el producto chino con el nacional

Pto de producci�n.
Especifica la cantidad de art�culos a fabricar que se planea
Requiere un presupuesto de ventas en unidades

Definici�n del precio del producto
Qu� es el coeficiente de elasticidad precio-demanda?





*/

#endif // CUENTA_H
