/**
 * @file timer.c
 * @brief Funciones para el manejo de timers por software
 * @author Augusto Santini
 * @date 12/2019
 * @version 1.0
 */

#include <stdint.h>
#include <timer.h>

#define		MAX_TIMERS			15

typedef struct
{
	uint8_t running : 1;
	uint8_t repeat : 1;
	uint8_t timeouted : 1;
	uint32_t msecs;
	uint32_t msecs_to_live;
	void (*callback)(void);
}timer_t;

static timer_t timers[MAX_TIMERS];

/**
 * @brief Iniciar un timer
 * @param[in] event_number Numero de evento asociado
 * @param[in] msecs Tiempo (en milisegundos) del timer a vencer
 * @param[in] callback Callback a ejecutar una vez vencido el timer. El parametro pasado al callback es el numero de evento que genero el llamado.
 * @param[in] repeat Si es 0 es un timer one-shot, caso contrario es repetitivo
 */
void timer_start(uint8_t event_number, uint32_t msecs, void (*callback)(uint8_t), uint8_t repeat)
{
	timers[event_number].msecs = msecs;
	timers[event_number].msecs_to_live = msecs;
	timers[event_number].repeat = repeat;
	timers[event_number].callback = callback;

	if(msecs > 0)
	{
		timers[event_number].timeouted = 0;
		timers[event_number].running = 1;
	}
	else
	{
		timers[event_number].timeouted = 1;
		timers[event_number].running = 0;
	}
}

/**
 * @brief Pausar un timer
 *
 * Ejecutar esta funcion con un timer que no esta corriendo no tiene ningun efecto.
 *
 * @param[in] event_number Numero de evento asociado
 */
void timer_pause(uint8_t event_number)
{
	timers[event_number].running = 0;
}

/**
 * @brief Despausar/resumir un timer
 *
 * Ejecutar esta funcion con un timer que esta corriendo no tiene ningun efecto.
 *
 * @param[in] event_number Numero de evento asociado
 */
void timer_resume(uint8_t event_number)
{
	timers[event_number].running = 1;
}

/**
 * @brief Detener un timer
 * @param[in] event_number Numero de evento asociado
 */
void timer_stop(uint8_t event_number)
{
	timers[event_number].running = 0;
	timers[event_number].msecs_to_live = 0;
}

/**
 * @brief Funcion a ejecutar en el loop principal
 *
 * Esta funcion se encargara de llamar a los callbacks de los timers que hayan vencido
 */
void timer_loop(void)
{
	uint8_t counter;

	for(counter = 0; counter < MAX_TIMERS; counter++)
	{
		if(timers[counter].timeouted == 1)
		{
			timers[counter].timeouted = 0;
			timers[counter].callback(counter);
		}
	}
}

/**
 * @brief Callback para revisar la maquinaria de timers (a ejecutar cada 1mseg)
 */
void timer_check(void)
{
	uint8_t counter;

	for(counter = 0; counter < MAX_TIMERS; counter++)
	{
		if(timers[counter].running == 1)
		{
			if(timers[counter].msecs_to_live > 0)
			{
				timers[counter].msecs_to_live--;

				if(timers[counter].msecs_to_live == 0)
				{
					timers[counter].timeouted = 1;

					if(timers[counter].repeat == 1)
					{
						timers[counter].msecs_to_live = timers[counter].msecs;
					}
					else
					{
						timers[counter].running = 0;
					}
				}
			}
		}
	}
}
