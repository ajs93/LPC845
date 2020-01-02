/**
 * @file display.c
 * @brief Funciones para el manejo de los displays
 * @author Augusto Santini
 * @date 12/2019
 * @version 1.0
 */

#include <stdint.h>
#include <display.h>
#include <HPL_GPIO.h>

#define		CANTIDAD_DISPLAYS		2
#define		CANTIDAD_DIGITOS		3

#define		DISPLAY_BCD_A_PORT		0
#define		DISPLAY_BCD_A_PIN		20

#define		DISPLAY_BCD_B_PORT		0
#define		DISPLAY_BCD_B_PIN		23

#define		DISPLAY_BCD_C_PORT		0
#define		DISPLAY_BCD_C_PIN		22

#define		DISPLAY_BCD_D_PORT		0
#define		DISPLAY_BCD_D_PIN		21

#define		DISPLAY_DP_PORT			0
#define		DISPLAY_DP_PIN			12

#define		DISPLAY_RST_PORT		0
#define		DISPLAY_RST_PIN			18

#define		DISPLAY_CK_PORT			0
#define		DISPLAY_CK_PIN			19

#define		DISPLAY_BLANK_REPEATS	5

static uint8_t display_number_buffer[CANTIDAD_DISPLAYS][CANTIDAD_DIGITOS];
static uint8_t display_dp_buffer[CANTIDAD_DISPLAYS][CANTIDAD_DIGITOS];
static uint8_t display_power_state[CANTIDAD_DISPLAYS];

static inline void display_blank_digit(void);
static inline void display_decode_number(uint8_t number);

/**
 * @brief Inicializacion de los display
 */
void display_init(void)
{
	GPIO_set_dir(DISPLAY_BCD_A_PORT, DISPLAY_BCD_A_PIN, GPIO_DIR_OUTPUT, 0);
	GPIO_set_dir(DISPLAY_BCD_B_PORT, DISPLAY_BCD_B_PIN, GPIO_DIR_OUTPUT, 0);
	GPIO_set_dir(DISPLAY_BCD_C_PORT, DISPLAY_BCD_C_PIN, GPIO_DIR_OUTPUT, 0);
	GPIO_set_dir(DISPLAY_BCD_D_PORT, DISPLAY_BCD_D_PIN, GPIO_DIR_OUTPUT, 0);

	GPIO_set_dir(DISPLAY_DP_PORT, DISPLAY_DP_PIN, GPIO_DIR_OUTPUT, 1);

	GPIO_set_dir(DISPLAY_RST_PORT, DISPLAY_RST_PIN, GPIO_DIR_OUTPUT, 0);
	GPIO_set_dir(DISPLAY_CK_PORT, DISPLAY_CK_PIN, GPIO_DIR_OUTPUT, 0);
}

/**
 * @brief Apagar display
 * @param[in] display Numero de display a apagar (0 ~ 1)
 */
void display_off(uint8_t display)
{
	if(display > 1)
	{
		return;
	}

	display_power_state[display] = 0;
}

/**
 * @brief Encender display
 * @param[in] display Numero de display a encender (0 ~ 1)
 */
void display_on(uint8_t display)
{
	if(display > 1)
	{
		return;
	}

	display_power_state[display] = 1;
}

/**
 * @brief Escribir display
 * @param[in] display Numero de display sobre el cual escribir (0 ~ 1)
 * @param[in] number Numero a escribir en el display (0 ~ 999)
 */
void display_write(uint8_t display, uint32_t number)
{
	uint8_t counter;
	uint32_t aux;

	if(display >= CANTIDAD_DISPLAYS)
	{
		return;
	}

	aux = 1;

	for(counter = 1; counter < CANTIDAD_DIGITOS; counter++)
	{
		aux *= 10;
	}

	number %= aux * 10;

	for(counter = 0; counter < CANTIDAD_DIGITOS; counter++)
	{
		display_number_buffer[display][counter] = ((number / aux) % 10);

		aux /= 10;
	}
}

/**
 * @brief Encender el display point de un digito de un display
 * @param[in] display Numero de display sobre el cual escribir (0 ~ 1)
 * @param[in] digit Digito del display a prender (0 ~ 2)
 */
