/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*  Function_define.h                                                                   */
/*  Common Macros and Type Definitions for MS51FB9AE                                    */
/*--------------------------------------------------------------------------------------*/
#ifndef __FUNCTION_DEFINE_H__
#define __FUNCTION_DEFINE_H__

#include <intrins.h>

/*--------------------------------------------------------------------------------------*/
/* Type Definitions                                                                     */
/*--------------------------------------------------------------------------------------*/
typedef unsigned char   UINT8;
typedef unsigned int    UINT16;
typedef unsigned long   UINT32;

typedef signed char     INT8;
typedef signed int      INT16;
typedef signed long     INT32;

typedef unsigned char   uint8_t;
typedef unsigned int    uint16_t;
typedef unsigned long   uint32_t;

typedef signed char     int8_t;
typedef signed int      int16_t;
typedef signed long     int32_t;

/* Keil C51 bit type - used for bit-addressable variables */
typedef bit             BIT;

/*--------------------------------------------------------------------------------------*/
/* Clock Definitions                                                                    */
/*--------------------------------------------------------------------------------------*/
#define FOSC_160000     0
#define FOSC_240000     1

/*--------------------------------------------------------------------------------------*/
/* GPIO Mode Macros                                                                     */
/* For MS51FB9AE:                                                                       */
/*   M1=0, M2=0 : Quasi-bidirectional                                                   */
/*   M1=0, M2=1 : Push-pull output                                                      */
/*   M1=1, M2=0 : Input only (high impedance)                                           */
/*   M1=1, M2=1 : Open-drain                                                            */
/*--------------------------------------------------------------------------------------*/

/* Port 0 GPIO Mode Macros */
#define P00_QUASI_MODE      P0M1&=0xFE;P0M2&=0xFE
#define P01_QUASI_MODE      P0M1&=0xFD;P0M2&=0xFD
#define P02_QUASI_MODE      P0M1&=0xFB;P0M2&=0xFB
#define P03_QUASI_MODE      P0M1&=0xF7;P0M2&=0xF7
#define P04_QUASI_MODE      P0M1&=0xEF;P0M2&=0xEF
#define P05_QUASI_MODE      P0M1&=0xDF;P0M2&=0xDF
#define P06_QUASI_MODE      P0M1&=0xBF;P0M2&=0xBF
#define P07_QUASI_MODE      P0M1&=0x7F;P0M2&=0x7F

#define P00_PUSHPULL_MODE   P0M1&=0xFE;P0M2|=0x01
#define P01_PUSHPULL_MODE   P0M1&=0xFD;P0M2|=0x02
#define P02_PUSHPULL_MODE   P0M1&=0xFB;P0M2|=0x04
#define P03_PUSHPULL_MODE   P0M1&=0xF7;P0M2|=0x08
#define P04_PUSHPULL_MODE   P0M1&=0xEF;P0M2|=0x10
#define P05_PUSHPULL_MODE   P0M1&=0xDF;P0M2|=0x20
#define P06_PUSHPULL_MODE   P0M1&=0xBF;P0M2|=0x40
#define P07_PUSHPULL_MODE   P0M1&=0x7F;P0M2|=0x80

#define P00_INPUT_MODE      P0M1|=0x01;P0M2&=0xFE
#define P01_INPUT_MODE      P0M1|=0x02;P0M2&=0xFD
#define P02_INPUT_MODE      P0M1|=0x04;P0M2&=0xFB
#define P03_INPUT_MODE      P0M1|=0x08;P0M2&=0xF7
#define P04_INPUT_MODE      P0M1|=0x10;P0M2&=0xEF
#define P05_INPUT_MODE      P0M1|=0x20;P0M2&=0xDF
#define P06_INPUT_MODE      P0M1|=0x40;P0M2&=0xBF
#define P07_INPUT_MODE      P0M1|=0x80;P0M2&=0x7F

