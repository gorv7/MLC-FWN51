/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*  Delay.h                                                                             */
/*  Timer-based Delay Functions for MS51FB9AE @ 24MHz                                   */
/*--------------------------------------------------------------------------------------*/
#ifndef __DELAY_H__
#define __DELAY_H__

/*--------------------------------------------------------------------------------------*/
/* Delay Functions                                                                      */
/* These use Timer2 for accurate delays at 24MHz HIRC                                   */
/*--------------------------------------------------------------------------------------*/

/**
 * @brief  Delay approximately 100us using Timer2
 * @param  u16CNT: Number of 100us units to delay
 * @note   Accuracy depends on HIRC tolerance
 */
void Timer2_Delay100us(unsigned int u16CNT);

/**
 * @brief  Delay approximately 500us using Timer2
 * @param  u16CNT: Number of 500us units to delay
 * @note   Accuracy depends on HIRC tolerance
 */
void Timer2_Delay500us(unsigned int u16CNT);

/**
 * @brief  Delay approximately 1ms using Timer2
 * @param  u16CNT: Number of 1ms units to delay
 * @note   Accuracy depends on HIRC tolerance
 */
void Timer2_Delay1ms(unsigned int u16CNT);

/**
 * @brief  Simple software delay (not accurate, for short waits)
 * @param  u16CNT: Delay count
 */
void Timer0_Delay1ms(unsigned int u16CNT);

/**
 * @brief  Delay approximately 10us using Timer3
 * @param  u16CNT: Number of 10us units to delay
 * @note   At 24MHz with /4 prescaler: 60 ticks = 10us
 */
void Timer3_Delay10us(unsigned int u16CNT);

#endif /* __DELAY_H__ */
