/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*  Delay.c                                                                             */
/*  Timer-based Delay Functions for MS51FB9AE @ 24MHz                                   */
/*--------------------------------------------------------------------------------------*/
#include "MS51_16K.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Delay.h"

/**
 * @brief  Delay approximately 100us using Timer2
 * @param  u16CNT: Number of 100us units to delay
 * @note   Timer tick = 12/24MHz = 0.5us
 *         For 100us: need 200 timer ticks
 *         Reload = 65536 - 200 = 65336 = 0xFF38
 */
void Timer2_Delay100us(unsigned int u16CNT)
{
    /* Clear Timer2 control */
    clr_T2CON_TF2;
    clr_T2CON_TR2;
    
    while(u16CNT--)
    {
        /* Set reload value for 100us at 24MHz (200 ticks) */
        TL2 = 0x38;
        TH2 = 0xFF;
        
        /* Start Timer2 */
        set_T2CON_TR2;
        
        /* Wait for overflow */
        while(!TF2);
        
        /* Clear flags */
        clr_T2CON_TF2;
        clr_T2CON_TR2;
    }
}

/**
 * @brief  Delay approximately 500us using Timer2
 * @param  u16CNT: Number of 500us units to delay
 * @note   For 500us: need 1000 timer ticks
 *         Reload = 65536 - 1000 = 64536 = 0xFC18
 */
void Timer2_Delay500us(unsigned int u16CNT)
{
    /* Clear Timer2 control */
    clr_T2CON_TF2;
    clr_T2CON_TR2;
    
    while(u16CNT--)
    {
        /* Set reload value for 500us at 24MHz (1000 ticks) */
        TL2 = 0x18;
        TH2 = 0xFC;
        
        /* Start Timer2 */
        set_T2CON_TR2;
        
        /* Wait for overflow */
        while(!TF2);
        
        /* Clear flags */
        clr_T2CON_TF2;
        clr_T2CON_TR2;
    }
}

/**
 * @brief  Delay approximately 1ms using Timer2
 * @param  u16CNT: Number of 1ms units to delay
 * @note   For 1ms: need 2000 timer ticks
 *         Reload = 65536 - 2000 = 63536 = 0xF830
 */
void Timer2_Delay1ms(unsigned int u16CNT)
{
    /* Clear Timer2 control */
    clr_T2CON_TF2;
    clr_T2CON_TR2;
    
    while(u16CNT--)
    {
        /* Set reload value for 1ms at 24MHz (2000 ticks) */
        TL2 = 0x30;
        TH2 = 0xF8;
        
        /* Start Timer2 */
        set_T2CON_TR2;
        
        /* Wait for overflow */
        while(!TF2);
        
        /* Clear flags */
        clr_T2CON_TF2;
        clr_T2CON_TR2;
    }
}

/**
 * @brief  Simple software delay using Timer0
 * @param  u16CNT: Number of 1ms units to delay
 * @note   For 1ms at 24MHz with Timer0/12: need 2000 ticks
 *         Reload = 65536 - 2000 = 63536 = 0xF830
 */
void Timer0_Delay1ms(unsigned int u16CNT)
{
    while(u16CNT--)
    {
        /* Timer0 Mode 1: 16-bit timer */
        clr_TCON_TF0;
        clr_TCON_TR0;
        TMOD &= 0xF0;
        TMOD |= 0x01;
        
        /* Load timer value */
        TL0 = 0x30;
        TH0 = 0xF8;
        
        /* Start Timer0 */
        set_TCON_TR0;
        
        /* Wait for overflow */
        while(!TF0);
        
        /* Stop Timer0 */
        clr_TCON_TR0;
        clr_TCON_TF0;
    }
}

/**
 * @brief  Delay approximately 10us using Timer3
 * @param  u16CNT: Number of 10us units to delay
 * @note   At 24MHz with /4 prescaler: 60 ticks = 10us
 *         Timer3 reload = 65536 - 60 = 65476 = 0xFFC4
 */
void Timer3_Delay10us(unsigned int u16CNT)
{
    unsigned int reload_value;
    
    /* At 24MHz/4 = 6MHz, each tick = 1/6us
     * For 10us, need 60 ticks
     * Reload value = 65536 - 60 = 65476 = 0xFFC4 */
    reload_value = 65536 - 60;
    
    /* Configure Timer3 */
    T3CON &= 0xF8;  /* Clear prescaler bits, /1 divider */
    T3CON |= 0x02;  /* Set /4 prescaler */
    
    while(u16CNT--)
    {
        /* Clear Timer3 flag */
        T3CON &= ~0x10;  /* Clear TF3 */
        
        /* Load reload value */
        RH3 = (unsigned char)(reload_value >> 8);
        RL3 = (unsigned char)(reload_value);
        
        /* Start Timer3 */
        T3CON |= 0x08;  /* Set TR3 */
        
        /* Wait for overflow */
        while(!(T3CON & 0x10));
        
        /* Stop Timer3 */
        T3CON &= ~0x08;  /* Clear TR3 */
        T3CON &= ~0x10;  /* Clear TF3 */
    }
}