void display_set_dp(uint8_t display, uint8_t digit)
{
	if(display >= CANTIDAD_DISPLAYS || digit >= CANTIDAD_DIGITOS)
	{
		return;
	}

	display_dp_buffer[display][digit] = 1;
}

/**
 * @brief Apagar el display point de un digito de un display
 * @param[in] display Numero de display sobre el cual escribir (0 ~ 1)
 * @param[in] digit Digito del display a prender (0 ~ 2)
 */
void display_clear_dp(uint8_t display, uint8_t digit)
{
	if(display >= CANTIDAD_DISPLAYS || digit >= CANTIDAD_DIGITOS)
	{
		return;
	}

	display_dp_buffer[display][digit] = 0;
}

/**
 * @brief Callback para actualizar el display periodicamente
 */
void display_check(void)
{
	static uint8_t digit_counter = 0;
	uint8_t this_number;

	digit_counter = (digit_counter + 1) % (CANTIDAD_DIGITOS * CANTIDAD_DISPLAYS);

	this_number = display_number_buffer[digit_counter / CANTIDAD_DIGITOS][digit_counter % CANTIDAD_DIGITOS];

	if(digit_counter == 0)
	{
		GPIO_set_pin(DISPLAY_RST_PORT, DISPLAY_RST_PIN);
		GPIO_clear_pin(DISPLAY_RST_PORT, DISPLAY_RST_PIN);
	}
	else
	{
		GPIO_set_pin(DISPLAY_CK_PORT, DISPLAY_CK_PIN);
		GPIO_clear_pin(DISPLAY_CK_PORT, DISPLAY_CK_PIN);
	}

	if(display_power_state[digit_counter / CANTIDAD_DIGITOS] == 1)
	{
		display_decode_number(this_number);

		if(display_dp_buffer[digit_counter / CANTIDAD_DIGITOS][digit_counter % CANTIDAD_DIGITOS] == 1)
		{
			GPIO_set_pin(DISPLAY_DP_PORT, DISPLAY_DP_PIN);
		}
		else
		{
			GPIO_clear_pin(DISPLAY_DP_PORT, DISPLAY_DP_PIN);
		}
	}
	else
	{
		display_blank_digit();
	}
}

/**
 * @brief Apagar los 7 segmentos del digito
 */
static inline void display_blank_digit(void)
{
	GPIO_set_pin(DISPLAY_BCD_D_PORT, DISPLAY_BCD_D_PIN);
	GPIO_set_pin(DISPLAY_BCD_C_PORT, DISPLAY_BCD_C_PIN);
	GPIO_set_pin(DISPLAY_BCD_B_PORT, DISPLAY_BCD_B_PIN);
	GPIO_set_pin(DISPLAY_BCD_A_PORT, DISPLAY_BCD_A_PIN);
	GPIO_clear_pin(DISPLAY_DP_PORT, DISPLAY_DP_PIN);
}

/**
 * @brief Decodificar numero BCD en cada uno de lo pines
 * @param[in] number Numero a decodifica
 */
static inline void display_decode_number(uint8_t number)
{
	if(number & (1 << 3))
	{
		GPIO_set_pin(DISPLAY_BCD_D_PORT, DISPLAY_BCD_D_PIN);
	}
	else
	{
		GPIO_clear_pin(DISPLAY_BCD_D_PORT, DISPLAY_BCD_D_PIN);
	}

	if(number & (1 << 2))
	{
		GPIO_set_pin(DISPLAY_BCD_C_PORT, DISPLAY_BCD_C_PIN);
	}
	else
	{
		GPIO_clear_pin(DISPLAY_BCD_C_PORT, DISPLAY_BCD_C_PIN);
	}

	if(number & (1 << 1))
	{
		GPIO_set_pin(DISPLAY_BCD_B_PORT, DISPLAY_BCD_B_PIN);
	}
	else
	{
		GPIO_clear_pin(DISPLAY_BCD_B_PORT, DISPLAY_BCD_B_PIN);
	}

	if(number & (1 << 0))
	{
		GPIO_set_pin(DISPLAY_BCD_A_PORT, DISPLAY_BCD_A_PIN);
	}
	else
	{
		GPIO_clear_pin(DISPLAY_BCD_A_PORT, DISPLAY_BCD_A_PIN);
	}
}
