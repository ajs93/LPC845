/**
 * @file HAL_CTIMER.h
 * @brief Declaraciones a nivel de aplicacion del periferico CTIMER (LPC845)
 * @author Augusto Santini
 * @date 3/2020
 * @version 1.0
 */

#ifndef HAL_CTIMER_H_
#define HAL_CTIMER_H_

#include <stdint.h>
#include <HAL_GPIO.h>

typedef enum
{
	HAL_CTIMER_MATCH_DO_NOGHING = 0,
	HAL_CTIMER_MATCH_CLEAR_PIN,
	HAL_CTIMER_MATCH_SET_PIN,
	HAL_CTIMER_MATCH_TOGGLE_PIN,
}hal_ctimer_match_action_en;

typedef struct
{
	uint8_t interrupt_on_match;
	uint8_t reset_on_match;
	uint8_t stop_on_match;
	uint8_t reload_on_match;
	uint32_t match_value_useg;
	hal_ctimer_match_action_en match_action;
	uint8_t enable_external_pin;
	hal_gpio_portpin_en match_pin;
	void (*callback)(void);
}hal_ctimer_match_config_t;

typedef enum
{
	HAL_CTIMER_MATCH_0 = 0,
	HAL_CTIMER_MATCH_1,
	HAL_CTIMER_MATCH_2,
	HAL_CTIMER_MATCH_3
}hal_ctimer_match_sel_en;

typedef struct
{
	uint8_t interrupt_on_action;
	uint32_t duty; //!< Duty en decimas de porciento (1 equivale a 0.1%)
	hal_gpio_portpin_en channel_pin;
	void (*callback)(void);
}hal_ctimer_pwm_channel_config_t;

typedef struct
{
	uint32_t clock_div; //!< Corresponde al numero deseado a dividir menos 1
	uint32_t pwm_period_useg; //!< Periodo del PWM en microsegundos
	uint8_t interrupt_on_period;
	void (*callback)(void);
}hal_ctimer_pwm_config_t;

typedef enum
{
	HAL_CTIMER_PWM_CHANNEL_0 = 0,
	HAL_CTIMER_PWM_CHANNEL_1,
	HAL_CTIMER_PWM_CHANNEL_2
}hal_ctimer_pwm_channel_sel_en;

/**
 * @brief Inicializacion del periferico en modo timer
 *
 * Esta funcion no pone a correr el contador.
 *
 * @param[in] clock_div Divisor del clock principal deseado (el valor efectivo es este valor + 1)
 */
void hal_ctimer_timer_mode_init(uint32_t clock_div);

/**
 * @brief Configurar un canal de match
 * @param[in] match_sel Match a configurar
 * @param[in] match_config Configuracion deseada
 */
void hal_ctimer_timer_mode_config_match(hal_ctimer_match_sel_en match_sel, const hal_ctimer_match_config_t *match_config);

/**
 * @brief Habilitar el conteo del ctimer
 */
void hal_ctimer_timer_mode_run(void);

/**
 * @brief Inhabilitar el conteo del ctimer
 */
void hal_ctimer_timer_mode_stop(void);

/**
 * @brief Reiniciar el conteo del ctimer
 */
void hal_ctimer_timer_mode_reset(void);

/**
 * @brief Inicializar el CTIMER en modo PWM
 *
 * @param[in] config Configuracion deseada
 */
void hal_ctimer_pwm_mode_init(const hal_ctimer_pwm_config_t *config);

/**
 * @brief Actualizar el periodo en modo PWM
 * @param[in] period_useg Nuevo periodo deseado en microsegundos
 */
void hal_ctimer_pwm_mode_set_period(uint32_t period_useg);

/**
 * @brief Actualizar configuracion de algun canal de PWM
 * @param[in] channel_sel Seleccion de canal a configurar
 * @param[in] channel_config Configuracion del canal de PWM
 */
void hal_ctimer_pwm_mode_config_channel(hal_ctimer_pwm_channel_sel_en channel_sel, const hal_ctimer_pwm_channel_config_t *channel_config);

#endif /* HAL_CTIMER_H_ */