#define P00_OPENDRAIN_MODE  P0M1|=0x01;P0M2|=0x01
#define P01_OPENDRAIN_MODE  P0M1|=0x02;P0M2|=0x02
#define P02_OPENDRAIN_MODE  P0M1|=0x04;P0M2|=0x04
#define P03_OPENDRAIN_MODE  P0M1|=0x08;P0M2|=0x08
#define P04_OPENDRAIN_MODE  P0M1|=0x10;P0M2|=0x10
#define P05_OPENDRAIN_MODE  P0M1|=0x20;P0M2|=0x20
#define P06_OPENDRAIN_MODE  P0M1|=0x40;P0M2|=0x40
#define P07_OPENDRAIN_MODE  P0M1|=0x80;P0M2|=0x80

/* Port 1 GPIO Mode Macros */
#define P10_QUASI_MODE      P1M1&=0xFE;P1M2&=0xFE
#define P11_QUASI_MODE      P1M1&=0xFD;P1M2&=0xFD
#define P12_QUASI_MODE      P1M1&=0xFB;P1M2&=0xFB
#define P13_QUASI_MODE      P1M1&=0xF7;P1M2&=0xF7
#define P14_QUASI_MODE      P1M1&=0xEF;P1M2&=0xEF
#define P15_QUASI_MODE      P1M1&=0xDF;P1M2&=0xDF
#define P16_QUASI_MODE      P1M1&=0xBF;P1M2&=0xBF
#define P17_QUASI_MODE      P1M1&=0x7F;P1M2&=0x7F

#define P10_PUSHPULL_MODE   P1M1&=0xFE;P1M2|=0x01
#define P11_PUSHPULL_MODE   P1M1&=0xFD;P1M2|=0x02
#define P12_PUSHPULL_MODE   P1M1&=0xFB;P1M2|=0x04
#define P13_PUSHPULL_MODE   P1M1&=0xF7;P1M2|=0x08
#define P14_PUSHPULL_MODE   P1M1&=0xEF;P1M2|=0x10
#define P15_PUSHPULL_MODE   P1M1&=0xDF;P1M2|=0x20
#define P16_PUSHPULL_MODE   P1M1&=0xBF;P1M2|=0x40
#define P17_PUSHPULL_MODE   P1M1&=0x7F;P1M2|=0x80

#define P10_INPUT_MODE      P1M1|=0x01;P1M2&=0xFE
#define P11_INPUT_MODE      P1M1|=0x02;P1M2&=0xFD
#define P12_INPUT_MODE      P1M1|=0x04;P1M2&=0xFB
#define P13_INPUT_MODE      P1M1|=0x08;P1M2&=0xF7
#define P14_INPUT_MODE      P1M1|=0x10;P1M2&=0xEF
#define P15_INPUT_MODE      P1M1|=0x20;P1M2&=0xDF
#define P16_INPUT_MODE      P1M1|=0x40;P1M2&=0xBF
#define P17_INPUT_MODE      P1M1|=0x80;P1M2&=0x7F

#define P10_OPENDRAIN_MODE  P1M1|=0x01;P1M2|=0x01
#define P11_OPENDRAIN_MODE  P1M1|=0x02;P1M2|=0x02
#define P12_OPENDRAIN_MODE  P1M1|=0x04;P1M2|=0x04
#define P13_OPENDRAIN_MODE  P1M1|=0x08;P1M2|=0x08
#define P14_OPENDRAIN_MODE  P1M1|=0x10;P1M2|=0x10
#define P15_OPENDRAIN_MODE  P1M1|=0x20;P1M2|=0x20
#define P16_OPENDRAIN_MODE  P1M1|=0x40;P1M2|=0x40
#define P17_OPENDRAIN_MODE  P1M1|=0x80;P1M2|=0x80

/* Port 3 GPIO Mode Macros */
#define P30_QUASI_MODE      P3M1&=0xFE;P3M2&=0xFE
#define P30_PUSHPULL_MODE   P3M1&=0xFE;P3M2|=0x01
#define P30_INPUT_MODE      P3M1|=0x01;P3M2&=0xFE
#define P30_OPENDRAIN_MODE  P3M1|=0x01;P3M2|=0x01

