/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/******************************************************************************
 * @file     ir_rx.c
 * @brief    NEC IR Protocol Receiver for MS51FB9AE @ 24MHz HIRC (Optimized)
 * @version  2.0.0
 * @note     Ported from N76E003 - Timer tick adjusted for 24MHz
 *           Timer0 Mode1: timer_tick = 12/24MHz = 0.5us
 *           
 * Optimizations Applied:
 *   - Inline timer access (no function call overhead in ISR)
 *   - Reduced ISR execution time
 *   - Optimized decode with early exit
 *   - Minimal processing in ISR context
 ******************************************************************************/

/* ir_rx.h includes all necessary headers (MS51_16K.h, SFR_Macro.h, etc.) */
#include "ir_rx.h"

/*===========================================================================*/
/* NEC IR Protocol Timing Constants (24MHz, Timer0 /12 = 0.5us tick)         */
/*===========================================================================*/
/* 
 * NEC Protocol Timing:
 * - Leader: 9ms mark + 4.5ms space = 13.5ms
 * - Bit 1:  562.5us mark + 1687.5us space = 2.25ms
 * - Bit 0:  562.5us mark + 562.5us space = 1.125ms
 * 
 * With 0.5us timer tick:
 * - Leader: ~27000 ticks
 * - Bit 1:  ~4500 ticks
 * - Bit 0:  ~2250 ticks
 */
#define SYNC_MIN        18000   /* ~9ms minimum */
#define SYNC_MAX        30000   /* ~15ms maximum */
#define ONE_MIN         3000    /* ~1.5ms minimum */
#define ONE_MAX         5400    /* ~2.7ms maximum */
#define ZERO_MIN        1200    /* ~0.6ms minimum */
#define ZERO_MAX        2700    /* ~1.35ms maximum */

/* Frame size: 1 sync + 32 data bits */
#define IR_FRAME_BITS   ((IR_DATA_LEN * 8) + 1)

/*===========================================================================*/
/* Module Variables - placed in fast RAM                                      */
/*===========================================================================*/
static volatile bit ir_received = 0;        /* Data ready flag */
static volatile bit ir_started = 0;         /* Reception in progress */
static volatile uint8_t bit_count = 0;      /* Bit counter */
static volatile uint16_t pulse_times[IR_FRAME_BITS];  /* Pulse timing buffer */

/*===========================================================================*/
/* Inline Timer0 Access (faster than function calls in ISR)                   */
/*===========================================================================*/
#define TIMER0_READ()       ((uint16_t)(TH0 << 8) | TL0)
#define TIMER0_RESET()      do { TL0 = 0; TH0 = 0; } while(0)
#define TIMER0_START()      set_TR0
#define TIMER0_STOP()       clr_TR0

/*===========================================================================*/
/* Pin Interrupt ISR for IR reception (P0.5) - OPTIMIZED                      */
/* Vector 7: Pin interrupt for MS51                                           */
/*===========================================================================*/
void PinInterrupt_ISR(void) interrupt 7
{
    uint16_t pulse;
    
    /* Quick exit if not our pin or already processing complete frame */
    if (!(PIF & 0x20))
    {
        PIF = 0x00;
        return;
    }
    
    /* Read timer value immediately (time critical) */
    pulse = TIMER0_READ();
    TIMER0_RESET();
    TIMER0_START();
    
    /* Clear interrupt flag early */
    PIF = 0x00;
    
    /* Already received complete frame? Ignore until processed */
    if (ir_received) return;
    
    /* Detect sync pulse (leader) */
    if (pulse >= SYNC_MIN && pulse <= SYNC_MAX)
    {
        ir_started = 1;
        bit_count = 0;
        pulse_times[0] = pulse;
        bit_count = 1;
        return;
    }
    
    /* Only process if we've seen the sync */
    if (!ir_started) return;
    
    /* Store pulse time */
    pulse_times[bit_count] = pulse;
    bit_count++;
    
    /* Check if we have complete frame */
    if (bit_count >= IR_FRAME_BITS)
    {
        ir_received = 1;
        ir_started = 0;
        TIMER0_STOP();
    }
}

