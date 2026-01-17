/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*  SFR_Macro.h                                                                         */
/*  SFR Bit Manipulation Macros for MS51FB9AE                                           */
/*--------------------------------------------------------------------------------------*/
#ifndef __SFR_MACRO_H__
#define __SFR_MACRO_H__

/********************************************************************/
/*  SFR Bit Set/Clear Macros                                        */
/********************************************************************/

/**** PCON  87H ****/
#define set_PCON_SMOD            PCON|=0x80
#define set_PCON_SMOD0           PCON|=0x40
#define set_PCON_POF             PCON|=0x10
#define set_PCON_GF1             PCON|=0x08
#define set_PCON_GF0             PCON|=0x04 
#define set_PCON_PD              PCON|=0x02
#define set_PCON_IDLE            PCON|=0x01
                                    
#define clr_PCON_SMOD            PCON&=0x7F
#define clr_PCON_SMOD0           PCON&=0xBF
#define clr_PCON_POF             PCON&=0xEF
#define clr_PCON_GF1             PCON&=0xF7
#define clr_PCON_GF0             PCON&=0xFB 
#define clr_PCON_PD              PCON&=0xFD
#define clr_PCON_IDLE            PCON&=0xFE

/**** TCON    88H ****/
#define set_TCON_TF1             TF1=1
#define set_TCON_TR1             TR1=1
#define set_TCON_TF0             TF0=1
#define set_TCON_TR0             TR0=1
#define set_TCON_IE1             IE1=1
#define set_TCON_IT1             IT1=1
#define set_TCON_IE0             IE0=1
#define set_TCON_IT0             IT0=1
                                 
#define clr_TCON_TF1             TF1=0
#define clr_TCON_TR1             TR1=0
#define clr_TCON_TF0             TF0=0
#define clr_TCON_TR0             TR0=0
#define clr_TCON_IE1             IE1=0
#define clr_TCON_IT1             IT1=0
#define clr_TCON_IE0             IE0=0
#define clr_TCON_IT0             IT0=0

/**** TMOD    89H ****/
#define set_TMOD_GATE_T1         TMOD|=0x80
#define set_TMOD_CT_T1           TMOD|=0x40
#define set_TMOD_M1_T1           TMOD|=0x20
#define set_TMOD_M0_T1           TMOD|=0x10
#define set_TMOD_GATE_T0         TMOD|=0x08
#define set_TMOD_CT_T0           TMOD|=0x04
#define set_TMOD_M1_T0           TMOD|=0x02
#define set_TMOD_M0_T0           TMOD|=0x01
                                 
#define clr_TMOD_GATE_T1         TMOD&=0x7F
#define clr_TMOD_CT_T1           TMOD&=0xBF
#define clr_TMOD_M1_T1           TMOD&=0xDF
#define clr_TMOD_M0_T1           TMOD&=0xEF
#define clr_TMOD_GATE_T0         TMOD&=0xF7
#define clr_TMOD_CT_T0           TMOD&=0xFB
#define clr_TMOD_M1_T0           TMOD&=0xFD
#define clr_TMOD_M0_T0           TMOD&=0xFE

/**** CKCON  8EH ****/
#define set_CKCON_PWMCKS         CKCON|=0x40
#define set_CKCON_T1M            CKCON|=0x10
#define set_CKCON_T0M            CKCON|=0x08
#define set_CKCON_CLOEN          CKCON|=0x02
                                 
#define clr_CKCON_PWMCKS         CKCON&=0xBF
#define clr_CKCON_T1M            CKCON&=0xEF
#define clr_CKCON_T0M            CKCON&=0xF7
#define clr_CKCON_CLOEN          CKCON&=0xFD

/**** WKCON  8FH ****/
#define set_WKCON_WKTF           WKCON|=0x10
#define set_WKCON_WKTR           WKCON|=0x08
#define set_WKCON_WKPS2          WKCON|=0x04
#define set_WKCON_WKPS1          WKCON|=0x02
#define set_WKCON_WKPS0          WKCON|=0x01
                                 
#define clr_WKCON_WKTF           WKCON&=0xEF
#define clr_WKCON_WKTR           WKCON&=0xF7
#define clr_WKCON_WKPS2          WKCON&=0xFB
#define clr_WKCON_WKPS1          WKCON&=0xFD
#define clr_WKCON_WKPS0          WKCON&=0xFE

