/**
 * @file HAL_ADC.h
 * @brief Declaraciones a nivel de aplicación del periférico ADC (LPC845)
 * @author Augusto Santini
 * @author Esteban E. Chiama
 * @date 3/2020
 * @version 1.0
 */

/**
 * @defgroup ADC Conversor analógico a digital (ADC)
 *
 * # Introducción
 *
 * Este periférico como su nombre lo indica, convierte una o más entradas analógicas, a un
 * valor equivalente digital. En el caso del LPC845, tiene un único módulo *ADC* con una
 * resolución de 12 bits, el cual tiene 12 canales, lo cual implica que se pueden realizar
 * conversiones de 12 fuentes analógicas distintas, pero no así realizar conversiones
 * *al mismo tiempo*. En caso de querer tomar señales de múltiples fuentes analógicas, se
 * deberán hacer sucesivas conversiones en los distintos canales deseados.
 *
 * Una resolución de 12 bits implica que la conversión aumentará cada unidad siguiendo la siguiente ecuación:
 *
 * \f{eqnarray*}{
 * 		ADC_{res} = \frac{V_{ref_{p}}}{2^N}
 * \f}
 *
 * Siendo N la cantidad de bits disponibles en la conversión. Esto implica que podemos preveer el valor
 * resultante de la conversión analógica/digital mediante la siguiente ecuación:
 *
 * \f{eqnarray*}{
 * 		ADC_{conv} = \frac{V_{ADC_{in}}}{ADC_{res}}
 * \f}
 *
 * @note Cabe destacar, que las conversiones serán redondeadas **siempre** hacia abajo, es decir, se descartan los
 * valores decimales.
 *
 * # Concepto de Secuencia de conversión
 *
 * Para el *ADC* de este microcontrolador, un inicio de conversión en realidad puede implicar el inicio de
 * una *secuencia de conversión*. Dicha secuencia puede implicar uno o más canales a convertir, y puede
 * generar eventos tanto cuando se termina la secuencia completa, o cuando se termina cada canal de la
 * secuencia. Asimismo los inicios de conversión pueden disparar una secuencia completa, o el próximo de los
 * canales habilitados en dicha secuencia. Se tienen dos secuencias configurables (*Secuencia A y
 * Secuencia B*), las cuales se pueden configurar de forma tal que un disparo de *Secuencia B*
 * interrumpa a una conversión actual de la *Secuencia A*.
 *
 * @note Las secuencias de conversión son configuradas mediante la función @ref hal_adc_sequence_config.
 *
 * # Inicio de conversiones
 *
 * El *ADC* de este microcontrolador permite el inicio de secuencia de conversión/canal de dos formas:
 * 		1. *Iniciadas por software*: Las secuencias de conversión son iniciadas mediante la escritura de un
 * 		registro de control del *ADC*. El software puede tener total control sobre el punto de inicio de
 * 		conversión.
 * 		2. *Iniciadas por hardware*: Las secuencias de conversión son iniciadas dependiendo de otras señales,
 * 		sean las mismas internas o externas al microcontrolador.
 * 		.
 *
 * @note En caso de disparar conversiones por software, se utiliza la función @ref hal_adc_sequence_start
 * para dicho propósito. En caso de que las conversiones sean iniciadas por hardware, no se debe llamar a
 * ninguna función, y la secuencia de conversión se disprará cuando suceda el evento configurado en la secuencia.
 *
 * # Calibración de hardware
 *
 * Este periférico contiene un bloque de autocalibración, el cual debe ser utilizado luego de cada reinicio
 * del microcontrolador o cada vez que se sale de modo de bajo consumo, para obtener la resolución y presición
 * especificada por el fabricante. La librería implementa la calibración por hardware en las funciones
 * @ref hal_adc_init_sync_mode y @ref hal_adc_init_async_mode.
 *
 * @note La autocalibración debe realizarse cuando el **microcontrolador** sale de un modo de funcionamiento
 * de bajo consumo, no cuando el periférico *ADC* sale de modo bajo consumo.
 *
 * # Modo sicnrónico/asincrónico
 *
 * En este microcontrolador, el *ADC* puede ser configurado de dos formas distintas en cuanto a sus clocks:
 * 		- Modo asincrónico: El clock que alimenta a la lógica de muestreo del periférico puede ser de una
 * 		naturaleza distinta al clock del sistema que alimenta a la lógica del periférico.
 * 		- Modo sincrónico: El clock que alimenta tanto a la lógica de muestreo del periférico como la lógica del
 * 		periférico, estan en sincronismo.
 * 		.
 *
 * @note La configuración de esta característica se realiza en la función @ref hal_adc_init_sync_mode o
 * @ref hal_adc_init_async_mode dependiendo de las necesidades del usuario.
 *
 * # Modo bajo consumo
 *
 * El periférico dispone de una funcionalidad configurable de bajo consumo. Si la misma está habilitada, en
 * cualquier momento que el *ADC* no esté realizando alguna conversión, la energía del mismo se reducirá,
 * permitiendo así tener un menor consumo. El costo de este modo de funcionamiento, es un delay extra cada vez
 * que se dispara una nueva conversión, dado que el periférico deberá salir del modo bajo consumo. Consultar el
 * manual de usuario del microcontrolador para más información.
 *
 * @note El parámetro de bajo consumo se configura en las funciones de inicialización
 * @ref hal_adc_init_sync_mode o @ref hal_adc_init_async_mode.
 *
 * # Velocidad de conversión/Frecuencia de muestreo
 *
 * Cada conversión realizada toma un tiempo que dependerá del clock configurado en el periférico. Podemos
 * obtener este tiempo de conversión mediante la ecuación:
 *
 * \f{eqnarray*}{
 * 		t_{conv_{ADC}} = \frac{1}{f_{ADC} / 25} \\
 * 		f_{muestreo_{ADC}} = \frac{1}{t_{conv_{ADC}}}
 * \f}
 *
 * La división por 25 en el denominador, es debido a la naturaleza del periférico de *aproximaciones
 * sucesivas*. Esto implica que desde que se genera un inicio de conversión hasta que la misma finaliza,
 * deben transcurrir 25 ciclos de clock del *ADC*.
 *
 * Ejemplo: Configurando el *ADC* con una frecuencia de 25MHz obtenemos el tiempo tomado por cada
 * conversión:
 *
 * \f{eqnarray*}{
 *     t_{conv_{ADC}} = \frac{1}{25MHz / 25} \\
 *     t_{conv_{ADC}} = 1 \mu s \\
 *     f_{muestreo_{ADC}} = 1 MHz
 * \f}
 *
 * Esto implica que entre un inicio de conversión y la finalización de la misma, pasará 1 microsegundo. Nótese que
 * este tiempo corresponde a una conversión para un único canal. En caso de estar convirtiendo varios canales, se
 * deberá multiplicar dicho tiempo de conversión por la cantidad de canales activos en la secuencia de
 * conversión, para obtener el tiempo total desde un inicio de secuencia de conversión y la finalización de
 * todos los canales (asumiendo que se dispara una conversión de secuencia completa).
 *
 * El *ADC* no puede convertir a cualquier frecuencia de muestreo, existen frecuencias máximas dependiendo del
 * tipo de funcionamiento configurado para el periférico:
 * 		- Funcionamiento en modo *sincrónico*: Frecuencia de muestreo máxima de 1.2MHz
 * 		- Funcionamiento en modo *asincrónico*: Frecuencia de muestreo máxima de 0.6MHz
 * 		.
 *
 * @note La frecuencia de muestreo se configura en las funciones de inicialización @ref hal_adc_init_sync_mode o
 * @ref hal_adc_init_async_mode.
 *
 * # Campos de aplicación típicos
 *
 * - Audio/Video
 * - Señales de naturaleza biológica (ECG, EEG)
 * - Entradas de usuario de hardware (Preset, Potenciómetro)
 * - Sensores con salida analógica de variables físicas (Termómetro, Luxómetro)
 *
 * # Consideraciones acerca de los callbacks
 *
 * Los callbacks asociados en las configuraciones posibles son ejecutados en el contexto de una **interrupción**,
 * por lo que el usuario deberá tener las consideraciones adecuadas a la hora de lo que realiza el callback
 * asociado. Ver @ref adc_sequence_interrupt_t y @ref adc_comparison_interrupt_t.
 *
 * @{
 */

