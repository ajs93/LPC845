/**
 * @file LCD.c
 * @brief Funciones del display LCD (2x16)
 * @author Augusto Santini
 * @date 1/2020
 * @version 1.0
 */

#include <stdint.h>
#include <LCD.h>
#include <infotronic.h>
#include <HPL_GPIO.h>

#define		LCD_D4_PORT				0
#define		LCD_D4_PIN				9
#define		LCD_D5_PORT				0
#define		LCD_D5_PIN				10
#define		LCD_D6_PORT				0
#define		LCD_D6_PIN				11
#define		LCD_D7_PORT				0
#define		LCD_D7_PIN				13

#define		LCD_EN_PORT				0
#define		LCD_EN_PIN				14

#define		LCD_RS_PORT				0
#define		LCD_RS_PIN				15

#define		LCD_CONTROL				0
#define		LCD_DATA				1

#define		LCD_CONTROL_FLAG		(0x80)

#define		LCD_BUFFER_LENGTH		50

#define		LCD_RENGLON_LENGTH		16

static uint8_t LCD_buffer[LCD_BUFFER_LENGTH];
static uint8_t LCD_out_idx;
static uint8_t LCD_in_idx;
static uint8_t LCD_buffer_counter = 0;

static void LCD_push(uint8_t data, uint8_t control);
static int32_t LCD_pop(void);

/**
 * @brief Inicializacion del display LCD (2x16)
 */
void LCD_init(void)
{
	uint32_t counter;

	GPIO_set_dir(LCD_D4_PORT, LCD_D4_PIN, GPIO_DIR_OUTPUT, 0);
	GPIO_set_dir(LCD_D5_PORT, LCD_D5_PIN, GPIO_DIR_OUTPUT, 0);
	GPIO_set_dir(LCD_D6_PORT, LCD_D6_PIN, GPIO_DIR_OUTPUT, 0);
	GPIO_set_dir(LCD_D7_PORT, LCD_D7_PIN, GPIO_DIR_OUTPUT, 0);
	GPIO_set_dir(LCD_EN_PORT, LCD_EN_PIN, GPIO_DIR_OUTPUT, 0);
	GPIO_set_dir(LCD_RS_PORT, LCD_RS_PIN, GPIO_DIR_OUTPUT, 0);

	GPIO_clear_pin(LCD_EN_PORT, LCD_EN_PIN);

	infotronic_blocking_delay(20);

	for(counter = 0; counter < 3; counter++)
	{
		GPIO_set_pin(LCD_D4_PORT, LCD_D4_PIN);
		GPIO_set_pin(LCD_D5_PORT, LCD_D5_PIN);
		GPIO_clear_pin(LCD_D6_PORT, LCD_D6_PIN);
		GPIO_clear_pin(LCD_D7_PORT, LCD_D7_PIN);

		GPIO_clear_pin(LCD_RS_PORT, LCD_RS_PIN);

		GPIO_set_pin(LCD_EN_PORT, LCD_EN_PIN);
		GPIO_clear_pin(LCD_EN_PORT, LCD_EN_PIN);

		infotronic_blocking_delay(4);
	}

	GPIO_clear_pin(LCD_D4_PORT, LCD_D4_PIN);
	GPIO_set_pin(LCD_D5_PORT, LCD_D5_PIN);
	GPIO_clear_pin(LCD_D6_PORT, LCD_D6_PIN);
	GPIO_clear_pin(LCD_D7_PORT, LCD_D7_PIN);

	GPIO_clear_pin(LCD_RS_PORT, LCD_RS_PIN);

	GPIO_set_pin(LCD_EN_PORT, LCD_EN_PIN);
	GPIO_clear_pin(LCD_EN_PORT, LCD_EN_PIN);

	infotronic_blocking_delay(2);

	LCD_push(0x28, LCD_CONTROL); // N = 1; 2 lineas, 5x7 puntos
	LCD_push(0x08, LCD_CONTROL); // Cursor off
	LCD_push(0x01, LCD_CONTROL); // Clear display
	LCD_push(0x06, LCD_CONTROL); // Puntero incremental
	LCD_push(0x0C, LCD_CONTROL); // Activo LCD
}

