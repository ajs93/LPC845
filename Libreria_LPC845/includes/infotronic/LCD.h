/**
 * @file LCD.h
 * @brief Declaraciones para el display LCD (2x16)
 * @author Augusto Santini
 * @date 12/2019
 * @version 1.0
 */

#ifndef LCD_H_
#define LCD_H_

#define		LCD_RENGLON_1		0
#define		LCD_RENGLON_2		1

/**
 * @brief Inicializacion del display LCD (2x16)
 */
void LCD_init(void);

/**
 * @brief Escribir un mensaje en el display LCD
 * @param[in] message Puntero al mensaje a escribir, terminando con el caracter nulo
 * @param[in] renglon Renglon en cual escribir el mensaje (LCD_RENGLON_1 o LCD_RENGLON_2)
 * @param[in] start_position Posicion del renglon a partir de la cual comenzar a escribir el mensaje
 */
void LCD_write(char * message, uint8_t renglon, uint8_t start_position);

/**
 * @brief Callback para actualizar el LCD periodicamente (a ser llamada cada 1mseg)
 */
void LCD_check(void);

#endif /* LCD_H_ */
