#ifndef CUENTA_H
#define CUENTA_H

#include <vector>

/**Hay tres elementos que Hacienda necesita:
1. Catálogo de cuentas utilizado en el período
2. Balanza de comprobación que incluya saldos iniciales, movimientos del periodo y
   saldos finales de cada una de las cuentas de activo, pasivo, capital y resultados
3. Información de las pólizas generadas, incluyendo el detalle de transacción, cuenta,
   subcuenta y partida, así como sus auxiliares.

   El catálogo de cuentas debe ser enviado una sola vez y cada que sea modificado, mientras que la
   balanza de comprobación se enviará de forma mensual.*/

/**Clase que herede de "Drawable" y de "Cuenta" podría servir en el diagrama
Posiblemente "Cuenta" sea una base abstracta, a partir de la cual subclases implementen su interfaz.
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

*reportes claros a la dirección en tiempo
*análisis con información correcta

*Baja los FDIS a excel y usa tablas dinámicas, ventas por cliente, por concepto, lo cuadra con su estado de resultados
*Cómo calculas un presupuesto? Qué busco? Dónde estoy? Tengo que solicitar financiamiento? Cuándo se tiene que hacer?
*PAE Procedimiento administrativo de ejecución - básicamente el SAT te la atora
* Un error común es a los ingresos y gastos agregarle un 5% y ya
*Situaciones financieras normales: no tenemos liquidez, no somos rentables
*"Los impuestos son una arga financiera por su pago y el gasto de su control"
*Qué respondes si te preguntan cuál es tu margen de utilidad?
*Se determina en un estado de resultados?
*"el costo de ventas como basurero"?
*Que el cliente recoja su mercancía y doy descuento. Si mi margen era del 20% "meto en problemas a la empresa"?
*Utilidades decrecientes. Muchas veces no consideramos todos los factores
*Reglas 2.7.2
*Pago de gastos, pago de proveedores,  inversiones, impuestos, intereses, bancos
*Impuestos sobre la utilidad (ISR, PTU, IVA), sobre el flujo, sobre la renta.
*Cuál es el flujo de efectivo de la semana? : Saldo inicial del banco, ... , cartera vencida- pago de gastos, pago
 de proveedores, pago
*Cuál es el presupuesto de cobranzas de esta semana o mes?
*Cuál es el margen de utilidad por producto?
*Cuál es mi punto de equilibrio en ventas en dinero?
*Cuál es la meta de ventas de este mes
*Cuánto importan mis gastos fijos mensuales?

Planeación.
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
Con qué periodicidad realizamos nuestras conciliaciones bancarias? diarias (correctamente)
  Sus cuentas por cobrar no estarían actualizadas si no lo hicieran.
Contamos con un sistema de costos? Gob te obliga a llevar una valuación de la mercancía por fecha, por producto...
  Qué conceptos sí entran y cuáles no al costeo? Costo de ventas o de servicios es muy importante? Maru: costo de producción
  Laurita: El costo de producción está dentro del costo de ventas.
Conocemos nuestro margen de utilidad? Martín: "Sí pero no" -> Global pero no por artículo ni familia
Manejamos tabuladores de sueldos? Sí manejamos
Tenemos implementado un presupuesto de sueldos y compensaciones por departamento?
Manejamos el indicador de rotación de personal? Sí. Cuesta. Capacitar, echar a perder, etc.
Realizamos planeación estratégica para 2016? No.
Conocemos nuestro break even o punto de equilibrio? Sí. Actualmente 180 toneladas (Maru)

Cómo se hace un presupuesto de ventas?
Gastos fijos / margen de utilidad? Investiga

-Premisas de los presupuestos-
Implican un cambio de pensamiento y cultura. (Ojo)
Es un trabajo continuo
Es una forma de vida empresarial
Requiere del total apoyo de dirección
Significa el mapa en el viaje que emprende la empresa
El presupuesto es un trabajo en equipo

Bonos por cobrar? suelen no existir, pero sí hay bonos por ventas
Presupuesto maestro.
1. Conocimiento de la empresa
2. Exposición del plan o política (Manual del presupuesto -> bases de su elaboración)
3. Coordinación para la ejecución de dicho plan. El gerente responsable sincroniza las actividades
4. Fijación del período presupuestal. Considerando la naturaleza de las operaciones
5. Dirección y vigilancia
6. Respaldo directivo

pto de operación
Todo parte del presupuesto de ventas. Lo hace dirección y el director de ventas. Es el eje de los demás presupuestos
Pto de producción
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
estoadosde posición financiera proyectado
Estado de resultados proyectado

Presupuesto de ventas:
Los factores que lo afectan {
  Específicos de ventas
  De fuerzas económicas generales (precio barril petróleo eg)

}
Factores de ajuste, de cambio, corrientes de crecimiento
de ajuste: Por causas fortuitas o de fuerza mayor influyen en la predeterminación de las ventas o proyectos
de cambio: modificaciones que van a efectuarse y que influirán en las ventas:
  eg. cambio de material, de productos, presentación, rediseños, etc.
  cambio de producción. Cambio de instalaciones
  cambio de mercados

Pv = (V+F)*E*A
V = Ventas del año anterior
F = Factores específicos de ajuste

Gravámenes adicionales para equilibrar el producto chino con el nacional

Pto de producción.
Especifica la cantidad de artículos a fabricar que se planea
Requiere un presupuesto de ventas en unidades

Definición del precio del producto
Qué es el coeficiente de elasticidad precio-demanda?





*/

#endif // CUENTA_H
