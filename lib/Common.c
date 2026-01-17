/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*  Common.c                                                                            */
/*  Common Functions for MS51FB9AE @ 24MHz                                              */
/*--------------------------------------------------------------------------------------*/
#include "MS51_16K.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"

/* Global bit variable for interrupt-safe SFR access */
BIT BIT_TMP;

/**
 * @brief  Initialize UART0 with Timer1 as baud rate generator
 * @param  u32Baudrate: Desired baud rate (e.g., 115200)
 * @note   Configured for 24MHz HIRC
 */
void InitialUART0_Timer1(unsigned long u32Baudrate)
{
    /* Configure P0.6 (TXD) and P0.7 (RXD) for UART0 */
    P06_QUASI_MODE;
    P07_INPUT_MODE;
    
    /* Configure UART0 Mode 1: 8-bit, variable baud rate */
    SCON = 0x50;    /* Mode 1, REN enabled */
    TMOD |= 0x20;   /* Timer1 Mode 2: 8-bit auto-reload */
    
    /* Enable SMOD for double baud rate */
    set_PCON_SMOD;
    
    /* Calculate Timer1 reload value for 24MHz
     * TH1 = 256 - (2^SMOD * Fsys) / (32 * 12 * Baudrate)
     * With SMOD=1: TH1 = 256 - (2 * 24000000) / (32 * 12 * Baudrate)
     * Simplified: TH1 = 256 - (24000000) / (192 * Baudrate)
     *             TH1 = 256 - 125000 / Baudrate
     */
    TH1 = 256 - (24000000UL / 12UL / 16UL / u32Baudrate);
    
    /* Start Timer1 */
    set_TCON_TR1;
    
    /* Clear TI flag */
    clr_SCON_TI;
}

/**
 * @brief  Initialize UART0 with Timer3 as baud rate generator
 * @param  u32Baudrate: Desired baud rate
 * @note   Timer3 provides more accurate baud rates at high speeds
 */
void InitialUART0_Timer3(unsigned long u32Baudrate)
{
    /* Configure P0.6 (TXD) and P0.7 (RXD) for UART0 */
    P06_QUASI_MODE;
    P07_INPUT_MODE;
    
    /* Configure UART0 Mode 1: 8-bit, variable baud rate */
    SCON = 0x50;    /* Mode 1, REN enabled */
    
    /* Select Timer3 for UART0 baud rate */
    set_T3CON_BRCK;
    
    /* Enable SMOD for double baud rate */
    set_T3CON_SMOD_1;
    
    /* Calculate Timer3 reload value for 24MHz 
     * With SMOD_1=1: RH3/RL3 = 65536 - (Fsys / (16 * Baudrate))
     */
    RH3 = HIBYTE(65536 - (24000000UL / 16UL / u32Baudrate));
    RL3 = LOBYTE(65536 - (24000000UL / 16UL / u32Baudrate));
    
    /* Start Timer3 */
    set_T3CON_TR3;
    
    /* Clear TI flag */
    clr_SCON_TI;
}

/**
 * @brief  Initialize UART1 with Timer3 as baud rate generator
 * @param  u32Baudrate: Desired baud rate
 */
void InitialUART1_Timer3(unsigned long u32Baudrate)
{
    /* Configure P1.6 (TXD_1) and P0.2 (RXD_1) for UART1 */
    P16_PUSHPULL_MODE;
    P02_INPUT_MODE;
    
    /* Configure UART1 Mode 1: 8-bit, variable baud rate */
    SCON_1 = 0x50;  /* Mode 1, REN enabled */
    
    /* Select Timer3 for UART1 baud rate */
    set_T3CON_BRCK;
    set_T3CON_SMOD_1;
    
    /* Calculate Timer3 reload value */
    RH3 = HIBYTE(65536 - (24000000UL / 16UL / u32Baudrate));
    RL3 = LOBYTE(65536 - (24000000UL / 16UL / u32Baudrate));
    
    /* Start Timer3 */
    set_T3CON_TR3;
    
    /* Clear TI_1 flag */
    TI_1 = 0;
}

/**
 * @brief  Send one byte through UART0
 * @param  c: Byte to send
 */
void Send_Data_To_UART0(unsigned char c)
{
    clr_SCON_TI;
    SBUF = c;
    while(!TI);
    clr_SCON_TI;
}

/**
 * @brief  Send one byte through UART1
 * @param  c: Byte to send
 */
void Send_Data_To_UART1(unsigned char c)
{
    TI_1 = 0;
    SBUF_1 = c;
    while(!TI_1);
    TI_1 = 0;
}

/**
 * @brief  Receive one byte from UART0 (blocking)
 * @retval Received byte
 */
unsigned char Receive_Data_From_UART0(void)
{
    unsigned char c;
    while(!RI);
    c = SBUF;
    clr_SCON_RI;
    return c;
}

/**
 * @brief  Receive one byte from UART1 (blocking)
 * @retval Received byte
 */
unsigned char Receive_Data_From_UART1(void)
{
    unsigned char c;
    while(!RI_1);
    c = SBUF_1;
    RI_1 = 0;
    return c;
}
