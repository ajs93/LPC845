/**
 * @file HAL_WKT.c
 * @brief Funciones a nivel de aplicacion del periferico WKT (LPC845)
 * @author Augusto Santini
 * @date 3/2020
 * @version 1.0
 */

#include <stddef.h>
#include <HAL_WKT.h>
#include <HAL_SYSCON.h>
#include <HPL_WKT.h>
#include <HPL_NVIC.h>
#include <HPL_SYSCON.h>
#include <HPL_PMU.h>

/** Valor de división que genera el *WKT* al utilizar el *FRO* como fuente de clock */
#define		HAL_WKT_DIVIDE_VALUE		(16)

/** Frecuencia del oscilador de bajo consumo */
#define		HAL_WKT_LOW_POWER_OSC_FREQ	(10e3)

/** Fuente actual configurada para el *WKT* */
static hal_wkt_clock_source_en current_clock_source = HAL_WKT_CLOCK_SOURCE_FRO_DIV;

/** Frecuencia actual externa configurada para el *WKT* */
static uint32_t current_ext_clock = 0;

static void dummy_irq(void *data);

/** Puntero al callback a ejecutar en interrupción de *WKT* */
static hal_wkt_callback_t hal_wkt_irq_callback = dummy_irq;

/** Puntero a los datos a pasarle al callback cuando se ejecute la interrupcion de WKT */
static void *hal_wkt_irq_callback_data = NULL;

void hal_wkt_init(hal_wkt_clock_source_en clock_sel, uint32_t ext_clock_value, hal_wkt_callback_t callback, void *data)
{
	SYSCON_enable_clock(SYSCON_ENABLE_CLOCK_SEL_WKT);

	SYSCON_clear_reset(SYSCON_RESET_SEL_WKT);

	hal_wkt_select_clock_source(clock_sel, ext_clock_value);
	current_ext_clock = ext_clock_value;

	hal_wkt_register_callback(callback, data);
}

void hal_wkt_select_clock_source(hal_wkt_clock_source_en clock_sel, uint32_t ext_clock_value)
{
	switch(clock_sel)
	{
	case HAL_WKT_CLOCK_SOURCE_FRO_DIV:
	{
		PMU_disable_wake_up_clock_pin();
		WKT_set_internal_clock_source();
		WKT_select_clock_source(WKT_CLOCK_SOURCE_DIVIDED_FRO);

		break;
	}

	case HAL_WKT_CLOCK_SOURCE_LOW_POWER_OSC:
	{
		PMU_disable_wake_up_clock_pin();
		PMU_enable_low_power_oscillator();
		WKT_set_internal_clock_source();
		WKT_select_clock_source(WKT_CLOCK_SOURCE_LOW_POWER_CLOCK);

		break;
	}

	case HAL_WKT_CLOCK_SOURCE_EXTERNAL:
	{
		PMU_enable_wake_up_clock_pin();
		current_ext_clock = ext_clock_value;
		WKT_set_external_clock_source();

		break;
	}
	default: break;
	}
}

void hal_wkt_register_callback(hal_wkt_callback_t new_callback, void *data)
{
	if(new_callback == NULL)
	{
		hal_wkt_irq_callback = dummy_irq;
		NVIC_disable_interrupt(NVIC_IRQ_SEL_WKT);
	}
	else
	{
		hal_wkt_irq_callback = new_callback;
		hal_wkt_irq_callback_data = data;
		NVIC_clear_pending_interrupt(NVIC_IRQ_SEL_WKT);
		NVIC_enable_interrupt(NVIC_IRQ_SEL_WKT);
	}
}

void hal_wkt_start_count(uint32_t time_useg)
{
	float clock_base_value;
	uint32_t calculated_count;

	switch(current_clock_source)
	{
	case HAL_WKT_CLOCK_SOURCE_FRO_DIV: { clock_base_value = (float) (hal_syscon_fro_clock_get() / HAL_WKT_DIVIDE_VALUE); break; }
	case HAL_WKT_CLOCK_SOURCE_LOW_POWER_OSC: { clock_base_value = (float) (HAL_WKT_LOW_POWER_OSC_FREQ); break; }
	case HAL_WKT_CLOCK_SOURCE_EXTERNAL: { clock_base_value = (float) (current_ext_clock); break; }
	}

	calculated_count = (uint32_t) ((clock_base_value * time_useg) / 1e6);

	WKT_write_count(calculated_count);
}

void hal_wkt_start_count_with_value(uint32_t value)
{
	WKT_write_count(value);
}

static void dummy_irq(void* data)
{
	(void) data;
	return;
}

/**
 * @brief Interrupcion de WKT
 */
void WKT_IRQHandler(void)
{
	hal_wkt_irq_callback(hal_wkt_irq_callback_data);

	if(WKT_get_alarm_flag())
	{
		WKT_clear_alarm_flag();
	}
}