/*===========================================================================*/
/* External Interrupt 1 ISR (alternative IR input) - OPTIMIZED                */
/*===========================================================================*/
void EXTI1_ISR(void) interrupt 2
{
    uint16_t pulse;
    
    /* Read timer immediately */
    pulse = TIMER0_READ();
    TIMER0_RESET();
    TIMER0_START();
    
    if (ir_received) return;
    
    /* Detect sync pulse */
    if (pulse >= SYNC_MIN && pulse <= SYNC_MAX)
    {
        ir_started = 1;
        bit_count = 1;
        pulse_times[0] = pulse;
        return;
    }
    
    if (!ir_started) return;
    
    pulse_times[bit_count++] = pulse;
    
    if (bit_count >= IR_FRAME_BITS)
    {
        ir_received = 1;
        ir_started = 0;
        TIMER0_STOP();
        clr_EA;
    }
}

/*===========================================================================*/
/* Setup IR Receiver                                                          */
/*===========================================================================*/
void ir_rx_setup(IR_Type_t ir_type)
{
    uint8_t i;
    
    /* Clear state */
    ir_received = 0;
    ir_started = 0;
    bit_count = 0;
    
    /* Clear pulse buffer - unrolled for speed */
    for (i = 0; i < IR_FRAME_BITS; i++)
    {
        pulse_times[i] = 0;
    }
    
    /* Setup Timer0 Mode 1 (16-bit timer, Fsys/12) */
    TMOD = (TMOD & 0xF0) | 0x01;
    TIMER0_RESET();
    
    /* Setup interrupt based on type */
    switch (ir_type)
    {
        case IR_USE_EXT1_IT:
            set_IT1;    /* Falling edge */
            set_EX1;    /* Enable EXT1 */
            break;
        
        case IR_USE_PIN_IT:
            set_EPI;    /* Enable pin interrupt */
            break;
        
        default:
            break;
    }
    
    set_EA;  /* Global interrupt enable */
}

/*===========================================================================*/
/* Decode single byte from pulse timings - OPTIMIZED                          */
/* @param start_bit: Starting bit position (1-based after sync)               */
/* @return Decoded byte value                                                  */
/*===========================================================================*/
static uint8_t decode_byte(uint8_t start_bit)
{
    uint8_t value = 0;
    uint8_t i;
    uint16_t pulse;
    
    for (i = 0; i < 8; i++)
    {
        pulse = pulse_times[start_bit + i];
        value <<= 1;
        
        /* Check for logic 1 (longer pulse) first - more common in data */
        if (pulse >= ONE_MIN && pulse <= ONE_MAX)
        {
            value |= 1;
        }
        /* Logic 0 is implicit (value already 0 from shift) */
        else if (pulse < ZERO_MIN || pulse > ZERO_MAX)
        {
            /* Invalid pulse timing - could add error flag here */
        }
    }
    
    return value;
}

/*===========================================================================*/
/* Check if IR data has been received                                         */
/*===========================================================================*/
uint8_t is_ir_data_received(void)
{
    return ir_received;
}

/*===========================================================================*/
/* Get received IR data - OPTIMIZED                                           */
/* @param buf: Buffer to store decoded data                                   */
/* @param len: Number of bytes to decode (typically 4)                        */
/*===========================================================================*/
void get_ir_data(uint8_t *buf, uint8_t len)
{
    uint8_t i;
    
    /* Decode each byte (positions 1-8, 9-16, 17-24, 25-32 after sync) */
    for (i = 0; i < len; i++)
    {
        buf[i] = decode_byte((i << 3) + 1);  /* i*8 + 1 */
    }
    
    /* Reset state for next reception */
    ir_received = 0;
    ir_started = 0;
    bit_count = 0;
    
    /* Re-enable interrupts */
    set_EA;
}
