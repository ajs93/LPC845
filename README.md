# Página principal # {#mainpage}

# Introducción

Esta librería esta diseñada para ser utilizada con la línea de microcontroladores **LPC845** de la firma **NXP**.
En particular, la misma actualmente está siendo desarrollada para el **LPC845** en encapsulado **QFP48** teniendo en
cuenta que el mismo se encuentra en el stick de desarrollo LPC845\_BRK*.

El desarrollo está siendo orientado hacia la mayor flexibilidad posible para el usuario final, pudiendo el mismo
ser un usuario con diferentes grados de conocimiento en programación orientada a sistemas embebidos como así 
también distintos intereses a la hora del alcance que busca en una librería.

-----------------------------------------------------------------------------------------------------------------

# Estructura de la librería

La librería se divide en tres capas de abstracción para aportar la mayor flexibilidad posible como fue explicado 
en la sección *Introducción*. Es recomendable que el usuario utilice la misma capa de abstracción para la utilización 
de la librería. En caso de no ser posible, es recomendable que al menos para cada periférico se utilice una 
única capa de abstracción. Estas tres capas están explicados a continuación.

> Toda la documentación acerca de los distintos periféricos implementados en la documentación de la 
> librería, están referidos a la *Capa de aplicación*, pero se explica la estructura de la librería 
> dado que usuarios más avanzados en el campo de sistemas embebidos puedan llegar a utilizar con mayor 
> frecuencia las dos capas inferiores.

## Capa de abstracción de hardware (HAL)

En esta capa están definidas todas las funciones necesarias para utilizar los periféricos sin necesidad de 
manejar los registros del microcontrolador. Esto implica ciertas restricciones en la funcionalidad de los 
distintos periféricos, dadas por las implementaciones propuestas en la librería.

## Capa de abstracción de hardware (HPL)

En esta capa están definidas todas las funciones para acceder a los distintos registros del microcontrolador 
de forma "humanamente legible". En caso de necesitar configuraciones particulares o no dispuestas en la *
Capa de aplicación*, se deberá utilizar este nivel de abstracción. Esta capa tiene implementaciones con 
funcionalidades mínimas.

## Capa de registros de hardware (HRI)

En esta capa están definidas todas las estructuras y direcciones necesarias para acceder a los distintos 
registros del microcontrolador en forma "directa". En caso de necesitar configuraciones o accesos no explicitados
en la *Capa de abstracción de hardware* se deberá utilizar esta capa.

-----------------------------------------------------------------------------------------------------------------

# Acerca del stick de desarrollo LPC845_BRK {#acerca_del_stick}

