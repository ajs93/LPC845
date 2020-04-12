/**
 * @file HAL_SYSCON.h
 * @brief Declaraciones a nivel de aplicacion del periferico SYSCON (LPC845)
 * @author Augusto Santini
 * @date 6/2019
 * @version 1.0
 */

/**
 * @defgroup SYSCON Configuración del Sistema (SYSCON)
 *
 * # Introducción
 *
 * Este periférico es el encargado de manejar múltiples características del sistema del microcontrolador. Entre
 * ellas se encuentran:
 * 		- Control de clocks
 * 		- Control de reset/clocks de periféricos
 * 		- Selección de pines para interrupciones externas (Ver @ref PININT)
 * 		- Configuración de modos de bajo consumo del microcontrolador
 * 		- Configuración de wake-up para salir de los modos de bajo consumo del microcontrolador
 * 		- Configuración del Brown-Out Detector
 * 		- Configuración del Micro Trace Buffer
 * 		- Control de interrupción de latencias
 * 		- Control de Non Maskable Interrupt
 * 		- Valor de calibración del @ref SYSTICK
 * 		.
 *
 * En la capa de abstracción de aplicación (HAL) se implementan principalmente las funciones relacionadas con los
 * tres primeros ítems hasta el momento.
 *
 * # Control de clocks
 *
 * ## Fuentes de clock
 *
 * Las distintas fuentes de clock que se explican a continuación, son seleccionables mediante el \e SYSCON. Algunos
 * necesitan una referencia para funcionar, mientras que otros funcionan sin necesidad de ninguna.
 *
 * ### Free Running Oscillator (FRO)
 *
 * Este oscilador es con el que comienza el microcontrolador por defecto luego de un reset. La frecuencia del
 * mismo se puede configurar (<em>no implementado todavia</em>) pero por default comienza en \f$24MHz\f$ con
 * un divisor \f$/2\f$, resultando en una frecuencia efectiva de \f$12MHz\f$.
 *
 * ### Phase Locked Loop (PLL)
 *
 * Este oscilador toma una frecuencia de entrada y genera una conversión para obtener una frecuencia efectiva
 * mayor a la de entrada. La frecuencia de entrada mínima del mismo es de \f$10MHz\f$.
 *
 * ### Main/System clock
 *
 * El clock principal o de sistema (estos nombres se usan indistintamente) genera la frecuencia base de la cual
 * se derivan la mayoría de los periféricos. El mismo puede ser tomado de la señal de salida del \e PLL o de la
 * señal previa al \e PLL. Este clock es el que provee la frecuencia del núcleo del microcontrolador.
 *
 * ### Clock externo
 *
 * El clock externo puede ser de utilidad cuando se tiene una referencia de frecuencia de muy buena estabilidad
 * externa al microcontrolador. Casos típicos son <em>cristales</em>, u osciladores de alta presición y bajo drift.
 * Si se utiliza un <em>cristal externo</em> se utilizarán los pines P0_8 y P0_9 como fuentes de entrada para el
 * circuito oscilador interno, el cual se encargará de generar la frecuencia de clock correspondiente, mientras que
 * si se utiliza un oscilador externo, se utilizará únicamente el pin P0_1.
 *
 * ### Generadores fraccionales de clock
 *
 * El microcontrolador dispone de dos <em>Generadores fraccionales de clock</em>. Los mismos son de gran utilidad
 * cuando se necesita tener presición en la frecuencia de algún periférico y la frecuencia del clock principal
 * con los divisores del periférico no nos alcanzas para lograr dicha presición. La ventaja de estos generadores
 * es que toman una frecuencia de referencia, y generan una división fraccional del mismo. El divisor de estos
 * generadores varía entre \f$1 y 2\f$ con valores decimales, pudiendo así lograr frecuencias que los demás
 * divisores no pueden, dada su naturaleza de división entera.
 *
 * Periféricos que pueden ser excitados mediante estos generadores:
 * 		- @ref UART
 * 		- @ref SPI
 * 		- @ref IIC
 * 		.
 *
 * ### Oscilador del <em>Watchdog</em>
 *
 * El periférico @ref WATCHDOG tiene su propia fuente de clock. Este oscilador es de ultra bajo consumo, pero su
 * presición es de \f$\pm40\%\f$. El oscilador puede tener como base una variedad de valores y también tiene su
 * propio divisor, logrando rangos entre \f$9.3KHz\f$ y \f$2.3MHz\f$.
 *
 * ## Divisores de clock
 *
 * Ciertos periféricos permiten dividir la frecuencia de clock principal o de otra fuente, para así tener un menor
 * consumo de energía, o para cumplir con especificaciones de frecuencia que requiera el periférico particularmente.
 *
 * ### Divisor del clock principal
 *
 * La frecuencia del clock principal puede ser dividida por cualquier número entero entre 1 y 255. Para
 * situaciones donde la velocidad de procesamiento no sea una necesidad primordial, se puede reducir la
 * misma mediante este divisor. Como este divisor afecta al clock principal, y la mayoría de los periféricos
 * corren con un clock asociado al mismo, se verá reducido el consumo notablemente, a expensas de reducir
 * la velocidad de procesamiento.
 *
 * @note La función para el control de este divisor es @ref hal_syscon_set_system_clock_divider.
 *
 * ### Divisor del clock del ADC
 *
 * El clock de la lógica del periférico @ref ADC es alimentado por el clock principal luego de pasar por este
 * divisor. El divisor puede ser configurado en cualquier valor entero entre 0 y 255. Si se coloca en 0, el
 * clock del \e ADC será anulado. Nótese que la configuración necesaria del divisor es realizada en las
 * funciones de inicialización del \e ADC.
 *
 * @note El periférico @ref ADC se ocupará de configurar su divisor en caso de ser necesario. No se proveen
 * funciones en este módulo para la configuración del mismo.
 *
 * ### Divisor del clock del SCT
 *
 * Al igual que con el \e ADC, el clock de la lógica del periférico @ref SCT es alimentado por el clock principal
 * luego de pasar por este divisor. El divisor puede ser configurado en cualquier valor entero entre 0 y 255. Si se
 * coloca en 0, el clock del \e SCT será anulado.
 *
 * @note Este periférico, así como las funciones para configurar su divisor en el \e SYSCON, todavía no
 * está implementado en la librería.
 *
 * ### Divisor de la salida CLKOUT
 *
 * En caso de ser necesario, el periférico \e SYSCON puede ser configurado para general una salida en uno de los
 * pines del microcontrolador que esté relacionada a algunas fuentes de clock. Antes de salir por el pin, la señal
 * pasa por un divisor, el cual puede ser configurado en cualquier valor entero entre 0 y 255. Si el mismo es
 * configurado en 0, la salida es anulada. Fuentes de clock disponibles para la salida CLKOUT:
 * 		- FRO
 * 		- Clock principal
 * 		- PLL
 * 		- Clock externo (Cristal/entrada externa)
 * 		- Oscilador del watchdog
 * 		.
 *
 * @note La función para el manejo de la salida \e CLKOUT es @ref hal_syscon_config_clkout.
 *
 * ### Divisores para el filtro de Glitches del IOCON
 *
 * El periférico @ref IOCON tiene la posibilidad de ser configurado para eliminar glitches en entradas mediante
 * un filtrado por hardware. Dicho filtro de glitches puede ser configurado para tomar su señal de excitación de
 * uno de los siete divisores que tiene el \e SYSCON reservados para este fin. El valor de estos divisores puede
 * ser cualquier valor entero entre 0 y 255. Si se configura como 0, el divisor no generará señal de excitación,
 * anulando así la funcionalidad.
 *
 * @note La función para el manejo de los divisores de filtros de glitches es
 * @ref hal_syscon_set_iocon_glitch_divider.
 *
 * @{
 */

