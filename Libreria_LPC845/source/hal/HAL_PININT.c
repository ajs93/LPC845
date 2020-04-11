/**
 * @file HAL_PININT.c
 * @brief Funciones a nivel de aplicacion del periferico PININT (LPC845)
 * @author Augusto Santini
 * @date 3/2020
 * @version 1.0
 */

#include <HAL_PININT.h>
#include <HAL_UART.h>
#include <HPL_PININT.h>
#include <HPL_SYSCON.h>
#include <HPL_SWM.h>
#include <HPL_NVIC.h>

static void dummy_irq_callback(void);

static void (*pinint_callbacks[8])(void) = { //!< Callbacks para las 8 interrupciones disponibles
		dummy_irq_callback,
		dummy_irq_callback,
		dummy_irq_callback,
		dummy_irq_callback,
		dummy_irq_callback,
		dummy_irq_callback,
		dummy_irq_callback
};

static void hal_pinint_handle_irq(hal_pinint_channel_en channel);

/**
 * @brief Inicializacion del modulo
 */
void hal_pinint_init(void)
{
	SYSCON_enable_clock(SYSCON_ENABLE_CLOCK_SEL_GPIO_INT);
}

/**
 * @brief Configurar interrupciones de pin
 * @param[in] config Configuracion de interrupciones de pin
 */
void hal_pinint_pin_interrupt_config(const hal_pinint_config_t *config)
{
	PININT_set_interrupt_mode(config->channel, config->mode);

	if(config->mode == HAL_PININT_INTERRUPT_MODE_LEVEL)
	{
		if(config->int_on_level == HAL_PININT_LEVEL_INT_HIGH)
		{
			PININT_enable_high_level(config->channel);
		}
		else
		{
			PININT_disable_high_level(config->channel);
		}
	}
	else
	{
		if(config->int_on_rising_edge)
		{
			PININT_enable_rising_edge(config->channel);
		}

		if(config->int_on_falling_edge)
		{
			PININT_enable_falling_edge(config->channel);
		}
	}

	pinint_callbacks[config->channel] = config->callback;

	SYSCON_set_pinint_pin(config->channel, config->portpin);

	switch(config->channel)
	{
	case 0: { NVIC_enable_interrupt(NVIC_IRQ_SEL_PININT0); break; }
	case 1: { NVIC_enable_interrupt(NVIC_IRQ_SEL_PININT1); break; }
	case 2: { NVIC_enable_interrupt(NVIC_IRQ_SEL_PININT2); break; }
	case 3: { NVIC_enable_interrupt(NVIC_IRQ_SEL_PININT3); break; }
	case 4: { NVIC_enable_interrupt(NVIC_IRQ_SEL_PININT4); break; }
	case 5: { NVIC_enable_interrupt(NVIC_IRQ_SEL_PININT5_DAC1); break; }
	case 6: { NVIC_enable_interrupt(NVIC_IRQ_SEL_PININT6_UART3); break; }
	case 7: { NVIC_enable_interrupt(NVIC_IRQ_SEL_PININT7_UART4); break; }
	}
}

/**
 * @brief Registrar callback a llamar en interrupcion de PININTn
 * @param[in] channel Canal al cual registrar el callback
 * @param[in] new_callback Puntero a funcion a ejecutar
 */
void hal_pinint_register_callback(hal_pinint_channel_en channel, void (*new_callback)(void))
{
	pinint_callbacks[channel] = new_callback;
}

/**
 * @brief Funcion dummy para inicializar los punteros de interrupciones
 */
static void dummy_irq_callback(void)
{
	return;
}

/**
 * @brief Manejo de interrupciones para el modulo
 * @param[in] channel Canal que genero la itnerrupcion
 */
static void hal_pinint_handle_irq(hal_pinint_channel_en channel)
{
	pinint_callbacks[channel]();

	if(PININT_get_interrupt_mode(channel) == PININT_INTERRUPT_MODE_EDGE)
	{
		PININT_clear_edge_level_irq(channel);
	}
}

/**
 * @brief Interrupcion para PININT0
 */
void PININT0_IRQHandler(void)
{
	hal_pinint_handle_irq(0);
}

/**
 * @brief Interrupcion para PININT1
 */
void PININT1_IRQHandler(void)
{
	hal_pinint_handle_irq(1);
}

/**
 * @brief Interrupcion para PININT2
 */
void PININT2_IRQHandler(void)
{
	hal_pinint_handle_irq(2);
}

/**
 * @brief Interrupcion para PININT3
 */
void PININT3_IRQHandler(void)
{
	hal_pinint_handle_irq(3);
}

/**
 * @brief Interrupcion para PININT4
 */
void PININT4_IRQHandler(void)
{
	hal_pinint_handle_irq(4);
}

/**
 * @brief Interrupcion para PININT5
 */
void PININT5_IRQHandler(void)
{
	hal_pinint_handle_irq(5);
}

/**
 * @brief Interrupcion para PININT6 y USART3
 */
void PININT6_IRQHandler(void)
{
	if(PININT->IST.PSTAT & (1 << 6))
	{
		hal_pinint_handle_irq(6);
	}

	UART3_irq();
}

/**
 * @brief Interrupcion para PININT7 y USART4
 */
void PININT7_IRQHandler(void)
{
	if(PININT->IST.PSTAT & (1 << 7))
	{
		hal_pinint_handle_irq(7);
	}

	UART4_irq();
}