![Pinout del LPC845_BRK](https://raw.githubusercontent.com/ajs93/LPC845/develop_augusto/Imagenes/LPC845_BRK_PINOUT.png)

El stick de desarrollo utilizado a lo largo del desarrollo de esta librería, también utilizado para la 
demostración de todos los ejemplos adjuntos, es el indicado en la imágen superior. El mismo viene dotado 
de varios componentes útiles a la hora de desarrollar un proyecto, evitándonos en gran medida realizar 
nuestro propio hardware, ahorrándonos así tiempo de desarrollo causado por errores en el desarrollo del 
hardware, fabricación de prototipos y demás. Dichos componentes son:
- LED RGB: LEDs *rojo*, *verde* y *azul* en un mismo sustrato. Los tres LEDs se encuentran en paralelo, 
por lo que **no es posible encender más de un LED a la vez**. Estos LEDs permiten pruebas de salidas con 
el periférico *GPIO*. Los puertos/pines de los mismos son:
	- Rojo: Puerto 1 ; Pin 2
	- Azul: Puerto 1 ; Pin 1
	- Verde: Puerto 1 ; Pin 0
- Preset de una vuelta sin tope: Resistor variable de tres terminales, muy útil para pruebas de *ADC*. 
Ubicado en:
	- Puerto 0 ; Pin 7
- Tres pulsadores:
	- Pulsador de usuario: Útil para pruebas relacionadas con lectura de *GPIO* interrupciones de pin
	*PININT*. El pulsador de usuario también se encuentra en un pin disponible para despertar al 
	microcontrolador de modos de funcionamiento de bajo consumo. Ubicado en: Puerto 0 ; Pin 4
	- Pulsador de reset: Con este pulsador se puede generar un *RESET* por hadrware en caso de 
	habilitar dicha función en el pin correspondiente. En caso de no habilitar dicha función, se puede 
	utilizar el pulsador como un pulsador normal. Ubicado en: Puerto 0 ; Pin 5
	- Pulsador para modo ISP: La principal utilidad de este pulsador, es en conjunto con el de reset, para 
	entrar a modo de programación ISP, para recuperar el microcontrolador de ciertos estados de falla.
- Conexión con un emulador de puerto serie: El chip que hace las veces de programador en el stick de 
desarrollo, cuenta con el software necesario para emular un puerto serie en un puerto USB. Esto es una 
gran ventaja, dado que no necesitamos ninguna interfaz adicional al stick de desarrollo para realizar 
pruebas con el periférico *UART*. Ubicación de pines en:
	- Pin RX (Receptor del emulador): Puerto 0 ; Pin 25
	- Pin TX (Transmisor del emulador): Puerto 0 ; Pin 24
- Pad táctil: El stick de desarrollo tiene en su borde opuesto al puerto USB, un pad táctil. El mismo 
puede ser utilizado por el microcontrolador y el periférico *CAPTOUCH*.

Cabe destacar que ciertos pines no estén disponibles o tengan componentes conectados, por lo cual es 
altamente recomendable revisar las conexiones en cada pin a utilizar, en el esquemático del stick de 
desarrollo.

-----------------------------------------------------------------------------------------------------------------

# Utilización de la librería en proyectos MCUXpresso

Existen básicamente dos formas de utilizar la librería.

## Compilación de librería en proyecto externo

En ciertos casos es deseable en el proyecto a trabajar, que la librería esté compilada previamente en un 
proyecto separado, para tener mayor control entre los distintos proyectos que utilicen la librería, y para un 
mayor encapsulamiento de las funciones. Con esta estrategia, cualquier cambio realizado en la librería, afectará 
a todos los proyectos que la utilicen. Para utilizar esta estrategia, seguir los siguientes pasos:
1. Compilar la librería en un proyecto de tipo <b>Librería estática</b>
2. En el proyecto que desee utilizar la librería, configurar las siguientes propiedades bajo las propiedades 
C/C++ Build -> Settings:
	- *MCU C Compiler -> Includes -> Include paths*: Indicar el directorio a la capa de abstracción a 
	utilizar del proyecto de librería estática ya compilado correctamente.
	Ejemplo: "${workspace_loc:/Libreria_LPC845/includes/hal}"
	- *MCU Linker -> Libraries -> Libraries*: Indicar el nombre de la librería compilada.
	Ejemplo: "Libreria_LPC845"
	- *MCU Linker -> Libraries -> Library search path*: Indicar el directorio donde la librería 
	estática haya sido compilada. Dicho directorio dependerá de si la compilación de la librería fue hecha 
	en modo *Release* o *Debug*. Ejemplo: "${workspace_loc:/Libreria_LPC845/Debug}"
3. En este punto debería poder utilizar la librería sin ningún problema

Las opciones de compilación de la librería como pueden ser niveles de optimización, quedan en este caso a 
cargo del usuario y son modificables en las configuraciones del proyecto de la librería.

## Agregado de archivos fuentes de librería necesarios

Si es necesario agregar/quitar funcionalidades de la librería, o se desea cambiar implementaciones ya 
realizadas, solo es necesario agregar los archivos de cabecera y fuentes necesarios al proyecto donde se va a 
trabajar. Esta opción permite una personalización de la librería, así como utilización de menos espacio de 
memoria de código, dado que el compilador probablemente descarte todas las funciones que no sean utilizadas.

-----------------------------------------------------------------------------------------------------------------

# Documentación completa de la libreria

La documentación completa de la librería se encuentra en [Documentación librería](https://github.com/ajs93/LPC845/blob/master/Libreria_LPC845/doc/refman.pdf) 
únicamente en la rama master