/**
 * @example Ejemplo_SYSCON.c
 *
 * El programa configura el FRO para funcionar sin divisor y con la frecuencia configurada por default, obteniendo
 * una frecuencia de funcionamiento del mismo de 24MHz. Además, selecciona el FRO como clock principal de sistema.
 *
 * Luego configura la salida CLKOUT para salir en el Puerto 0 ; Pin 18 mediante un divisor por 24, obteniendo una
 * frecuencia de salida en el pin Puerto 0 ; Pin 18 de 1MHz.
 *
 * La única forma de medir dicha salida es mediante un osciloscopio.
 */

#ifndef HAL_SYSCON_H_
#define HAL_SYSCON_H_

#include <stdint.h>
#include <HAL_GPIO.h>

/** Selección de fuente de clock para la salida CLKOUT */
typedef enum
{
	HAL_SYSCON_CLKOUT_SOURCE_SEL_FRO = 0, /**< Free Running Oscillator */
	HAL_SYSCON_CLKOUT_SOURCE_SEL_MAIN_CLOCK, /**< Clock principal */
	HAL_SYSCON_CLKOUT_SOURCE_SEL_SYS_PLL, /**< Phase Locked Loop */
	HAL_SYSCON_CLKOUT_SOURCE_SEL_EXT_CLOCK, /**< Clock externo */
	HAL_SYSCON_CLKOUT_SOURCE_SEL_WATCHDOG_OSC /**< Watchdog Oscillator */
}hal_syscon_clkout_source_sel_en;