/**
 * @example Ejemplo_ADC.c
 *
 * # Configuraciones
 *
 * El programa utiliza el clock por default con el que comienza el microcontrolador, es decir, el *Free Running
 * Oscillator* funcionando a 12MHz
 *
 * El periférico *ADC*. será configurado con las siguientes características:
 * 	- Funcionamiento **sincrónico**
 * 	- Frecuencia de muestreo de 1Mhz
 * 	- Modo bajo consumo inhabilitado
 *  .
 *
 * La secuencia A es configurada para generar conversiones en el canal 0:
 * 	- El canal 0 está conectado al preset propio del stick de desarrollo (Puerto 0 pin 7)
 * 	.
 *
 * La secuencia B es configurada para generar conversiones en los canales 4 y 8:
 * 	- El canal 4 está ubicado en el pin número 7 (Puerto 0 pin 22)
 * 	- El canal 8 está ubicado en el pin número 3 (Puerto 0 pin 18)
 * 	.
 *
 * En ambos canales de la secuencia B se puede conectar un preset con los terminales de los extremos uno a VDD y el
 * otro a VSS, y el terminal central a cada uno de los canales.
 *
 * Además, la secuencia A tendrá la siguiente configuración:
 * 	- Trigger: Únicamente se disparan conversiones por software
 * 	- Bypass sincronismo: Sí
 * 	- Modo de interrupción: Cuando termina cada canal
 * 	- Burst: Habilitado
 * 	- Un trigger dispara: Una conversión de canal
 * 	- Secuencia A como baja prioridad: Si
 * 	.
 *
 * La secuencia B tendrá la siguiente configuración:
 * 	- Trigger: Únicamente se disparan conversiones por software
 * 	- Bypass sincronismo: Sí
 * 	- Modo de interrupción: Cuando se termina la secuencia completa
 * 	- Burst: Inhabilitado
 * 	- Un trigger dispara: Una conversión de secuencia completa
 * 	.
 *
 * Una vez inicializado el periférico, se configura el periférico *Systick* para interrumpir cada 1 milisegundo
 * y mediante su manejador se lleva la cuenta de los milisegundos transcurridos. Una vez transcurridos
 * 1 segundo, se dispara una conversión de *ADC*, y sus resultados se guardan en dos variables globales.
 *
 * # Descripción de funcionamiento
 *
 * La idea de este ejemplo es demostrar las capacidades de la librería para con el periférico *ADC*. Con dicho
 * fin, se muestran las dos principales capacidades del periférico, incluidas particularidades explicadas a
 * continuación.
 *
 * ## Conversiones no continuas
 *
 * La *secuencia B* está configurada para generar conversiones disparadas por software. Esta forma es
 * usualmente utilizada cuando no es necesario tener conversiones continuas, dada la naturaleza de necesidad de
 * la aplicación a desarrollar. Sin embargo, no es la óptima para distintos casos como pueden ser, por ejemplo:
 * 		- Conversiones con tiempos precisos. En estos casos, se recomienda disparar conversiones disparadas
 * 		por algún timer, la librería tiene implementada esta configuración.
 * 		- Conversiones que dependen de variables externas. En estos casos, es útil disparar las conversiones
 * 		mediante interrupciones de *PININT* o *ACOMP*.
 * 		.
 *
 * En el caso del ejemplo, se disparan conversiones cada aproximadamente 100 milisegundos con ayuda del *Systick*.
 *
 * ## Conversiones continuas
 *
 * La *secuencia A*, al tener el modo *BURST* habilitado, genera conversiones continuamente. Dependiendo
 * de la necesidad de procesamiento de dichos resultados, tal vez es deseable que el programa no sea interrumpido
 * constantemente, dado que tal vez se necesiten condiciones muy simples de análisis, como puede ser un umbral. En
 * este ejemplo, se demuestra la potencia del periférico *ADC* al utilizar el *threshold compare*. El
 * mismo es configurado para generar interrupciones cuando el valor convertido en el *Canal 0* cruce por
 * ciertos umbrales estipulados por el usuario. Para el ejemplo, dependiendo de si la conversión que generó la
 * interrupción estaba entre los valores de umbral, o por fuera, se enciende alguno de los LEDs RGB (rojo o azul)
 * del stick de desarrollo, sin una interrupción constante por parte de la finalización de conversión de la
 * *secuencia A*.
 *
 * ## Posibilidad de interrupcion de secuencia de conversión A
 *
 * La *secuencia A* está configurada como *baja prioridad*. Esto implica que cualquier disparo de
 * conversión de *secuencia B* frenará las conversiones de la *secuencia A*, realizará la/s
 * conversiones de la *secuencia B* y luego retomará las conversiones de *secuencia A*. Como la
 * *secuencia A* está configurada para convertir continuamente, queda en real evidencia la condición de
 * baja prioridad de la *secuencia A*.
 */