/**** SFRS  91H ****/
#define set_SFRS_SFRPAGE         TA=0xAA;TA=0x55;SFRS|=0x01
#define clr_SFRS_SFRPAGE         TA=0xAA;TA=0x55;SFRS&=0xFE

/**** CAPCON0  92H ****/
#define set_CAPCON0_CAPEN2       CAPCON0|=0x40
#define set_CAPCON0_CAPEN1       CAPCON0|=0x20
#define set_CAPCON0_CAPEN0       CAPCON0|=0x10
#define set_CAPCON0_CAPF2        CAPCON0|=0x04
#define set_CAPCON0_CAPF1        CAPCON0|=0x02
#define set_CAPCON0_CAPF0        CAPCON0|=0x01
                                 
#define clr_CAPCON0_CAPEN2       CAPCON0&=0xBF
#define clr_CAPCON0_CAPEN1       CAPCON0&=0xDF
#define clr_CAPCON0_CAPEN0       CAPCON0&=0xEF
#define clr_CAPCON0_CAPF2        CAPCON0&=0xFB
#define clr_CAPCON0_CAPF1        CAPCON0&=0xFD
#define clr_CAPCON0_CAPF0        CAPCON0&=0xFE

/**** SCON  98H ****/
#define set_SCON_SM0             SM0=1
#define set_SCON_FE              FE=1
#define set_SCON_SM1             SM1=1
#define set_SCON_SM2             SM2=1
#define set_SCON_REN             REN=1
#define set_SCON_TB8             TB8=1
#define set_SCON_RB8             RB8=1
#define set_SCON_TI              TI=1
#define set_SCON_RI              RI=1
                                 
#define clr_SCON_SM0             SM0=0
#define clr_SCON_FE              FE=0
#define clr_SCON_SM1             SM1=0
#define clr_SCON_SM2             SM2=0
#define clr_SCON_REN             REN=0
#define clr_SCON_TB8             TB8=0
#define clr_SCON_RB8             RB8=0
#define clr_SCON_TI              TI=0
#define clr_SCON_RI              RI=0

/**** EIE  9BH ****/
#define set_EIE_ET2              EIE|=0x80
#define set_EIE_ESPI             EIE|=0x40
#define set_EIE_EFB              EIE|=0x20
#define set_EIE_EWDT             EIE|=0x10
#define set_EIE_EPWM             EIE|=0x08
#define set_EIE_ECAP             EIE|=0x04
#define set_EIE_EPI              EIE|=0x02
#define set_EIE_EI2C             EIE|=0x01
                                 
#define clr_EIE_ET2              EIE&=0x7F
#define clr_EIE_ESPI             EIE&=0xBF
#define clr_EIE_EFB              EIE&=0xDF
#define clr_EIE_EWDT             EIE&=0xEF
#define clr_EIE_EPWM             EIE&=0xF7
#define clr_EIE_ECAP             EIE&=0xFB
#define clr_EIE_EPI              EIE&=0xFD
#define clr_EIE_EI2C             EIE&=0xFE

/**** EIE1  9CH ****/
#define set_EIE1_EWKT            EIE1|=0x04
#define set_EIE1_ET3             EIE1|=0x02
#define set_EIE1_ES_1            EIE1|=0x01
                                 
#define clr_EIE1_EWKT            EIE1&=0xFB
#define clr_EIE1_ET3             EIE1&=0xFD
#define clr_EIE1_ES_1            EIE1&=0xFE

/**** CHPCON  9FH  TA Protection ****/
#define set_CHPCON_SWRST         TA=0xAA;TA=0x55;CHPCON|=0x80
#define set_CHPCON_IAPFF         TA=0xAA;TA=0x55;CHPCON|=0x40
#define set_CHPCON_BS            TA=0xAA;TA=0x55;CHPCON|=0x02
#define set_CHPCON_IAPEN         TA=0xAA;TA=0x55;CHPCON|=0x01
                                 
#define clr_CHPCON_SWRST         TA=0xAA;TA=0x55;CHPCON&=0x7F
#define clr_CHPCON_IAPFF         TA=0xAA;TA=0x55;CHPCON&=0xBF
#define clr_CHPCON_BS            TA=0xAA;TA=0x55;CHPCON&=0xFD
#define clr_CHPCON_IAPEN         TA=0xAA;TA=0x55;CHPCON&=0xFE