/**< Selección de fuente de clock para los generadores fraccionales de clock */
typedef enum
{
	HAL_SYSCON_FRG_CLOCK_SEL_FRO = 0, /**< Free Running Oscillator */
	HAL_SYSCON_FRG_CLOCK_SEL_MAIN_CLOCK, /**< Clock principal */
	HAL_SYSCON_FRG_CLOCK_SEL_SYS_PLL, /**< Phase Locked Loop */
	HAL_SYSCON_FRG_CLOCK_SEL_NONE /**< Ninguno */
}hal_syscon_frg_clock_sel_en;

/** Selección de periférico para controlar fuente de clock */
typedef enum
{
	HAL_SYSCON_PERIPHERAL_SEL_UART0 = 0, /**< UART0 */
	HAL_SYSCON_PERIPHERAL_SEL_UART1, /**< UART1 */
	HAL_SYSCON_PERIPHERAL_SEL_UART2, /**< UART2 */
	HAL_SYSCON_PERIPHERAL_SEL_UART3, /**< UART3 */
	HAL_SYSCON_PERIPHERAL_SEL_UART4, /**< UART4 */
	HAL_SYSCON_PERIPHERAL_SEL_IIC0, /**< IIC0 */
	HAL_SYSCON_PERIPHERAL_SEL_IIC1, /**< IIC1 */
	HAL_SYSCON_PERIPHERAL_SEL_IIC2, /**< IIC2 */
	HAL_SYSCON_PERIPHERAL_SEL_IIC3, /**< IIC3 */
	HAL_SYSCON_PERIPHERAL_SEL_SPI0, /**< SPI0 */
	HAL_SYSCON_PERIPHERAL_SEL_SPI1 /**< SPI1 */
}hal_syscon_peripheral_sel_en;

/** Selección de fuente de clock para los periféricos */
typedef enum
{
	HAL_SYSCON_PERIPHERAL_CLOCK_SEL_FRO = 0, /**< Free Running Oscillator */
	HAL_SYSCON_PERIPHERAL_CLOCK_SEL_MAIN, /**< Clock principal */
	HAL_SYSCON_PERIPHERAL_CLOCK_SEL_FRG0, /**< Generador fraccional 0 */
	HAL_SYSCON_PERIPHERAL_CLOCK_SEL_FRG1, /**< Generador fraccional 1 */
	HAL_SYSCON_PERIPHERAL_CLOCK_SEL_FRO_DIV, /**< Free Running Oscillator dividido por 2 */
	HAL_SYSCON_PERIPHERAL_CLOCK_SEL_NONE = 7 /**< Ninguna */
}hal_syscon_peripheral_clock_sel_en;

/** Selección de banco de división para filtros de glitches */
typedef enum
{
	HAL_SYSCON_IOCON_GLITCH_SEL_0 = 0, /**< Banco 0 */
	HAL_SYSCON_IOCON_GLITCH_SEL_1, /**< Banco 1 */
	HAL_SYSCON_IOCON_GLITCH_SEL_2, /**< Banco 2 */
	HAL_SYSCON_IOCON_GLITCH_SEL_3, /**< Banco 3 */
	HAL_SYSCON_IOCON_GLITCH_SEL_4, /**< Banco 4 */
	HAL_SYSCON_IOCON_GLITCH_SEL_5, /**< Banco 5 */
	HAL_SYSCON_IOCON_GLITCH_SEL_6, /**< Banco 6 */
	HAL_SYSCON_IOCON_GLITCH_SEL_7 /**< Banco 7 */
}hal_syscon_iocon_glitch_sel_en;