#ifndef HAL_ADC_H_
#define HAL_ADC_H_

#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif

/** Selección de fuente de clock para el *ADC* */
typedef enum
{
	HAL_ADC_CLOCK_SOURCE_FRO = 0, /**< Free running oscillator como fuente de clock */
	HAL_ADC_CLOCK_SYS_PLL /**< Phase locked loop oscillator como fuente de clock */
}hal_adc_clock_source_en;

/** Selección de modo bajo consumo */
typedef enum
{
	HAL_ADC_LOW_POWER_MODE_DISABLED = 0, /**< Modo bajo consumo inhabilitado */
	HAL_ADC_LOW_POWER_MODE_ENABLED /**< Modo bajo consumo habilitado */
}hal_adc_low_power_mode_en;

/** Selección de secuencia de *ADC* */
typedef enum
{
	HAL_ADC_SEQUENCE_SEL_A = 0, /**< Secuencia A */
	HAL_ADC_SEQUENCE_SEL_B /**< Secuencia B */
}hal_adc_sequence_sel_en;

/** Fuente de trigger para el *ADC* */
typedef enum
{
	HAL_ADC_TRIGGER_SEL_NONE = 0, /**< Ninguna (trigger por software) */
	HAL_ADC_TRIGGER_SEL_PININT0_IRQ, /**< Interrupción de PININT, canal 0 */
	HAL_ADC_TRIGGER_SEL_PININT1_IRQ, /**< Interrupción de PININT, canal 1 */
	HAL_ADC_TRIGGER_SEL_SCT0_OUT3, /**< Salida 3 del SCT */
	HAL_ADC_TRIGGER_SEL_SCT0_OUT4, /**< Salida 4 del SCT */
	HAL_ADC_TRIGGER_SEL_T0_MAT3, /**< Match 3 del CTIMER */
	HAL_ADC_TRIGGER_SEL_CMP0_OUT_ADC, /**< Salida 0 del comparador analógico */
	HAL_ADC_TRIGGER_SEL_GPIO_INT_BMAT, /**< Pattern match */
	HAL_ADC_TRIGGER_SEL_ARM_TXEV /**< Señal TXEV causada por una instrucción SEV */
}hal_adc_trigger_sel_en;