/*--------------------------------------------------------------------------------------*/
/* PWM Macros                                                                           */
/*--------------------------------------------------------------------------------------*/
#define PWM1_P14_OUTPUT_ENABLE   PIOCON0|=0x02
#define PWM1_P14_OUTPUT_DISABLE  PIOCON0&=0xFD
#define PWM5_P15_OUTPUT_ENABLE   PIOCON1|=0x02
#define PWM5_P15_OUTPUT_DISABLE  PIOCON1&=0xFD

#define PWM0_P12_OUTPUT_ENABLE   PIOCON0|=0x01
#define PWM0_P12_OUTPUT_DISABLE  PIOCON0&=0xFE
#define PWM2_P10_OUTPUT_ENABLE   PIOCON0|=0x04
#define PWM2_P10_OUTPUT_DISABLE  PIOCON0&=0xFB
#define PWM3_P00_OUTPUT_ENABLE   PIOCON0|=0x08
#define PWM3_P00_OUTPUT_DISABLE  PIOCON0&=0xF7
#define PWM4_P01_OUTPUT_ENABLE   PIOCON1|=0x01
#define PWM4_P01_OUTPUT_DISABLE  PIOCON1&=0xFE

/*--------------------------------------------------------------------------------------*/
/* Timer Mode Macros                                                                    */
/*--------------------------------------------------------------------------------------*/
#define TIMER0_MODE0    TMOD&=0xF0
#define TIMER0_MODE1    TMOD&=0xF0;TMOD|=0x01
#define TIMER0_MODE2    TMOD&=0xF0;TMOD|=0x02
#define TIMER0_MODE3    TMOD&=0xF0;TMOD|=0x03

#define TIMER1_MODE0    TMOD&=0x0F
#define TIMER1_MODE1    TMOD&=0x0F;TMOD|=0x10
#define TIMER1_MODE2    TMOD&=0x0F;TMOD|=0x20
#define TIMER1_MODE3    TMOD&=0x0F;TMOD|=0x30

/*--------------------------------------------------------------------------------------*/
/* UART Mode Macros                                                                     */
/*--------------------------------------------------------------------------------------*/
#define UART0_MODE0     SCON&=0x3F
#define UART0_MODE1     SCON&=0x3F;SCON|=0x40
#define UART0_MODE2     SCON&=0x3F;SCON|=0x80
#define UART0_MODE3     SCON|=0xC0

/*--------------------------------------------------------------------------------------*/
/* Interrupt Enable/Disable Macros                                                      */
/*--------------------------------------------------------------------------------------*/
#define ENABLE_GLOBAL_INTERRUPT     EA=1
#define DISABLE_GLOBAL_INTERRUPT    EA=0

#define ENABLE_TIMER0_INTERRUPT     ET0=1
#define DISABLE_TIMER0_INTERRUPT    ET0=0

#define ENABLE_TIMER1_INTERRUPT     ET1=1
#define DISABLE_TIMER1_INTERRUPT    ET1=0

#define ENABLE_UART0_INTERRUPT      ES=1
#define DISABLE_UART0_INTERRUPT     ES=0

#define ENABLE_ADC_INTERRUPT        EADC=1
#define DISABLE_ADC_INTERRUPT       EADC=0

/*--------------------------------------------------------------------------------------*/
/* Pin Interrupt Macros                                                                 */
/*--------------------------------------------------------------------------------------*/
#define ENABLE_PIT0_P00_LOWLEVEL        PICON&=0xFB;PINEN|=0x01;PIPEN&=0xFE
#define ENABLE_PIT0_P00_HIGHLEVEL       PICON&=0xFB;PINEN&=0xFE;PIPEN|=0x01
#define ENABLE_PIT0_P00_BOTHEDGE        PICON&=0xFB;PINEN|=0x01;PIPEN|=0x01
#define ENABLE_PIT0_P00_FALLINGEDGE     PICON|=0x04;PINEN|=0x01;PIPEN&=0xFE
#define ENABLE_PIT0_P00_RISINGEDGE      PICON|=0x04;PINEN&=0xFE;PIPEN|=0x01

#define ENABLE_PIT5_P05_FALLINGEDGE     PICON|=0x04;PINEN|=0x20;PIPEN&=0xDF
#define ENABLE_PIT5_P05_RISINGEDGE      PICON|=0x04;PINEN&=0xDF;PIPEN|=0x20
#define ENABLE_PIT5_P05_BOTHEDGE        PICON|=0x04;PINEN|=0x20;PIPEN|=0x20

