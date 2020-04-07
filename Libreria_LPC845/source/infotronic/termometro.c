/**
 * @file termometro.c
 * @brief Funciones para el manejo del termometro
 * @author Augusto Santini
 * @date 12/2019
 * @version 1.0
 */

#include <stdint.h>
#include <termometro.h>
#include <HAL_ADC.h>

#define		ADC_SAMPLING_FREQUENCY			100000
#define		ADC_CHANNEL						1
#define		ADC_CHANNEL_MASK				(1 << ADC_CHANNEL)

#define		CONVERSION_PERIOD_MSEC			25

#define		CONVERSION_BUFFER_LENGTH		10

#define		ADC_TERMOMETRO_SEQUENCE			HAL_ADC_SEQUENCE_SEL_A

/** Estructura para definir relaciones temperatura/cuentas */
typedef struct
{
	int32_t temperature; /**< Temperatura en mili °C */
	int32_t counts; /**< Cuentas correspondientes */
}temperature_counts_t;

static const temperature_counts_t conversion_table[] = /**< Tabla de conversiones de temperatura a cuentas */
{
	{.temperature = -70000, .counts = 4085},
	{.temperature = -65000, .counts = 4080},
	{.temperature = -60000, .counts = 4072},
	{.temperature = -55000, .counts = 4061},
	{.temperature = -50000, .counts = 4045},
	{.temperature = -45000, .counts = 4022},
	{.temperature = -40000, .counts = 3990},
	{.temperature = -35000, .counts = 3948},
	{.temperature = -30000, .counts = 3893},
	{.temperature = -25000, .counts = 3821},
	{.temperature = -20000, .counts = 3730},
	{.temperature = -15000, .counts = 3618},
	{.temperature = -10000, .counts = 3483},
	{.temperature = -5000, .counts = 3325},
	{.temperature = 0, .counts = 3145},
	{.temperature = 5000, .counts = 2945},
	{.temperature = 10000, .counts = 2729},
	{.temperature = 15000, .counts = 2504},
	{.temperature = 20000, .counts = 2274},
	{.temperature = 25000, .counts = 2047},
	{.temperature = 30000, .counts = 1827},
	{.temperature = 35000, .counts = 1619},
	{.temperature = 40000, .counts = 1426},
	{.temperature = 45000, .counts = 1250},
	{.temperature = 50000, .counts = 1091},
	{.temperature = 55000, .counts = 950},
	{.temperature = 60000, .counts = 826},
	{.temperature = 65000, .counts = 718},
	{.temperature = 70000, .counts = 624},
	{.temperature = 75000, .counts = 542},
	{.temperature = 80000, .counts = 472},
	{.temperature = 85000, .counts = 411},
	{.temperature = 90000, .counts = 359},
	{.temperature = 95000, .counts = 314},
	{.temperature = 100000, .counts = 275},
	{.temperature = 105000, .counts = 242},
	{.temperature = 110000, .counts = 213},
	{.temperature = 115000, .counts = 188},
	{.temperature = 120000, .counts = 166},
	{.temperature = 125000, .counts = 147},
	{.temperature = 130000, .counts = 131},
	{.temperature = 135000, .counts = 117},
	{.temperature = 140000, .counts = 104},
	{.temperature = 145000, .counts = 93},
	{.temperature = 150000, .counts = 84}
};

static uint32_t conversion_buffer[CONVERSION_BUFFER_LENGTH];
static uint32_t conversion_buffer_idx = 0;
static int32_t current_temperature = 0;

static void adc_conversion_finished(void);

/**
 * @brief Inicializacion del termometro
 */
void termometro_init(void)
{
	hal_adc_sequence_config_t adc_config;

	adc_config.burst = 0;
	adc_config.channels = ADC_CHANNEL_MASK;
	adc_config.callback = adc_conversion_finished;
	adc_config.single_step = 0;
	adc_config.trigger = HAL_ADC_TRIGGER_SEL_NONE;

	hal_adc_init_async_mode(ADC_SAMPLING_FREQUENCY, 0, HAL_ADC_CLOCK_SOURCE_FRO, HAL_ADC_LOW_POWER_MODE_DISABLED);

	hal_adc_config_sequence(ADC_TERMOMETRO_SEQUENCE, &adc_config);
}

/**
 * @brief Callback para revisar el termometro periodicamente
 */
void termometro_check(void)
{
	static uint32_t msec_counter = 0;

	msec_counter = (msec_counter + 1) % CONVERSION_PERIOD_MSEC;

	if(msec_counter == 0)
	{
		hal_adc_start_sequence(ADC_TERMOMETRO_SEQUENCE);
	}
}

/**
 * @brief Tomar la lectura actual del termometro
 * @return Lectura actual del termometro en mili °C
 */
uint32_t termometro_read(void)
{
	return current_temperature;
}

/**
 * @brief Callback a ejecutar una vez terminada una conversion de ADC
 */
static void adc_conversion_finished(void)
{
	uint32_t adc_value;
	hal_adc_sequence_result_t adc_result;
	hal_adc_sequence_result_t *adc_result_p = &adc_result;

	if(hal_adc_get_sequence_result(ADC_TERMOMETRO_SEQUENCE, &adc_result_p) == HAL_ADC_SEQUENCE_RESULT_VALID)
	{
		adc_value = adc_result.result;
		conversion_buffer[conversion_buffer_idx] = adc_value;

		conversion_buffer_idx = (conversion_buffer_idx + 1) % CONVERSION_BUFFER_LENGTH;

		if(conversion_buffer_idx == 0)
		{
			uint8_t counter;
			uint32_t current_conversion = 0;

			// Promedio de las mediciones
			for(counter = 0; counter < CONVERSION_BUFFER_LENGTH; counter++)
			{
				current_conversion += conversion_buffer[counter];
			}

			current_conversion /= CONVERSION_BUFFER_LENGTH;

			// Conversion de muestras a temperaturas
			for(counter = 0; counter <= (sizeof(conversion_table) / sizeof(conversion_table[0])); counter++)
			{
				if(conversion_table[counter].counts <= current_conversion)
				{
					break;
				}
			}

			if(counter == 0)
			{
				if(conversion_table[counter].counts == current_conversion)
				{
					current_temperature = conversion_table[counter].temperature;
				}
				else
				{
					current_temperature = (conversion_table[counter].temperature - conversion_table[counter - 1].temperature);
					current_temperature *= (current_conversion - 4095);
					current_temperature /= (conversion_table[counter].counts - conversion_table[counter - 1].counts);
					current_temperature += conversion_table[counter].temperature;
				}
			}
			else
			{
				if(conversion_table[counter].counts == current_conversion)
				{
					current_temperature = conversion_table[counter].temperature;
				}
				else
				{
					current_temperature = (conversion_table[counter].temperature - conversion_table[counter - 1].temperature);
					current_temperature *= (current_conversion - conversion_table[counter].counts);
					current_temperature /= (conversion_table[counter - 1].counts - conversion_table[counter].counts);
					current_temperature = conversion_table[counter].temperature - current_temperature;
				}
			}
		}
	}
}