/** Selección de polaridad del trigger del *ADC* */
typedef enum
{
	HAL_ADC_TRIGGER_POL_SEL_NEGATIVE_EDGE = 0, /**< Flanco negativo */
	HAL_ADC_TRIGGER_POL_SEL_POSITIVE_EDGE /**< Flanco positivo */
}hal_adc_trigger_pol_sel_en;

/** Selección de sincronismo en secuencia del *ADC* */
typedef enum
{
	HAL_ADC_SYNC_SEL_ENABLE_SYNC = 0, /**< Habilitación de sincronismo */
	HAL_ADC_SYNC_SEL_BYPASS_SYNC /**< Bypass el sincronismo */
}hal_adc_sync_sel_en;

/** Selección de modo de interrupción del *ADC* */
typedef enum
{
	HAL_ADC_INTERRUPT_MODE_EOC = 0, /**< Modo de interrupción en fin de conversión */
	HAL_ADC_INTERRUPT_MODE_EOS /**< Modo de interrupción en fin de secuencia */
}hal_adc_interrupt_mode_en;

/** Canal que genero el resultado de *ADC* */
typedef enum
{
	HAL_ADC_RESULT_CHANNEL_0 = 0, /**< Canal 0 */
	HAL_ADC_RESULT_CHANNEL_1, /**< Canal 1 */
	HAL_ADC_RESULT_CHANNEL_2, /**< Canal 2 */
	HAL_ADC_RESULT_CHANNEL_3, /**< Canal 3 */
	HAL_ADC_RESULT_CHANNEL_4, /**< Canal 4 */
	HAL_ADC_RESULT_CHANNEL_5, /**< Canal 5 */
	HAL_ADC_RESULT_CHANNEL_6, /**< Canal 6 */
	HAL_ADC_RESULT_CHANNEL_7, /**< Canal 7 */
	HAL_ADC_RESULT_CHANNEL_8, /**< Canal 8 */
	HAL_ADC_RESULT_CHANNEL_9, /**< Canal 9 */
	HAL_ADC_RESULT_CHANNEL_10, /**< Canal 10 */
	HAL_ADC_RESULT_CHANNEL_11, /**< Canal 11 */
	HAL_ADC_RESULT_CHANNEL_GLOBAL /**< Global */
}hal_adc_result_channel_en;

