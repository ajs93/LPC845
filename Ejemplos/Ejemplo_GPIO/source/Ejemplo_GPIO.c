/**
 * @file Ejemplo_GPIO.c
 * @brief Ejemplo de utilización del \e GPIO con la librería (capa de aplicación)
 *
 * # Configuraciones
 *
 * El programa utiliza únicamente el LED RGB y el pulsador de usuario del stick de desarrollo.
 * - LED RGB - Rojo: Puerto 1, Pin 2
 * - LED RGB - Azul: Puerto 1, Pin 1
 * - Pulsador de usuario: Puerto 0, Pin 4
 *
 * Ver @ref acerca_del_stick para más información.
 *
 * Se configura el \e Systick para generar ticks cada 1 milisegundo.
 *
 * Se configura el pin del pulsador para utilizar lógica invertida en su hardware.
 *
 * # Funcionamiento del programa
 *
 * El \e Systick se utiliza para generar un titileo en el LED RGB \e rojo siempre y cuando el pulsador de usuario
 * no esté presionado. Mientras el pulsador de usuario se encuentre presionado, el LED RGB \e azul estará fijo y
 * el \e rojo quedará apagado. El titileo nunca deja de suceder, sino que se enmascara la salida correspondiente.
 * Esto implica que si el pulsador se deja de presionar en el momento en que el mismo tiene el LED RGB \e rojo
 * encendido, no se va a ver el LED RGB \e azul encender, dado el tipo de conexión en el stick de desarrollo
 * y los valores de tensión de LED.
 *
 * @author Augusto Santini
 * @date 4/2020
 */

#include <cr_section_macros.h>
#include <HAL_SYSTICK.h>
#include <HAL_GPIO.h>
#include <HAL_IOCON.h>

/** Tiempo de interrupción del \e Systick en \b microsegundos */
#define		TICK_TIME_USEG				(1000)

/** Puerto/pin del LED RGB \e rojo */
#define		LED_RGB_RED_PORTPIN			(HAL_GPIO_PORTPIN_1_2)

/** Puerto/pin del LED RGB \e azul */
#define		LED_RGB_BLUE_PORTPIN		(HAL_GPIO_PORTPIN_1_1)

/** Puerto/pin del switch */
#define		KEY_PORTPINT				(HAL_GPIO_PORTPIN_0_4)

/** Tiempo de titileo en \e milisegundos */
#define		BLINK_TIME_MSEG				(1000)

/** Valor que apaga el LED RGB, sea el azul o el rojo */
#define		LED_RGB_OFF					(1)

/** Valor que enciende el LED RGB, sea el azul o el rojo */
#define		LED_RGB_ON					(0)

/** Macro para apagar el LED RGB \e azul */
#define		LED_RGB_BLUE_OFF			(hal_gpio_set_pin(LED_RGB_BLUE_PORTPIN))

/** Macro para encender el LED RGB \e azul */
#define		LED_RGB_BLUE_ON				(hal_gpio_clear_pin(LED_RGB_BLUE_PORTPIN))

static void tick_callback(void);

/** Configuración de hardware del pin del pulsador de usuario */
static const hal_iocon_config_t key_config =
{
	.pull_mode = HAL_IOCON_PULL_NONE,
	.hysteresis = 0,
	.invert_input = 1,
	.open_drain = 0,
	.sample_mode = HAL_IOCON_SAMPLE_MODE_BYPASS,
	.clk_sel = HAL_IOCON_CLK_DIV_0,
	.iic_mode = HAL_IOCON_IIC_MODE_GPIO
};

/**
 * @brief Punto de entrada del programa
 * @return Nunca deberia terminar esta función
 */
int main(void)
{
	// Inicialización de ambos puertos de GPIO
	// En este caso se necesitan los dos, dado que el switch esta en el puerto 0 y los LED RGB en el puerto 1
	hal_gpio_init(HAL_GPIO_PORT_0);
	hal_gpio_init(HAL_GPIO_PORT_1);

	// Inicialización de los pines de los LEDs RGB como salida y con los LEDs apagados
	hal_gpio_set_dir(LED_RGB_RED_PORTPIN, HAL_GPIO_DIR_OUTPUT, LED_RGB_OFF);
	hal_gpio_set_dir(LED_RGB_BLUE_PORTPIN, HAL_GPIO_DIR_OUTPUT, LED_RGB_OFF);

	// Configuración del pin del switch para utilizar lógica invertida
	hal_iocon_config_io(KEY_PORTPINT, &key_config);

	// Inicialización del pin del switch como entrada
	hal_gpio_set_dir(KEY_PORTPINT, HAL_GPIO_DIR_INPUT, 0);

	// Inicialización del \e Systick con el tiempo de tick adecuado
	hal_systick_init(TICK_TIME_USEG, tick_callback);

	while(1)
	{
		/*
		 * Si bien el pulsador lleva el nivel físico del pin a VSS al ser presionado, se configuró el mismo para
		 * utilizar lógica invertida, es decir que en este caso, al leer 0 se está leyendo que el pulsador NO
		 * está siendo presionado.
		 */
		if(hal_gpio_read_pin(KEY_PORTPINT) == 0)
		{
			// El switch no está presionado
			hal_gpio_set_mask_bits(HAL_GPIO_PORTPIN_TO_PORT(LED_RGB_RED_PORTPIN), 1 << HAL_GPIO_PORTPIN_TO_PIN(LED_RGB_RED_PORTPIN));
			LED_RGB_BLUE_ON;
		}
		else
		{
			// El switch está presionado
			hal_gpio_clear_mask_bits(HAL_GPIO_PORTPIN_TO_PORT(LED_RGB_RED_PORTPIN), 1 << HAL_GPIO_PORTPIN_TO_PIN(LED_RGB_RED_PORTPIN));
			LED_RGB_BLUE_OFF;
		}
		/*
		 * NOTA: El LED RGB \e verde y la máscara necesaria están siendo escritas constantemente. Esto no es ideal,
		 * lo ideal sería identificar en qué momento el switch cambia de estado y escribir una única vez, en el
		 * momento que sea necesario.
		 */
	}

	return 0;
}

/**
 * @brief Callback a ejecutar en cada tick
 */
static void tick_callback(void)
{
	static uint32_t contador = 0;

	contador = (contador + 1) % BLINK_TIME_MSEG;

	if(!contador)
	{
		// Titileo cada 1 segundo
		hal_gpio_masked_toggle_port(HAL_GPIO_PORTPIN_TO_PORT(LED_RGB_RED_PORTPIN), 1 << HAL_GPIO_PORTPIN_TO_PIN(LED_RGB_RED_PORTPIN));
	}
}