/*--------------------------------------------------------------------------------------*/
/* Utility Macros                                                                       */
/*--------------------------------------------------------------------------------------*/
#define NOP     _nop_()
#define nop     _nop_()

#define HIBYTE(v1)              ((UINT8)((v1)>>8))
#define LOBYTE(v1)              ((UINT8)((v1)&0xFF))

#define MAKELONG(a, b)          ((UINT32)(((UINT16)(a))|((UINT32)((UINT16)(b)))<<16))
#define MAKEWORD(a, b)          ((UINT16)(((UINT8)(a))|((UINT16)((UINT8)(b)))<<8))

#define HIWORD(l)               ((UINT16)(((UINT32)(l)>>16)&0xFFFF))
#define LOWORD(l)               ((UINT16)(l))

#define BIT0    0x01
#define BIT1    0x02
#define BIT2    0x04
#define BIT3    0x08
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80

/*--------------------------------------------------------------------------------------*/
/* Boolean Macros                                                                       */
/*--------------------------------------------------------------------------------------*/
#define TRUE    1
#define FALSE   0

#define FAIL    1
#define PASS    0

/*--------------------------------------------------------------------------------------*/
/* SET/CLR BIT Macros (for legacy compatibility)                                        */
/*--------------------------------------------------------------------------------------*/
#define SET_BIT0        0x01
#define SET_BIT1        0x02
#define SET_BIT2        0x04
#define SET_BIT3        0x08
#define SET_BIT4        0x10
#define SET_BIT5        0x20
#define SET_BIT6        0x40
#define SET_BIT7        0x80

#define CLR_BIT0        0xFE
#define CLR_BIT1        0xFD
#define CLR_BIT2        0xFB
#define CLR_BIT3        0xF7
#define CLR_BIT4        0xEF
#define CLR_BIT5        0xDF
#define CLR_BIT6        0xBF
#define CLR_BIT7        0x7F

/*--------------------------------------------------------------------------------------*/
/* GPIO Mode Macros - Legacy Naming Convention                                          */
/*--------------------------------------------------------------------------------------*/
/* Port 0 */
#define P00_Quasi_Mode          P0M1&=~SET_BIT0;P0M2&=~SET_BIT0
#define P01_Quasi_Mode          P0M1&=~SET_BIT1;P0M2&=~SET_BIT1
#define P02_Quasi_Mode          P0M1&=~SET_BIT2;P0M2&=~SET_BIT2
#define P03_Quasi_Mode          P0M1&=~SET_BIT3;P0M2&=~SET_BIT3
#define P04_Quasi_Mode          P0M1&=~SET_BIT4;P0M2&=~SET_BIT4
#define P05_Quasi_Mode          P0M1&=~SET_BIT5;P0M2&=~SET_BIT5
#define P06_Quasi_Mode          P0M1&=~SET_BIT6;P0M2&=~SET_BIT6
#define P07_Quasi_Mode          P0M1&=~SET_BIT7;P0M2&=~SET_BIT7

#define P00_PushPull_Mode       P0M1&=~SET_BIT0;P0M2|=SET_BIT0
#define P01_PushPull_Mode       P0M1&=~SET_BIT1;P0M2|=SET_BIT1
#define P02_PushPull_Mode       P0M1&=~SET_BIT2;P0M2|=SET_BIT2
#define P03_PushPull_Mode       P0M1&=~SET_BIT3;P0M2|=SET_BIT3
#define P04_PushPull_Mode       P0M1&=~SET_BIT4;P0M2|=SET_BIT4
#define P05_PushPull_Mode       P0M1&=~SET_BIT5;P0M2|=SET_BIT5
#define P06_PushPull_Mode       P0M1&=~SET_BIT6;P0M2|=SET_BIT6
#define P07_PushPull_Mode       P0M1&=~SET_BIT7;P0M2|=SET_BIT7