/**
 * @brief Escribir un mensaje en el display LCD
 * @param[in] message Puntero al mensaje a escribir, terminando con el caracter nulo
 * @param[in] renglon Renglon en cual escribir el mensaje (LCD_RENGLON_1 o LCD_RENGLON_2)
 * @param[in] start_position Posicion del renglon a partir de la cual comenzar a escribir el mensaje
 */
void LCD_write(char * message, uint8_t renglon, uint8_t start_position)
{
	uint32_t counter;

	switch(renglon)
	{
		case LCD_RENGLON_1:
		{
			LCD_push(start_position + 0x80, LCD_CONTROL);

			break;
		}

		case LCD_RENGLON_2:
		{
			LCD_push(start_position + 0xC0, LCD_CONTROL);

			break;
		}
	}

	for(counter = 0; (message[counter] != '\0') && (counter < LCD_RENGLON_LENGTH); counter++)
	{
		LCD_push(message[counter], LCD_DATA);
	}
}

/**
 * @brief Callback para actualizar el LCD periodicamente (a ser llamada cada 1mseg)
 */
void LCD_check(void)
{
	int32_t data = LCD_pop();

	if(data != -1)
	{
		if((data >> 0) & 0x01 == 0x01)
		{
			GPIO_set_pin(LCD_D4_PORT, LCD_D4_PIN);
		}
		else
		{
			GPIO_clear_pin(LCD_D4_PORT, LCD_D4_PIN);
		}

		if((data >> 1) & 0x01 == 0x01)
		{
			GPIO_set_pin(LCD_D5_PORT, LCD_D5_PIN);
		}
		else
		{
			GPIO_clear_pin(LCD_D5_PORT, LCD_D5_PIN);
		}

		if((data >> 2) & 0x01 == 0x01)
		{
			GPIO_set_pin(LCD_D6_PORT, LCD_D6_PIN);
		}
		else
		{
			GPIO_clear_pin(LCD_D6_PORT, LCD_D6_PIN);
		}

		if((data >> 3) & 0x01 == 0x01)
		{
			GPIO_set_pin(LCD_D7_PORT, LCD_D7_PIN);
		}
		else
		{
			GPIO_clear_pin(LCD_D7_PORT, LCD_D7_PIN);
		}

		if(data & 0x80)
		{
			GPIO_clear_pin(LCD_RS_PORT, LCD_RS_PIN);
		}
		else
		{
			GPIO_set_pin(LCD_RS_PORT, LCD_RS_PIN);
		}

		GPIO_set_pin(LCD_EN_PORT, LCD_EN_PIN);
		GPIO_clear_pin(LCD_EN_PORT, LCD_EN_PIN);
	}
}

/**
 * @brief Colocar un dato (control o data) en el LCD
 */
static void LCD_push(uint8_t data, uint8_t control)
{
	if(LCD_buffer_counter >= LCD_BUFFER_LENGTH)
	{
		return;
	}

	LCD_buffer[LCD_in_idx] = (data >> 4) & 0x0F;

	if(control == LCD_CONTROL)
	{
		LCD_buffer[LCD_in_idx] |= LCD_CONTROL_FLAG;
	}

	LCD_in_idx = (LCD_in_idx + 1) % LCD_BUFFER_LENGTH;
	LCD_buffer_counter++;

	LCD_buffer[LCD_in_idx] = data & 0x0F;

	if(control == LCD_CONTROL)
	{
		LCD_buffer[LCD_in_idx] |= LCD_CONTROL_FLAG;
	}

	LCD_in_idx = (LCD_in_idx + 1) % LCD_BUFFER_LENGTH;
	LCD_buffer_counter++;
}

/**
 * @brief Colocar un dato (control o data) en el LCD
 * @return En caso de no haber dato -1, caso contrario el dato en el buffer
 */
static int32_t LCD_pop(void)
{
	int32_t data = -1;

	if(LCD_buffer_counter > 0)
	{
		data = (int32_t) LCD_buffer[LCD_out_idx];

		LCD_buffer_counter--;

		LCD_out_idx = (LCD_out_idx + 1) % LCD_BUFFER_LENGTH;
	}

	return data;
}
