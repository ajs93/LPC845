/**
 * @file HAL_SYSTICK.c
 * @brief Funciones a nivel de aplicacion para el SYSTICK (LPC845)
 * @author Augusto Santini
 * @date 3/2020
 * @version 1.0
 */

#include <HAL_SYSTICK.h>
#include <HAL_SYSCON.h>
#include <HPL_SYSTICK.h>

static void dummy_irq(void);

static void (*systick_callback)(void) = dummy_irq; //!< Callback a llamar en la interrupcion

/**
 * @brief Inicializacion del SYSTICK
 * @param[in] tick_us Tiempo en microsegundos deseado para el tick
 * @param[in] callback Funcion a llamar en cada tick
 */
void hal_systick_init(uint32_t tick_us, void (*callback)(void))
{
	uint32_t aux;

	// En base a los us deseados calculo el valor de STRELOAD
	aux = hal_syscon_get_system_clock() / 10;
	aux *= tick_us;
	aux /= 100000; // La cuenta hecha asi aumenta la presicion sin generar nunca un overflow

	aux--;

	if(aux >= (1 << 24))
	{
		// En este caso habria overflow. Lo fijo al maximo.
		aux = (1 << 24) - 1;
	}

	SYSTICK_set_reload(aux);

	SYSTICK_select_clock_source(SYSTICK_CLOCK_SOURCE_MAIN_CLOCK);

	SYSTICK_set_clear_current_value();

	SYSTICK_enable_count();

	systick_callback = callback;

	SYSTICK_enable_interrupt();
}

/**
 * @brief Actualizar callback del SYSTICK
 * @param[in] callback Nuevo callback a ejecutar en cada tick
 */
void hal_systick_update_callback(void (*callback)(void))
{
	systick_callback = callback;
}

/**
 * @brief Interrupcion de SYSTICK
 */
void SysTick_Handler(void)
{
	systick_callback();
}

/**
 * @brief Dummy function para inicializar los punteros a los callbacks
 */
static void dummy_irq(void)
{
	return;
}