/**** IE  A8H ****/
#define set_IE_EA                EA=1
#define set_IE_EADC              EADC=1
#define set_IE_EBOD              EBOD=1
#define set_IE_ES                ES=1
#define set_IE_ET1               ET1=1
#define set_IE_EX1               EX1=1
#define set_IE_ET0               ET0=1
#define set_IE_EX0               EX0=1
                                 
#define clr_IE_EA                EA=0
#define clr_IE_EADC              EADC=0
#define clr_IE_EBOD              EBOD=0
#define clr_IE_ES                ES=0
#define clr_IE_ET1               ET1=0
#define clr_IE_EX1               EX1=0
#define clr_IE_ET0               ET0=0
#define clr_IE_EX0               EX0=0

/**** WDCON  AAH ****/
#define set_WDCON_WDTR           TA=0xAA;TA=0x55;WDCON|=0x80
#define set_WDCON_WDCLR          TA=0xAA;TA=0x55;WDCON|=0x40
#define set_WDCON_WDTF           TA=0xAA;TA=0x55;WDCON|=0x20
#define set_WDCON_WIDPD          TA=0xAA;TA=0x55;WDCON|=0x10
#define set_WDCON_WDTRF          TA=0xAA;TA=0x55;WDCON|=0x08
#define set_WDCON_WDPS2          TA=0xAA;TA=0x55;WDCON|=0x04
#define set_WDCON_WDPS1          TA=0xAA;TA=0x55;WDCON|=0x02
#define set_WDCON_WDPS0          TA=0xAA;TA=0x55;WDCON|=0x01
                                 
#define clr_WDCON_WDTR           TA=0xAA;TA=0x55;WDCON&=0x7F
#define clr_WDCON_WDCLR          TA=0xAA;TA=0x55;WDCON&=0xBF
#define clr_WDCON_WDTF           TA=0xAA;TA=0x55;WDCON&=0xDF
#define clr_WDCON_WIDPD          TA=0xAA;TA=0x55;WDCON&=0xEF
#define clr_WDCON_WDTRF          TA=0xAA;TA=0x55;WDCON&=0xF7
#define clr_WDCON_WDPS2          TA=0xAA;TA=0x55;WDCON&=0xFB
#define clr_WDCON_WDPS1          TA=0xAA;TA=0x55;WDCON&=0xFD
#define clr_WDCON_WDPS0          TA=0xAA;TA=0x55;WDCON&=0xFE

/**** T2CON  C8H ****/
#define set_T2CON_TF2            TF2=1
#define set_T2CON_TR2            TR2=1
#define set_T2CON_CM_RL2         CM_RL2=1
                                 
#define clr_T2CON_TF2            TF2=0
#define clr_T2CON_TR2            TR2=0
#define clr_T2CON_CM_RL2         CM_RL2=0

/**** T2MOD  C9H ****/
#define set_T2MOD_LDEN           T2MOD|=0x80
#define set_T2MOD_T2DIV2         T2MOD|=0x40
#define set_T2MOD_T2DIV1         T2MOD|=0x20
#define set_T2MOD_T2DIV0         T2MOD|=0x10
#define set_T2MOD_CAPCR          T2MOD|=0x08
#define set_T2MOD_CMPCR          T2MOD|=0x04
#define set_T2MOD_LDTS1          T2MOD|=0x02
#define set_T2MOD_LDTS0          T2MOD|=0x01
                                 
#define clr_T2MOD_LDEN           T2MOD&=0x7F
#define clr_T2MOD_T2DIV2         T2MOD&=0xBF
#define clr_T2MOD_T2DIV1         T2MOD&=0xDF
#define clr_T2MOD_T2DIV0         T2MOD&=0xEF
#define clr_T2MOD_CAPCR          T2MOD&=0xF7
#define clr_T2MOD_CMPCR          T2MOD&=0xFB
#define clr_T2MOD_LDTS1          T2MOD&=0xFD
#define clr_T2MOD_LDTS0          T2MOD&=0xFE

