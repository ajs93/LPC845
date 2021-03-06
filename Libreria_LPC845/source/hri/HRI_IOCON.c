/**
 * @file HRI_IOCON.c
 * @brief Declaración del periférico IOCON (LPC845)
 * @author Augusto Santini
 * @date 4/2020
 * @version 1.0
 */

#include <stdint.h>
#include <HRI_IOCON.h>

volatile IOCON_per_t * const IOCON = (IOCON_per_t *) IOCON_BASE; //!< Periferico IOCON

volatile IOCON_PIO_reg_t dummy_reg; //!< Registro dummy para los pines no disponibles en el encapsulado

volatile IOCON_PIO_reg_t * const IOCON_PIN_TABLE[2][32] = //!< Tabla de registros de configuracion
{
	{ // Port 0
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_0, (IOCON_PIO_reg_t * ) &IOCON->PIO0_1,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_2, (IOCON_PIO_reg_t * ) &IOCON->PIO0_3,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_4, (IOCON_PIO_reg_t * ) &IOCON->PIO0_5,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_6, (IOCON_PIO_reg_t * ) &IOCON->PIO0_7,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_8, (IOCON_PIO_reg_t * ) &IOCON->PIO0_9,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_10, (IOCON_PIO_reg_t * ) &IOCON->PIO0_11,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_12, (IOCON_PIO_reg_t * ) &IOCON->PIO0_13,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_14, (IOCON_PIO_reg_t * ) &IOCON->PIO0_15,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_16, (IOCON_PIO_reg_t * ) &IOCON->PIO0_17,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_18, (IOCON_PIO_reg_t * ) &IOCON->PIO0_19,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_20, (IOCON_PIO_reg_t * ) &IOCON->PIO0_21,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_22, (IOCON_PIO_reg_t * ) &IOCON->PIO0_23,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_24, (IOCON_PIO_reg_t * ) &IOCON->PIO0_25,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_26, (IOCON_PIO_reg_t * ) &IOCON->PIO0_27,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_28, (IOCON_PIO_reg_t * ) &IOCON->PIO0_29,
		(IOCON_PIO_reg_t * ) &IOCON->PIO0_30, (IOCON_PIO_reg_t * ) &IOCON->PIO0_31
	},

	{ // Port 1
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_0, (IOCON_PIO_reg_t * ) &IOCON->PIO1_1,
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_2, (IOCON_PIO_reg_t * ) &IOCON->PIO1_3,
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_4, (IOCON_PIO_reg_t * ) &IOCON->PIO1_5,
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_6, (IOCON_PIO_reg_t * ) &IOCON->PIO1_7,
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_8, (IOCON_PIO_reg_t * ) &IOCON->PIO1_9,
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_10, (IOCON_PIO_reg_t * ) &IOCON->PIO1_11,
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_12, (IOCON_PIO_reg_t * ) &IOCON->PIO1_13,
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_14, (IOCON_PIO_reg_t * ) &IOCON->PIO1_15,
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_16, (IOCON_PIO_reg_t * ) &IOCON->PIO1_17,
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_18, (IOCON_PIO_reg_t * ) &IOCON->PIO1_19,
		(IOCON_PIO_reg_t * ) &IOCON->PIO1_20, (IOCON_PIO_reg_t * ) &IOCON->PIO1_21,
		&dummy_reg, &dummy_reg,
		&dummy_reg, &dummy_reg, &dummy_reg, &dummy_reg,
		&dummy_reg, &dummy_reg, &dummy_reg, &dummy_reg
	},
};