/** Resultado de obtención de resultado de secuencia */
typedef enum
{
	HAL_ADC_SEQUENCE_RESULT_VALID = 0, /**< Resultado válido */
	HAL_ADC_SEQUENCE_RESULT_INVALID /**< Resultado inválido */
}hal_adc_sequence_result_en;

/** Selección del umbral del ADC.*/
typedef enum
{
	HAL_ADC_THRESHOLD_SEL_0 = 0, /**< Banco 0 de threshold */
	HAL_ADC_THRESHOLD_SEL_1 /**< Banco 1 de threshold */
}hal_adc_threshold_sel_en;

/** Posibles configuraciones de la interrupción por comparación de las muestras obtenidas de un
 * canal con el umbral establecido. */
typedef enum
{
	HAL_ADC_THRESHOLD_IRQ_SEL_DISABLED = 0, /**< Interrupción por umbral inhabilitada */
	HAL_ADC_THRESHOLD_IRQ_SEL_OUTSIDE, /**< Interrupción por conversión fuera del umbral establecido */
	HAL_ADC_THRESHOLD_IRQ_SEL_CROSSING /**< Interrupción por conversión cruzando alguno de los umbrales establecidos */
}hal_adc_threshold_interrupt_sel_en;

/** Resultado de comparación de conversión contra los umbrales */
typedef enum
{
	HAL_ADC_COMPARISON_RANGE_INSIDE = 0, /**< Resultado de conversión dentro del umbral estipulado */
	HAL_ADC_COMPARISON_RANGE_BELOW, /**< Resultado de conversión por debajo del umbral estipulado */
	HAL_ADC_COMPARISON_RANGE_ABOVE /**< Resultado de conversión por encima del umbral estipulado */
}hal_adc_compare_range_result_en;

/** Resultado de comparación de conversión (cruce) */
typedef enum
{
	HAL_ADC_COMPARISON_NO_CROSSING = 0, /**< Resultado de conversión no estaba cruzando ningún umbral */
	HAL_ADC_COMPARISON_CROSSING_DOWNWARD = 2, /**< El resultado de la conversión actual cruzó algún umbral hacia abajo */
	HAL_ADC_COMPARISON_CROSSING_UPWARD /**< El resultado de la conversión actual cruzó algún umbral hacia arriba */
}hal_adc_compare_crossing_result_en;