#define P00_Input_Mode          P0M1|=SET_BIT0;P0M2&=~SET_BIT0
#define P01_Input_Mode          P0M1|=SET_BIT1;P0M2&=~SET_BIT1
#define P02_Input_Mode          P0M1|=SET_BIT2;P0M2&=~SET_BIT2
#define P03_Input_Mode          P0M1|=SET_BIT3;P0M2&=~SET_BIT3
#define P04_Input_Mode          P0M1|=SET_BIT4;P0M2&=~SET_BIT4
#define P05_Input_Mode          P0M1|=SET_BIT5;P0M2&=~SET_BIT5
#define P06_Input_Mode          P0M1|=SET_BIT6;P0M2&=~SET_BIT6
#define P07_Input_Mode          P0M1|=SET_BIT7;P0M2&=~SET_BIT7

/* Port 1 */
#define P10_Quasi_Mode          P1M1&=~SET_BIT0;P1M2&=~SET_BIT0
#define P11_Quasi_Mode          P1M1&=~SET_BIT1;P1M2&=~SET_BIT1
#define P12_Quasi_Mode          P1M1&=~SET_BIT2;P1M2&=~SET_BIT2
#define P13_Quasi_Mode          P1M1&=~SET_BIT3;P1M2&=~SET_BIT3
#define P14_Quasi_Mode          P1M1&=~SET_BIT4;P1M2&=~SET_BIT4
#define P15_Quasi_Mode          P1M1&=~SET_BIT5;P1M2&=~SET_BIT5
#define P16_Quasi_Mode          P1M1&=~SET_BIT6;P1M2&=~SET_BIT6
#define P17_Quasi_Mode          P1M1&=~SET_BIT7;P1M2&=~SET_BIT7

#define P10_PushPull_Mode       P1M1&=~SET_BIT0;P1M2|=SET_BIT0
#define P11_PushPull_Mode       P1M1&=~SET_BIT1;P1M2|=SET_BIT1
#define P12_PushPull_Mode       P1M1&=~SET_BIT2;P1M2|=SET_BIT2
#define P13_PushPull_Mode       P1M1&=~SET_BIT3;P1M2|=SET_BIT3
#define P14_PushPull_Mode       P1M1&=~SET_BIT4;P1M2|=SET_BIT4
#define P15_PushPull_Mode       P1M1&=~SET_BIT5;P1M2|=SET_BIT5
#define P16_PushPull_Mode       P1M1&=~SET_BIT6;P1M2|=SET_BIT6
#define P17_PushPull_Mode       P1M1&=~SET_BIT7;P1M2|=SET_BIT7

#define P10_Input_Mode          P1M1|=SET_BIT0;P1M2&=~SET_BIT0
#define P11_Input_Mode          P1M1|=SET_BIT1;P1M2&=~SET_BIT1
#define P12_Input_Mode          P1M1|=SET_BIT2;P1M2&=~SET_BIT2
#define P13_Input_Mode          P1M1|=SET_BIT3;P1M2&=~SET_BIT3
#define P14_Input_Mode          P1M1|=SET_BIT4;P1M2&=~SET_BIT4
#define P15_Input_Mode          P1M1|=SET_BIT5;P1M2&=~SET_BIT5
#define P16_Input_Mode          P1M1|=SET_BIT6;P1M2&=~SET_BIT6
#define P17_Input_Mode          P1M1|=SET_BIT7;P1M2&=~SET_BIT7

/* Port 3 */
#define P30_Quasi_Mode          P3M1&=~SET_BIT0;P3M2&=~SET_BIT0

/* All GPIO Quasi Mode */
#define Set_All_GPIO_Quasi_Mode P0M1=0;P0M2=0;P1M1=0;P1M2=0;P3M1=0;P3M2=0

/*--------------------------------------------------------------------------------------*/
/* Pin Interrupt Enable Macros (Legacy)                                                 */
/*--------------------------------------------------------------------------------------*/
#define Enable_INT_Port0                PICON&=0xFB
#define Enable_INT_Port1                PICON|=0x01
#define Enable_INT_Port2                PICON|=0x02
#define Enable_INT_Port3                PICON|=0x03

