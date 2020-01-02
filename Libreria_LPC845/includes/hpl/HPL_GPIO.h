/**
 * @file HPL_GPIO.h
 * @brief Declaraciones a nivel de abstraccion de periferico del GPIO (LPC845)
 * @author Augusto Santini
 * @date 6/2019
 * @version 1.0
 */

#ifndef HPL_GPIO_H_
#define HPL_GPIO_H_

#include <stdint.h>

#define	GPIO_DIR_INPUT						0
#define	GPIO_DIR_OUTPUT						1

#define	GPIO_SET_DIR_SUCCESS				0
#define	GPIO_SET_DIR_NOT_CLOCKED			-1
#define	GPIO_SET_DIR_INVALID_PORT			-2
#define	GPIO_SET_DIR_INVALID_PIN			-3
#define	GPIO_SET_DIR_INVALID_PORTPIN		-4

#define	GPIO_SET_PIN_SUCCESS				0
#define	GPIO_SET_PIN_NOT_CLOCKED			-1
#define	GPIO_SET_PIN_INVALID_PORT			-2
#define	GPIO_SET_PIN_INVALID_PIN			-3
#define	GPIO_SET_PIN_INVALID_PORTPIN		-4

#define	GPIO_CLEAR_PIN_SUCCESS				0
#define	GPIO_CLEAR_PIN_NOT_CLOCKED			-1
#define	GPIO_CLEAR_PIN_INVALID_PORT			-2
#define	GPIO_CLEAR_PIN_INVALID_PIN			-3
#define	GPIO_CLEAR_PIN_INVALID_PORTPIN		-4

#define	GPIO_TOGGLE_PIN_SUCCESS				0
#define	GPIO_TOGGLE_PIN_NOT_CLOCKED			-1
#define	GPIO_TOGGLE_PIN_INVALID_PORT		-2
#define	GPIO_TOGGLE_PIN_INVALID_PIN			-3
#define	GPIO_TOGGLE_PIN_INVALID_PORTPIN		-4

#define	GPIO_READ_PIN_NOT_CLOCKED			-1
#define	GPIO_READ_PIN_INVALID_PORT			-2
#define	GPIO_READ_PIN_INVALID_PIN			-3
#define	GPIO_READ_PIN_INVALID_PORTPIN		-4

/**
 * @brief Inicializa el puerto
 *
 * Lo unico que hace es habilitar el clock para el puerto requerido
 *
 * @param[in] port Puerto a inicializar. Si se pasa un puerto invalido no hace nada
 */
void GPIO_init(uint32_t port);

/**
 * @brief Inhabilita el puerto
 *
 * Lo unico que hace es inhabilitar el clock para el puerto requerido
 *
 * @param[in] port Puerto a inhabilitar. Si se pasa un puerto invalido no hace nada
 */
void GPIO_deinit(uint32_t port);

/**
 * @brief Inicializar un pin como entrada o salida
 * @param[in] port Puerto del pin a inicializar
 * @param[in] pin Numero del pin a inicializar
 * @param[in] dir Direccion del pin a inicializar. GPIO_DIR_INPUT o GPIO_DIR_OUTPUT
 * @param[in] initial_state Estado inicial del pin en caso de ser configurado como salida.
 * @return Estado del inicio de conversiones del ADC
 */
int32_t GPIO_set_dir(uint32_t port, uint32_t pin, uint8_t dir, uint8_t initial_state);

/**
 * @brief Fijar nivel del pin en esatado alto
 * @param[in] port Puerto del pin a accionar
 * @param[in] pin Numero del pin a accionar
 * @return Estado de accionado del pin
 */
int32_t GPIO_set_pin(uint32_t port, uint32_t pin);

/**
 * @brief Fijar nivel del pin en estado bajo
 * @param[in] port Puerto del pin a accionar
 * @param[in] pin Numero del pin a accionar
 * @return Estado de accionado del pin
 */
int32_t GPIO_clear_pin(uint32_t port, uint32_t pin);

/**
 * @brief Invertir estado actual del pin
 * @param[in] port Puerto del pin a accionar
 * @param[in] pin Numero del pin a accionar
 * @return Estado de inversion del pin
 */
int32_t GPIO_toggle_pin(uint32_t port, uint32_t pin);

/**
 * @brief Lectura del estado de un pin
 * @param[in] port Puerto del pin a leer
 * @param[in] pin Numero del pin a leer
 * @return Lectura del pin. Puede devolver errores
 */
int32_t GPIO_read_pin(uint32_t port, uint32_t pin);

#endif /* HPL_GPIO_H_ */