/**
 * @brief Tipo de dato para callback de interrupcion de sequencia
 *
 * @note Estos callbacks son ejecutados desde un contexto de interrupción, por lo que el usuario deberá tener
 * todas las consideraciones necesarias al respecto.
 */
typedef void (*adc_sequence_interrupt_t)(void*);

/**
 * @brief Tipo de dato para callback de interrupcion de comparación
 *
 * @note Estos callbacks son ejecutados desde un contexto de interrupción, por lo que el usuario deberá tener
 * todas las consideraciones necesarias al respecto.
 */
typedef void (*adc_comparison_interrupt_t)(void*);

/** Configuración de secuencia de *ADC* */
typedef struct
{
	uint16_t channels; /**< Canales habilitados. Cada uno de los bits representa el canal */
	hal_adc_trigger_sel_en trigger; /**< Configuración de fuente de trigger para la secuencia */
	hal_adc_trigger_pol_sel_en trigger_pol; /**< Configuración de flanco del trigger para la secuencia */
	hal_adc_sync_sel_en sync_bypass; /**< Configuración de sincronismo de la secuencia */
	hal_adc_interrupt_mode_en mode;/**< Configuración de modo de interrupcion */
	uint8_t burst; /**< Configuración de modo BURST. En caso de ser 0 esta inhabilitado, cualquier otro
						valor lo habilita */
	uint8_t single_step; /**< Configuración de funcionamiento del trigger. En caso de ser 0, un trigger
							  dispara la conversión de toda la secuencia configurada, en caso de ser
							  cualquier otro valor, un trigger dispara la conversión del siguiente canal
							  habilitado en la secuencia */
	uint8_t low_priority; /**< Fijar baja prioridad de la secuencia. Unicamente aplica para la secuencia
	 	 	 	 	 	 	   *A*. En caso de ser 0, la secuencia *A* tiene prioridad por sobre el *B*,
	 	 	 	 	 	 	   cualquier otro valor, implica que la secuencia B tiene prioridad por sobre
	 	 	 	 	 	 	   la *A* */
	adc_sequence_interrupt_t callback; /**< Callback a ejecutar en interrupción de secuencia. La misma se generará
											al final de la conversión de cada canal, o de toda la secuencia,
											dependiendo de la configuración global del *ADC* */
	void *cb_data; /**< Datos del usuario para pasar al callback de interrupcion de secuencia */
}hal_adc_sequence_config_t;

/** Dato que representa el resultado de una conversión (sea de secuencia completa o de canal) */
typedef struct
{
	hal_adc_result_channel_en channel; /**< Canal que generó el resultado */
	uint16_t result; /**< Valor de la conversión */
}hal_adc_sequence_result_t;

/** Resultado de comparaciones */
typedef struct
{
	hal_adc_result_channel_en channel; /**< Canal que detectó comparación configurada */
	uint16_t value; /**< Valor de la conversión */
	hal_adc_compare_range_result_en result_range; /**< Rango de la comparación */
	hal_adc_compare_crossing_result_en result_crossing; /**< Resultado si está cruzando */
}hal_adc_channel_compare_result_t;

/**
 * @brief Inicializar el *ADC* en modo **asincrónico**
 *
 * Realiza la calibración de hardware y fija la frecuencia de muestreo deseada.
 *
 * @note Solamente se debe realizar el llamado a una de las dos funciones de inicialización del *ADC*.
 *
 * @see hal_adc_clock_source_en
 * @see hal_adc_low_power_mode_en
 * @param[in] sample_freq Frecuencia de sampleo deseada
 * @param[in] div Divisor para la lógica del *ADC*
 * @param[in] clock_source Fuente de clock para el *ADC*
 * @param[in] low_power Selección de modo de bajo consumo
 * @pre Configuración del clock a utilizar
 */