/**** PWMCON0  D8H ****/
#define set_PWMCON0_PWMRUN       PWMRUN=1
#define set_PWMCON0_LOAD         LOAD=1
#define set_PWMCON0_PWMF         PWMF=1
#define set_PWMCON0_CLRPWM       CLRPWM=1
                                 
#define clr_PWMCON0_PWMRUN       PWMRUN=0
#define clr_PWMCON0_LOAD         LOAD=0
#define clr_PWMCON0_PWMF         PWMF=0
#define clr_PWMCON0_CLRPWM       CLRPWM=0

/**** PWMCON1  DFH ****/
#define set_PWMCON1_PWMMOD1      PWMCON1|=0x80
#define set_PWMCON1_PWMMOD0      PWMCON1|=0x40
#define set_PWMCON1_GP           PWMCON1|=0x20
#define set_PWMCON1_PWMTYP       PWMCON1|=0x10
#define set_PWMCON1_FBINEN       PWMCON1|=0x08
#define set_PWMCON1_PWMDIV2      PWMCON1|=0x04
#define set_PWMCON1_PWMDIV1      PWMCON1|=0x02
#define set_PWMCON1_PWMDIV0      PWMCON1|=0x01
                                 
#define clr_PWMCON1_PWMMOD1      PWMCON1&=0x7F
#define clr_PWMCON1_PWMMOD0      PWMCON1&=0xBF
#define clr_PWMCON1_GP           PWMCON1&=0xDF
#define clr_PWMCON1_PWMTYP       PWMCON1&=0xEF
#define clr_PWMCON1_FBINEN       PWMCON1&=0xF7
#define clr_PWMCON1_PWMDIV2      PWMCON1&=0xFB
#define clr_PWMCON1_PWMDIV1      PWMCON1&=0xFD
#define clr_PWMCON1_PWMDIV0      PWMCON1&=0xFE

/**** ADCCON0  E8H ****/
#define set_ADCCON0_ADCF         ADCF=1
#define set_ADCCON0_ADCS         ADCS=1
#define set_ADCCON0_ETGSEL1      ETGSEL1=1
#define set_ADCCON0_ETGSEL0      ETGSEL0=1
#define set_ADCCON0_ADCHS3       ADCHS3=1
#define set_ADCCON0_ADCHS2       ADCHS2=1
#define set_ADCCON0_ADCHS1       ADCHS1=1
#define set_ADCCON0_ADCHS0       ADCHS0=1
                                 
#define clr_ADCCON0_ADCF         ADCF=0
#define clr_ADCCON0_ADCS         ADCS=0
#define clr_ADCCON0_ETGSEL1      ETGSEL1=0
#define clr_ADCCON0_ETGSEL0      ETGSEL0=0
#define clr_ADCCON0_ADCHS3       ADCHS3=0
#define clr_ADCCON0_ADCHS2       ADCHS2=0
#define clr_ADCCON0_ADCHS1       ADCHS1=0
#define clr_ADCCON0_ADCHS0       ADCHS0=0

/**** ADCCON1  E1H ****/
#define set_ADCCON1_ADCEN        ADCCON1|=0x01
#define clr_ADCCON1_ADCEN        ADCCON1&=0xFE

/**** ADCCON2  E2H ****/
#define set_ADCCON2_ADFBEN       ADCCON2|=0x80
#define set_ADCCON2_ADCMPOP      ADCCON2|=0x40
#define set_ADCCON2_ADCMPEN      ADCCON2|=0x20
#define set_ADCCON2_ADCMPO       ADCCON2|=0x10
                                 
#define clr_ADCCON2_ADFBEN       ADCCON2&=0x7F
#define clr_ADCCON2_ADCMPOP      ADCCON2&=0xBF
#define clr_ADCCON2_ADCMPEN      ADCCON2&=0xDF
#define clr_ADCCON2_ADCMPO       ADCCON2&=0xEF

/**** PICON  E9H ****/
#define set_PICON_PIT67          PICON|=0x80
#define set_PICON_PIT45          PICON|=0x40
#define set_PICON_PIT3           PICON|=0x20
#define set_PICON_PIT2           PICON|=0x10
#define set_PICON_PIT1           PICON|=0x08
#define set_PICON_PIT0           PICON|=0x04
#define set_PICON_PIPS1          PICON|=0x02
#define set_PICON_PIPS0          PICON|=0x01
                                 
