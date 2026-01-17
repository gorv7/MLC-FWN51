/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*  Common.h                                                                            */
/*  Common Definitions and UART Functions for MS51FB9AE                                 */
/*--------------------------------------------------------------------------------------*/
#ifndef __COMMON_H__
#define __COMMON_H__

/*--------------------------------------------------------------------------------------*/
/* Global bit variable for interrupt-safe SFR access                                    */
/* This is required by set_SFRPAGE/clr_SFRPAGE macros in SFR_Macro.h                    */
/*--------------------------------------------------------------------------------------*/
extern BIT BIT_TMP;

/*--------------------------------------------------------------------------------------*/
/* UART Initialization Functions                                                        */
/*--------------------------------------------------------------------------------------*/
void InitialUART0_Timer1(unsigned long u32Baudrate);
void InitialUART0_Timer3(unsigned long u32Baudrate);
void InitialUART1_Timer3(unsigned long u32Baudrate);

/*--------------------------------------------------------------------------------------*/
/* UART Transmit Functions                                                              */
/*--------------------------------------------------------------------------------------*/
void Send_Data_To_UART0(unsigned char c);
void Send_Data_To_UART1(unsigned char c);
unsigned char Receive_Data_From_UART0(void);
unsigned char Receive_Data_From_UART1(void);

#endif /* __COMMON_H__ */