void hal_adc_init_async_mode(uint32_t sample_freq, uint8_t div, hal_adc_clock_source_en clock_source, hal_adc_low_power_mode_en low_power);

/**
 * @brief Inicializar el *ADC* en modo **sincrónico**
 *
 * Realiza la calibración de hardware y fija la frecuencia de muestreo deseada.
 *
 * @note Solamente se debe realizar el llamado a una de las dos funciones de inicialización del *ADC*.
 *
 * @see hal_adc_clock_source_en
 * @see hal_adc_low_power_mode_en
 * @param[in] sample_freq Frecuencia de sampleo deseada
 * @param[in] low_power Selección de modo de bajo consumo
 * @pre Configuración del clock a utilizar
 */
void hal_adc_init_sync_mode(uint32_t sample_freq, hal_adc_low_power_mode_en low_power);

/**
 * @brief De-inicialización del *ADC*
 *
 * Además, desliga todos los pines externos posiblemente utilizados por el ADC de su función analógica.
 */
void hal_adc_deinit(void);

/**
 * @brief Inhibir las interrupciones de secuencia de ADC
 * @param[in] sequence Sequenca a inhibir
 */
void hal_adc_inhibit_sequence_interrupts(hal_adc_sequence_sel_en sequence);

/**
 * @brief Desinhibir las interrupciones de secuencia de ADC
 * @param[in] sequence Sequenca a desinhibir
 */
void hal_adc_deinhibit_sequence_interrupts(hal_adc_sequence_sel_en sequence);

/**
 * @brief Configurar una secuencia de conversión
 *
 * Una vez configurado un canal del ADC con este método, el pin externo correspondiente a él quedará ligado
 * a su función analógica hasta de-inicializar el periférico por medio de la función hal_adc_deinit()
 *
 * @note Esta función no habilita las secuencias.
 *
 * @param[in] sequence Seleccion de secuencia a configurar
 * @param[in] config Configuracion deseada para la secuencia
 * @see hal_adc_sequence_sel_en
 * @see hal_adc_sequence_config_t
 */
void hal_adc_sequence_config(hal_adc_sequence_sel_en sequence, const hal_adc_sequence_config_t *config);


/**
 * @brief Disparar conversiones en una secuencia
 *
 * @pre Si se están utilizando triggers por hardware, esta función simplemente habilitará la secuencia,
 * sin disparar una conversión. Sin embargo, para evitar un trigger espúrio, es necesario asegurar que
 * la señal de trigger se encuentre inactiva según cómo se lo haya definido en el parámetro
 * hal_adc_sequence_config_t::trigger_pol
 *
 * Si la secuencia está configurada en modo hal_adc_sequence_config_t::burst
 * esta función comenzará conversiones consecutivas en todos los canales configurados.
 *
 * Si la secuencia **no utiliza** el modo hal_adc_sequence_config_t::burst, entonces esta función disparará una sola conversión en un canal
 * o una conversión en todos los canales configurados de la secuencia, dependiendo del parámetro hal_adc_sequence_config_t::single_step.
 *
 * @note En todos los casos, esta función habilita la secuencia antes de disparar la conversión (si corresponde).
 *
 * @param[in] sequence Secuencia a disparar
 * @see hal_adc_sequence_sel_en
 * @see hal_adc_sequence_start
 * @see hal_adc_sequence_config_t
 */
void hal_adc_sequence_start(hal_adc_sequence_sel_en sequence);

/**
 * @brief Detener conversiones en una secuencia de conversión.
 *
 * Si hay una conversión en curso, al finalizar ésta ya no se realizará otra.
 *
 * @note Esta función, además, deshabilita la secuencia. Esto es necesario para cambiar las
 * configuraciones de secuencias ya en uso de forma segura.
 *
 * @param[in] sequence Secuencia a detener
 * @see hal_adc_sequence_sel_en
 * @see hal_adc_start_sequence
 */