/** Fuente de clock para el PLL */
typedef enum
{
	HAL_SYSCON_PLL_SOURCE_SEL_FRO = 0, /**< Free Running Oscillator */
	HAL_SYSCON_PLL_SOURCE_SEL_EXT_CLK, /**< Clock externo */
	HAL_SYSCON_PLL_SOURCE_SEL_WATCHDOG, /**< Watchdog Oscillator */
	HAL_SYSCON_PLL_SOURCE_SEL_FRO_DIV /**< Free Running Oscillator dividido por 2 */
}hal_syscon_pll_source_sel_en;

/**
 * @brief Obtener la frecuencia actual del main clock
 * @return Frecuencia del main clock en Hz
 */
uint32_t hal_syscon_get_system_clock(void);

/*
 * @brief Fijar el divisor del clock principal
 * @param[in] div Divisor deseado. Cero inhabilita el clock principal
 */
void hal_syscon_set_system_clock_divider(uint8_t div);

/**
 * @brief Obtener la frecuencia actual del FRO
 * @return Frecuencia del FRO en Hz
 */
uint32_t hal_syscon_get_fro_clock(void);

/**
 * @brief Configurar el ext clock a partir de un cristal externo
 * @param[in] crystal_freq Frecuencia del cristal externo utilizado
 * @param[in] use_as_main Si es distinto de cero, se utilizara el oscilador a cristal como main clock
 */
void hal_syscon_config_external_crystal(uint32_t crystal_freq, uint8_t use_as_main);

/**
 * @brief Configurar el clock FRO
 * @param[in] direct Si es distinto de cero se omite el divisor del FRO
 * @param[in] use_as_main Si es distinto de cero, se utilizará el FRO como main clock
 */
void hal_syscon_config_fro_direct(uint8_t direct, uint8_t use_as_main);

/**
 * @brief Configurar el pin de clock out (salida de clock hacia afuera)
 * @param[in] portpin Número de puerto/pin por donde sacar el clock out
 * @param[in] clock_source Fuente deseada para la salida clock out
 * @param[in] divider Divisor deseado para la salida clock out
 */
void hal_syscon_config_clkout(hal_gpio_portpin_en portpin, hal_syscon_clkout_source_sel_en clock_source, uint8_t divider);

/**
 * @brief Configurar el divisor fraccional
 *
 * El divisor siempre se debe fijar en 256 para estos MCU.
 *
 * @param[in] inst Instancia de FRG a configurar
 * @param[in] clock_source Fuente de clock de entrada para el FRG
 * @param[in] mul Multiplicador deseado
 */
void hal_syscon_config_frg(uint8_t inst, hal_syscon_frg_clock_sel_en clock_source, uint32_t mul);

/**
 * @brief Obtener la frecuencia de clock en Hz configurada para cierto periférico
 * @param[in] peripheral Periférico deseado
 * @return Frecuencia en Hz del clock del periférico
 */
uint32_t hal_syscon_get_peripheral_clock(hal_syscon_peripheral_sel_en peripheral);

/**
 * @brief Configurar divisor para el clock de glitches del IOCON
 * @param[in] sel Selección de divisor
 * @param[in] div Valor de división deseado
 */
void hal_syscon_set_iocon_glitch_divider(hal_syscon_iocon_glitch_sel_en sel, uint32_t div);

/**
 * @brief Configurar el PLL
 * @param[in] clock_source Fuente de clock de referencia para el PLL
 * @param[in] freq Frecuencia deseada de salida del PLL
 */
void hal_syscon_config_pll(hal_syscon_pll_source_sel_en clock_source, uint32_t freq);

/**
 * @brief Obtener frecuencia actual configurada del PLL
 * @return Frecuencia actual del PLL en Hz
 */
uint32_t hal_syscon_get_pll_clock(void);

#endif /* HAL_SYSCON_H_ */

/**
 * @}
 */