#define clr_PICON_PIT67          PICON&=0x7F
#define clr_PICON_PIT45          PICON&=0xBF
#define clr_PICON_PIT3           PICON&=0xDF
#define clr_PICON_PIT2           PICON&=0xEF
#define clr_PICON_PIT1           PICON&=0xF7
#define clr_PICON_PIT0           PICON&=0xFB
#define clr_PICON_PIPS1          PICON&=0xFD
#define clr_PICON_PIPS0          PICON&=0xFE

/**** I2CON  C0H ****/
#define set_I2CON_I2CEN          I2CEN=1
#define set_I2CON_STA            STA=1
#define set_I2CON_STO            STO=1
#define set_I2CON_SI             SI=1
#define set_I2CON_AA             AA=1
#define set_I2CON_I2CPX          I2CPX=1
                                 
#define clr_I2CON_I2CEN          I2CEN=0
#define clr_I2CON_STA            STA=0
#define clr_I2CON_STO            STO=0
#define clr_I2CON_SI             SI=0
#define clr_I2CON_AA             AA=0
#define clr_I2CON_I2CPX          I2CPX=0

/**** T3CON  C4H ****/
#define set_T3CON_SMOD_1         T3CON|=0x80
#define set_T3CON_SMOD0_1        T3CON|=0x40
#define set_T3CON_BRCK           T3CON|=0x20
#define set_T3CON_TF3            T3CON|=0x10
#define set_T3CON_TR3            T3CON|=0x08
#define set_T3CON_T3PS2          T3CON|=0x04
#define set_T3CON_T3PS1          T3CON|=0x02
#define set_T3CON_T3PS0          T3CON|=0x01
                                 
#define clr_T3CON_SMOD_1         T3CON&=0x7F
#define clr_T3CON_SMOD0_1        T3CON&=0xBF
#define clr_T3CON_BRCK           T3CON&=0xDF
#define clr_T3CON_TF3            T3CON&=0xEF
#define clr_T3CON_TR3            T3CON&=0xF7
#define clr_T3CON_T3PS2          T3CON&=0xFB
#define clr_T3CON_T3PS1          T3CON&=0xFD
#define clr_T3CON_T3PS0          T3CON&=0xFE

/*--------------------------------------------------------------------------------------*/
/* Shorthand Macros (BSP compatible)                                                    */
/* BIT_TMP must be declared as: BIT BIT_TMP; in Common.c                                */
/*--------------------------------------------------------------------------------------*/
/* Interrupt-safe SFR page switching */
#define set_SFRPAGE              BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS=1;EA=BIT_TMP
#define clr_SFRPAGE              BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS=0;EA=BIT_TMP

/* PWM Control - LOAD bit triggers loading of new PWM values */
#define set_LOAD                 LOAD=1

/* Enable/disable all interrupts (EA bit) */
#define set_EA                   EA=1
#define clr_EA                   EA=0

/* Timer Control Shortcuts (without TCON_ prefix) */
#define set_TR0                  TR0=1
#define clr_TR0                  TR0=0
#define set_TR1                  TR1=1
#define clr_TR1                  TR1=0
#define set_TF0                  TF0=1
#define clr_TF0                  TF0=0
#define set_TF1                  TF1=1
#define clr_TF1                  TF1=0
#define set_IT0                  IT0=1
#define clr_IT0                  IT0=0
#define set_IT1                  IT1=1
#define clr_IT1                  IT1=0
#define set_IE0                  IE0=1
#define clr_IE0                  IE0=0
#define set_IE1                  IE1=1
#define clr_IE1                  IE1=0

/* External Interrupt Enable Shortcuts */
#define set_EX0                  EX0=1
#define clr_EX0                  EX0=0
#define set_EX1                  EX1=1
#define clr_EX1                  EX1=0

/* Timer Interrupt Enable Shortcuts */
#define set_ET0                  ET0=1
#define clr_ET0                  ET0=0
#define set_ET1                  ET1=1
#define clr_ET1                  ET1=0

/* Pin Interrupt Enable (EIE register) */
#define set_EPI                  EIE|=0x80
#define clr_EPI                  EIE&=0x7F

#endif /* __SFR_MACRO_H__ */
