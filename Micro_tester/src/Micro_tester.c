#include <cr_section_macros.h>
#include <stdio.h>
#include <HPL_GPIO.h>
#include <HPL_ADC.h>
#include <HPL_SYSCON.h>
#include <HPL_SWM.h>
#include <HRI_SWM.h>
#include <HPL_SYSTICK.h>
#include <HPL_UART.h>

#define		LED_PORT			0
#define		LED_PIN				1

#define		TICK_PERIOD_US		1000
#define		CONVERSION_TIME_MS	50

#define		ADC_CHANNEL			0
#define		ADC_FREQUENCY		500e3

#define		RX_PORT				0
#define		RX_PIN				17

#define		TX_PORT				0
#define		TX_PIN				16

#define		UART_NUMBER			0
#define		UART_BAUDRATE		115200

#define		CLOCKOUT_DIVIDER	10
#define		CLOCKOUT_PORT		0
#define		CLOCKOUT_PIN		18

static void tick_callback(void);
static void adc_callback(void);
static void rx_callback(void);

static uint32_t blink_time_ms = 0;
static uint32_t adc_conversion = 0;

int main(void)
{
	const ADC_conversions_config_t adc_config =
	{
		.channels = (1 << ADC_CHANNEL),
		.burst = 0,
		.conversion_ended_callback = adc_callback
	};

	const UART_config_t uart_config =
	{
		.data_length = UART_DATALEN_8BIT,
		.parity = UART_PARITY_NO_PARITY,
		.stop_bits = UART_STOPLEN_1BIT,
		.oversampling = UART_OVERSAMPLING_X16,
		.clock_selection = PERIPHERAL_CLOCK_SELECTION_FRG0,
		.baudrate = UART_BAUDRATE,
		.tx_port = TX_PORT,
		.tx_pin = TX_PIN,
		.rx_port = RX_PORT,
		.rx_pin = RX_PIN,
		.rx_ready_callback = rx_callback,
		.tx_ready_callback = NULL
	};

	SYSCON_set_crystal_clock(12e6);
	SYSCON_set_ext_clock_source(EXT_CLOCK_SOURCE_SELECTION_CRYSTAL);
	SYSCON_set_PLL(PLL_SOURCE_EXT_OSC, 2);
	SYSCON_set_system_clock_source(SYSTEM_CLOCK_SEL_PLL);

	// Clock principal en un pin (utilizando un divisor)
	SYSCON_set_clkout_config(CLKOUT_SOURCE_SELECTION_MAIN_CLOCK, CLOCKOUT_DIVIDER, CLOCKOUT_PORT, CLOCKOUT_PIN);

	// Hasta aca queda el clock configurado con un cristal externo de 12MHz, elevado a 24MHz
	// Configuro el fraccional para poder tener buena presicion para un baudrate de 115200bps
	// El DIV siempre debe estar en 256 (especificacion del manual de usuario)
	// Como fuente utilizo el PLL a 24MHz ya configurado
	// 24MHz / (1 + (47 / 256)) = 20.27722772MHz
	SYSCON_set_frg_config(0, FRG_CLOCK_SELECTION_MAIN_CLOCK, 47, 256);

	GPIO_init(LED_PORT);

	GPIO_set_dir(LED_PORT, LED_PIN, GPIO_DIR_OUTPUT, 0);

	ADC_init(ADC_FREQUENCY, ADC_CLOCK_SOURCE_PLL);
	ADC_config_conversions(&adc_config);

	UART_init(UART_NUMBER, &uart_config);

	SYSTICK_init(TICK_PERIOD_US, tick_callback);

	UART_tx_byte(UART_NUMBER, 'U');

	while(1)
	{

	}

	return 0;
}

static void tick_callback(void)
{
	static uint32_t tick_counter = 0;
	static uint32_t adc_counter = 0;

	tick_counter = (tick_counter + 1) % blink_time_ms;
	adc_counter = (adc_counter + 1) % CONVERSION_TIME_MS;

	if(tick_counter == 0)
	{
		GPIO_toggle_pin(LED_PORT, LED_PIN);
	}

	if(adc_counter == 0)
	{
		ADC_start_conversions();
	}
}

static void adc_callback(void)
{
	ADC_get_conversion(ADC_CHANNEL, &adc_conversion);

	blink_time_ms = adc_conversion / 4;
}

static void rx_callback(void)
{
	uint32_t data;

	UART_rx_byte(UART_NUMBER, &data);

	if((char) data == ' ')
	{
		UART_tx_byte(UART_NUMBER, 'a');
	}
}