#define Enable_BIT7_LowLevel_Trig       PICON&=0x7F;PINEN|=0x80;PIPEN&=0x7F
#define Enable_BIT6_LowLevel_Trig       PICON&=0x7F;PINEN|=0x40;PIPEN&=0xBF
#define Enable_BIT5_LowLevel_Trig       PICON&=0xBF;PINEN|=0x20;PIPEN&=0xDF
#define Enable_BIT4_LowLevel_Trig       PICON&=0xBF;PINEN|=0x10;PIPEN&=0xEF
#define Enable_BIT3_LowLevel_Trig       PICON&=0xDF;PINEN|=0x08;PIPEN&=0xF7
#define Enable_BIT2_LowLevel_Trig       PICON&=0xEF;PINEN|=0x04;PIPEN&=0xFB
#define Enable_BIT1_LowLevel_Trig       PICON&=0xF7;PINEN|=0x02;PIPEN&=0xFD
#define Enable_BIT0_LowLevel_Trig       PICON&=0xFD;PINEN|=0x01;PIPEN&=0xFE

#define Enable_BIT7_FallEdge_Trig       PICON|=0x80;PINEN|=0x80;PIPEN&=0x7F
#define Enable_BIT6_FallEdge_Trig       PICON|=0x80;PINEN|=0x40;PIPEN&=0xBF
#define Enable_BIT5_FallEdge_Trig       PICON|=0x40;PINEN|=0x20;PIPEN&=0xDF
#define Enable_BIT4_FallEdge_Trig       PICON|=0x40;PINEN|=0x10;PIPEN&=0xEF
#define Enable_BIT3_FallEdge_Trig       PICON|=0x20;PINEN|=0x08;PIPEN&=0xF7
#define Enable_BIT2_FallEdge_Trig       PICON|=0x10;PINEN|=0x04;PIPEN&=0xFB
#define Enable_BIT1_FallEdge_Trig       PICON|=0x08;PINEN|=0x02;PIPEN&=0xFD
#define Enable_BIT0_FallEdge_Trig       PICON|=0x04;PINEN|=0x01;PIPEN&=0xFE

/*--------------------------------------------------------------------------------------*/
/* PWM Configuration Macros (Legacy)                                                    */
/*--------------------------------------------------------------------------------------*/
/* PWM clock source select */
#define PWM_CLOCK_FSYS              CKCON&=0xBF
#define PWM_CLOCK_TIMER1            CKCON|=0x40

/* PWM clock divider */
#define PWM_CLOCK_DIV_2             PWMCON1|=0x01;PWMCON1&=0xF9
#define PWM_CLOCK_DIV_4             PWMCON1|=0x02;PWMCON1&=0xFA
#define PWM_CLOCK_DIV_8             PWMCON1|=0x03;PWMCON1&=0xFB
#define PWM_CLOCK_DIV_16            PWMCON1|=0x04;PWMCON1&=0xFC
#define PWM_CLOCK_DIV_32            PWMCON1|=0x05;PWMCON1&=0xFD
#define PWM_CLOCK_DIV_64            PWMCON1|=0x06;PWMCON1&=0xFE
#define PWM_CLOCK_DIV_128           PWMCON1|=0x07

/* PWM type */
#define PWM_EDGE_TYPE               PWMCON1&=~SET_BIT4
#define PWM_CENTER_TYPE             PWMCON1|=SET_BIT4

/* PWM mode */
#define PWM_IMDEPENDENT_MODE        PWMCON1&=0x3F
#define PWM_COMPLEMENTARY_MODE      PWMCON1|=0x40;PWMCON1&=0x7F
#define PWM_SYNCHRONIZED_MODE       PWMCON1|=0x80;PWMCON1&=0xBF

/* PWM output polarity */
#define PWM_OUTPUT_ALL_NORMAL       PNP=0x00
#define PWM_OUTPUT_ALL_INVERSE      PNP=0xFF

/* PWM control shortcuts */
#define set_PWMRUN                  PWMRUN=1
#define clr_PWMRUN                  PWMRUN=0
#define set_CLRPWM                  CLRPWM=1

#endif /* __FUNCTION_DEFINE_H__ */