void hal_adc_sequence_stop(hal_adc_sequence_sel_en sequence);

/**
 * @brief Obtener resultado de la secuencia
 *
 * El comportamiento de esta funcion depende de la configuración de la secuencia, en particular
 * de la configuracion hal_adc_sequence_config_t::mode. En caso de estar configurada para interrumpir al final de cada
 * conversión, la función únicamente guardara el resultado de la conversión en el primer lugar
 * del parámetro hal_adc_sequence_result_t::result, caso contrario, guardara la cantidad de canales habilitados en la
 * conversión en los distintos lugares del parámetro *result*.
 *
 * @see hal_adc_sequence_result_en
 * @see hal_adc_sequence_sel_en
 * @see hal_adc_sequence_result_t
 * @param[in] sequence Secuencia de la cual obtener el resultado
 * @param[out] result Lugares donde guardar los resultados de la secuencia
 *
 * El usuario debe garantizar que existe lugar para la misma cantidad de canales habilitados en la secuencia.
 *
 * @return Resultado de la función
 */
hal_adc_sequence_result_en hal_adc_sequence_get_result(hal_adc_sequence_sel_en sequence, hal_adc_sequence_result_t *result);

/**
 * @brief Configurar valor de umbral de comparación.
 * @param[in] threshold	Selección de umbral a configurar
 * @param[in] low Umbral bajo
 * @param[in] high Umbral alto
 * @see hal_adc_threshold_sel_en
 */
void hal_adc_threshold_config(hal_adc_threshold_sel_en threshold, uint16_t low, uint16_t high);

/**
 * @brief Configura un canal para utilizar la funcionalidad de comparación con un umbral y su tipo de interrupción deseada.
 *
 * Puede utilizarse como método de deshabilitación de las interrupciones, debidas a comparación, de canales específicos.
 * Para deshabilitar las interrupciones, debidas a comparación, de todos los canales a la vez puede utilizarse la función
 * hal_adc_threshold_register_interrupt()
 *
 * @param[in] adc_channel Canal de ADC en el cual configurar el umbral
 * @param[in] threshold	Selección de umbral a configurar
 * @param[in] irq_mode Indica el tipo evento por el cual la comparación con el umbral dispara la interrupción, o la desactiva.
 * @see hal_adc_threshold_sel_en
 * @see hal_adc_threshold_interrupt_sel_en
 */
void hal_adc_threshold_channel_config(uint8_t adc_channel, hal_adc_threshold_sel_en threshold, hal_adc_threshold_interrupt_sel_en irq_mode);

/**
 * @brief Registrar un callabck de interrupción para interrupción por threshold.
 *
 * Si se le pasa NULL, esta función efectivamente deshabilita las interrupciones, debidas a comparaciones, de todos los canales del ADC.
 * Sin embargo, esto no altera en modo alguno la configuración ya establecida por el usuario para las comparaciones de cada canal.
 *
 * En caso de querer volver a habilitar las interrupciones debidas a comparación, de todos los canales del ADC
 * configurados para ello de antemano, con simplemente llamar a esta función con un puntero a función válido (no NULL) para la interrupción
 * es suficiente.
 *
 * @param[in] callback Callback a ejecutar en interrupción por threshold
 * @param[in] data Datos para pasarle al callback a ejecutar
 * @see hal_adc_threshold_channel_config
 */
void hal_adc_threshold_register_interrupt(adc_comparison_interrupt_t callback, void *data);

/**
 * @brief Obtener resultados de comparación de la última conversión
 * @param[out] results Puntero a donde guardar los resultados
 *
 * El usuario **debe** garantizar que hayan por lo menos la cantidad de memoria reservada de este tipo
 * como cantidad de canales habilitados para comparar contra un umbral.
 *
 * @see hal_adc_channel_compare_result_t
 */
void hal_adc_threshold_get_comparison_results(hal_adc_channel_compare_result_t *results);

#if defined (__cplusplus)
} // extern "C"
#endif

#endif /* HAL_ADC_H_ */

/**
 * @}
 */
