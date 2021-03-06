/**
 * @file Ejemplo_ADC.c
 * @brief Ejemplo de utilización del \e ADC con la librería (capa de aplicación)
 *
 * # Configuraciones
 *
 * El programa utiliza el clock por default con el que comienza el microcontrolador, es decir, el <em>Free Running
 * Oscillator</em> funcionando a 12MHz
 *
 * El periférico \e ADC será configurado con las siguientes características:
 * 	- Funcionamiento \b sincrónico
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
 * Una vez inicializado el periférico, se configura el periférico \e Systick para interrumpir cada 1 milisegundo
 * y mediante su manejador se lleva la cuenta de los milisegundos transcurridos. Una vez transcurridos
 * 1 segundo, se dispara una conversión de \e ADC, y sus resultados se guardan en dos variables globales.
 *
 * # Descripción de funcionamiento
 *
 * La idea de este ejemplo es demostrar las capacidades de la librería para con el periférico \e ADC. Con dicho
 * fin, se muestran las dos principales capacidades del periférico, incluidas particularidades explicadas a
 * continuación.
 *
 * ## Conversiones no continuas
 *
 * La <em>secuencia B</em> está configurada para generar conversiones disparadas por software. Esta forma es
 * usualmente utilizada cuando no es necesario tener conversiones continuas, dada la naturaleza de necesidad de
 * la aplicación a desarrollar. Sin embargo, no es la óptima para distintos casos como pueden ser, por ejemplo:
 * 		- Conversiones con tiempos precisos. En estos casos, se recomienda disparar conversiones disparadas
 * 		por algún timer, la librería tiene implementada esta configuración.
 * 		- Conversiones que dependen de variables externas. En estos casos, es útil disparar las conversiones
 * 		mediante interrupciones de \e PININT o \e ACOMP.
 * 		.
 *
 * En el caso del ejemplo, se disparan conversiones cada aproximadamente 100 milisegundos con ayuda del \e Systick.
 *
 * ## Conversiones continuas
 *
 * La <em>secuencia A</em>, al tener el modo \e BURST habilitado, genera conversiones continuamente. Dependiendo
 * de la necesidad de procesamiento de dichos resultados, tal vez es deseable que el programa no sea interrumpido
 * constantemente, dado que tal vez se necesiten condiciones muy simples de análisis, como puede ser un umbral. En
 * este ejemplo, se demuestra la potencia del periférico \e ADC al utilizar el <em>threshold compare</em>. El
 * mismo es configurado para generar interrupciones cuando el valor convertido en el <em>Canal 0</em> cruce por
 * ciertos umbrales estipulados por el usuario. Para el ejemplo, dependiendo de si la conversión que generó la
 * interrupción estaba entre los valores de umbral, o por fuera, se enciende alguno de los LEDs RGB (rojo o azul)
 * del stick de desarrollo, sin una interrupción constante por parte de la finalización de conversión de la
 * <em>secuencia A</em>.
 *
 * ## Posibilidad de interrupcion de secuencia de conversión A
 *
 * La <em>secuencia A</em> está configurada como <em>baja prioridad</em>. Esto implica que cualquier disparo de
 * conversión de <em>secuencia B</em> frenará las conversiones de la <em>secuencia A</em>, realizará la/s
 * conversiones de la <em>secuencia B</em> y luego retomará las conversiones de <em>secuencia A</em>. Como la
 * <em>secuencia A</em> está configurada para convertir continuamente, queda en real evidencia la condición de
 * baja prioridad de la <em>secuencia A</em>.
 *
 * @author Augusto Santini
 * @author Esteban E. Chiama
 * @date 4/2020
 */

#include <cr_section_macros.h>
#include <stddef.h>
#include <HAL_GPIO.h>
#include <HAL_ADC.h>
#include <HAL_SYSTICK.h>

/** Máscara de configuración de canales habilitados para la secuencia A */
#define		ADC_SEQA_CHANNELS			(1 << 0)

/** Canal donde se encuentra el preset del stick de desarrollo */
#define		ADC_PRESET_CHANNEL			(0)

/** Máscara de configuración de canales habilitados para la secuencia B */
#define		ADC_SEQB_CHANNELS			((1 << 4) | (1 << 8))

/** Tiempo de interrupción del \e Systick en \b microsegundos */
#define		TICK_TIME_USEG				(1000)

/** Frecuencia de muestreo a utilizar por el ADC */
#define		ADC_SAMPLE_FREQ				(1000000)

/** Tiempo de disparo de conversiones de \e ADC en \b milisegundos */
#define		ADC_CONVERSION_TIME_MSEG	(100)

/** Valor de umbral bajo para las comparaciones del ADC */
#define		THR_LOW						(1000)

/** Valor de umbral alto para las comparaciones del ADC */
#define 	THR_HIGH					(2500)

/** Puerto de ambos LEDS */
#define 	LED_PORT					(1)

/** Puerto y pin de Led Azul */
#define 	LED_AZUL					(HAL_GPIO_PORTPIN_1_1)

/** Puerto y pin de Led Rojo */
#define 	LED_ROJO					(HAL_GPIO_PORTPIN_1_2)

/** Valor lógico de LED encendido */
#define 	LED_ON_STATE				(0)

/** Valor lógico de LED apagado */
#define 	LED_OFF_STATE				(1)

static void adc_sequence_callback(void);

static void adc_thr_callback(void);

static void systick_callback(void);

