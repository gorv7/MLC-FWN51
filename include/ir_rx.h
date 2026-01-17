/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
ir_rx.h

NEC IR Protocol Receiver Header for MS51FB9AE
Ported from N76E003 - Following legacy project structure
--------------------------------------------------------------------------*/
#ifndef _IR_RX_H_
#define _IR_RX_H_

/* Include headers following legacy project pattern */
#include "MS51_16K.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"

#define IR_DATA_LEN 4   // 4 bytes of NEC IR data

typedef enum
{
    IR_USE_PIN_IT = 0,  // Use pin interrupt
    IR_USE_EXT0_IT,     // Use external interrupt 0
    IR_USE_EXT1_IT,     // Use external interrupt 1
} IR_Type_t;

/**
 * @brief  Initialize IR receiver
 * @param  ir_type: Type of interrupt to use for IR reception
 * @retval None
 */
void ir_rx_setup(IR_Type_t ir_type);

/**
 * @brief  Check if IR data has been received
 * @retval 1 if data received, 0 otherwise
 */
uint8_t is_ir_data_received(void);

/**
 * @brief  Get the received IR data
 * @param  buf: Buffer to store received data
 * @param  len: Length of data to retrieve
 * @retval None
 */
void get_ir_data(uint8_t *buf, uint8_t len);

#endif