/** Flag para indicar finalización de secuencia de conversión de \e ADC */
static uint8_t flag_secuencia_adc_completada = 0;

/** Variables para guardar los resultados de la secuencia de conversión */
static hal_adc_sequence_result_t resultados_conversion_adc[2];

/** Variable para guardar el resultado de una comparación válida contra el umbral */
static hal_adc_channel_compare_result_t comparison_result =
{
	.value = 0,
	.result_range = 0,
	.result_crossing = 0
};

/** Configuración de la secuencia A. Como no va a cambiar es declarada \e const */
static const hal_adc_sequence_config_t sequence_a_config =
{
	.channels = ADC_SEQA_CHANNELS,
	.trigger = HAL_ADC_TRIGGER_SEL_NONE,
	.trigger_pol = HAL_ADC_TRIGGER_POL_SEL_NEGATIVE_EDGE,
	.sync_bypass = HAL_ADC_SYNC_SEL_BYPASS_SYNC,
	.mode = HAL_ADC_INTERRUPT_MODE_EOC,
	.burst = 1,
	.single_step = 1,
	.low_priority = 1,
	.callback = NULL
};

/** Configuración de la secuencia B. Como no va a cambiar es declarada \e const */
static const hal_adc_sequence_config_t sequence_b_config =
{
	.channels = ADC_SEQB_CHANNELS,
	.trigger = HAL_ADC_TRIGGER_SEL_NONE,
	.trigger_pol = HAL_ADC_TRIGGER_POL_SEL_NEGATIVE_EDGE,
	.sync_bypass = HAL_ADC_SYNC_SEL_BYPASS_SYNC,
	.mode = HAL_ADC_INTERRUPT_MODE_EOS,
	.burst = 0,
	.single_step = 0,
	.low_priority = 0,
	.callback = adc_sequence_callback
};

/**
 * @brief Punto de entrada del programa
 * @return Nunca deberia terminar esta función
 */
int main(void)
{
	// Inicialización del periférico en modo SINCRÓNICO
	hal_adc_init_sync_mode(ADC_SAMPLE_FREQ, HAL_ADC_LOW_POWER_MODE_DISABLED);

	// Configuración de la secuencia a utilizar
	hal_adc_sequence_config(HAL_ADC_SEQUENCE_SEL_A, &sequence_a_config);
	hal_adc_sequence_config(HAL_ADC_SEQUENCE_SEL_B, &sequence_b_config);

	// Configuración de comparaciones e interrupción
	hal_adc_threshold_config(HAL_ADC_THRESHOLD_SEL_0, THR_LOW, THR_HIGH);
	hal_adc_threshold_channel_config(ADC_PRESET_CHANNEL, HAL_ADC_THRESHOLD_SEL_0, HAL_ADC_THRESHOLD_IRQ_SEL_CROSSING);
	hal_adc_threshold_register_interrupt(adc_thr_callback);

	// Inicialización del \e Systick con el tiempo de tick adecuado
	hal_systick_init(TICK_TIME_USEG, systick_callback);

	// Inicialización de GPIO correspondiente a LEDs
	hal_gpio_init(LED_PORT);
	hal_gpio_set_dir(LED_AZUL, HAL_GPIO_DIR_OUTPUT, LED_OFF_STATE);
	hal_gpio_set_dir(LED_ROJO, HAL_GPIO_DIR_OUTPUT, LED_OFF_STATE);

	// Comienzo de conversiones (modo burst) en secuencia A
	hal_adc_sequence_start(HAL_ADC_SEQUENCE_SEL_A);

    while(1)
    {
    	if(flag_secuencia_adc_completada == 1)
    	{
    		uint32_t variable_auxiliar;

    		flag_secuencia_adc_completada = 0;

    		(void) variable_auxiliar; // Esta línea es ideal para colocar el breakpoint!
    	}
    }

    return 0;
}

/**
 * @brief Callback a ejecutar en cada tick del \e Systick
 */
static void systick_callback(void)
{
	static uint32_t contador_disparo_adc = 0;

	// Conteo con valor límite
	contador_disparo_adc = (contador_disparo_adc + 1) % ADC_CONVERSION_TIME_MSEG;

	if(contador_disparo_adc == 0)
	{
		hal_adc_sequence_start(HAL_ADC_SEQUENCE_SEL_B);
	}
}

/**
 * @brief Callback a ejecutar en cada finalización de conversión de \b secuencia de \e ADC
 */
static void adc_sequence_callback(void)
{
	// Obtención de resultados de conversión
	hal_adc_sequence_get_result(HAL_ADC_SEQUENCE_SEL_B, resultados_conversion_adc);

	flag_secuencia_adc_completada = 1;
}

/**
 * @brief Callback cuando los canales habilitados cumplen con la condicion de umbral establecida
 */
static void adc_thr_callback(void)
{
	hal_adc_threshold_get_comparison_results(&comparison_result);

	// Chequeo si la interrupción se disparó porque la conversión quedó por dentro o fuera del rango
	if(comparison_result.result_range == HAL_ADC_COMPARISON_RANGE_INSIDE)
	{
		hal_gpio_set_pin(LED_ROJO);		// Apaga rojo
		hal_gpio_clear_pin(LED_AZUL);	// Prende azul
	}
	else
	{
		hal_gpio_set_pin(LED_AZUL);		// Apaga azul
		hal_gpio_clear_pin(LED_ROJO);	// Prende rojo
	}
}
