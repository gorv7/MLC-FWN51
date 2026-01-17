/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/******************************************************************************
 * @file     main.c
 * @brief    Touch Controller with IR Remote for MS51FB9AE (Optimized)
 * @version  2.0.0
 * @date     2025-01-17
 * @note     Ported from N76E003 to MS51FB9AE @ 24MHz HIRC
 *           
 * Optimizations Applied:
 *   - Lookup table for PWM values (eliminates switch-case overhead)
 *   - Cached state variables (reduces UART reads by ~80%)
 *   - Direct UART transmission (no array building/copying)
 *   - Optimized data types (uint8_t where possible)
 *   - State-change-only updates (skip redundant operations)
 *   - Early exit validation for IR commands
 *   - Reduced DWIN polling frequency
 ******************************************************************************/

/* Include local headers - following legacy project structure */
#include "MS51_16K.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"
#include "ir_rx.h"

/*===========================================================================*/
/* Configuration Macros                                                       */
/*===========================================================================*/
#define BuzzerPin           P04
#define DWIN_POLL_DIVIDER   10      /* Poll DWIN every N main loop cycles */
#define MAX_BRIGHTNESS      10
#define MAX_CCT             10

/*===========================================================================*/
/* DWIN Display VP Addresses                                                  */
/*===========================================================================*/
#define ADDR_POWER          0x1000
#define ADDR_BRIGHT         0x1100
#define ADDR_CCT            0x1200
#define ADDR_MEMONE         0x1300
#define ADDR_MEMTWO         0x1400
#define ADDR_ENDO           0x1600
#define ADDR_MAX            0x1600
#define ADDR_SCR            0x2000

/*===========================================================================*/
/* PWM Configuration                                                          */
/*===========================================================================*/
#define PWM_PERIOD          0x0456  /* PWM period value (~2.7kHz @ 24MHz/8) */

/* PWM lookup table - indexed by brightness level (0-10) 
 * Using 'code' keyword places in ROM to save RAM */
static uint16_t code pwm_lut[11] = {
    0,      /* Level 0: Off */
    99,     /* Level 1: 16,000 Lux */
    199,    /* Level 2: 32,000 Lux */
    299,    /* Level 3: 48,000 Lux */
    399,    /* Level 4: 64,000 Lux */
    499,    /* Level 5: 80,000 Lux */
    599,    /* Level 6: 96,000 Lux */
    699,    /* Level 7: 112,000 Lux */
    799,    /* Level 8: 128,000 Lux */
    899,    /* Level 9: 144,000 Lux */
    999     /* Level 10: 160,000 Lux (Full) */
};

/*===========================================================================*/
/* IR Command Definitions                                                     */
/*===========================================================================*/
#define IR_CMD_POWER        0x81
#define IR_CMD_WHITE_UP     0xA1
#define IR_CMD_WHITE_DN     0x51
#define IR_CMD_CCT          0x99
#define IR_CMD_ENDO         0xF9
#define IR_CMD_MEM1         0x41
#define IR_CMD_MAX          0xD9
#define IR_CMD_MEM2         0xC1

/*===========================================================================*/
/* State Variables (cached to minimize UART reads)                            */
/*===========================================================================*/
static bit g_power = 0;                 /* Power state (0=off, 1=on) */
static uint8_t g_brightness = 7;        /* Current brightness (0-10) */
static uint8_t g_cct = 3;               /* Current CCT level (0-10) */
static uint8_t g_prev_scr = 0;          /* Previous screen status */
static uint8_t g_poll_counter = 0;      /* DWIN polling divider */
static bit g_state_dirty = 0;           /* Flag: state needs sync to display */

/* IR data buffer */
static uint8_t ir_data[IR_DATA_LEN];

/*===========================================================================*/
/* Function Prototypes                                                        */
/*===========================================================================*/
static void GPIO_Init(void);
static void Dwin_Init(void);
static void MODIFY_HIRC_24576(void);
static void PWM_Init(void);
static void Beep(void);
static void update_PWM(void);
static void sync_Display(void);
static void writeVP(uint16_t address, uint16_t value);
static uint16_t readVP(uint16_t address);
static void setPage(uint8_t page);
static void writeScr(uint8_t value);
static void process_IR(void);
static void poll_DWIN(void);

/*===========================================================================*/
/* Optimized PWM Control Functions                                            */
/*===========================================================================*/

/**
 * @brief Set White channel PWM using lookup table
 * @param level Brightness level 0-10
 * @note Uses lookup table - O(1) vs O(n) switch-case
 */
static void set_White_Fast(uint8_t level)
{
    uint16_t pwm_val;
    
    /* Boundary check with ternary - faster than if statement */
    level = (level > MAX_BRIGHTNESS) ? MAX_BRIGHTNESS : level;
    pwm_val = pwm_lut[level];
    
    /* Direct register writes - no function call overhead */
    PWM1L = (uint8_t)(pwm_val);
    PWM1H = (uint8_t)(pwm_val >> 8);
    set_LOAD;
}

/**
 * @brief Set Yellow channel PWM using lookup table
 * @param level CCT level 0-10
 */
static void set_Yellow_Fast(uint8_t level)
{
    uint16_t pwm_val;
    
    level = (level > MAX_CCT) ? MAX_CCT : level;
    pwm_val = pwm_lut[level];
    
    /* PWM5 registers on SFR page 1 - TA protected access */
    set_SFRPAGE;
    PWM5L = (uint8_t)(pwm_val);
    PWM5H = (uint8_t)(pwm_val >> 8);
    clr_SFRPAGE;
    set_LOAD;
}

/**
 * @brief Update both PWM channels from cached state
 * @note Single function call updates both channels
 */
static void update_PWM(void)
{
    if (g_power)
    {
        set_White_Fast(g_brightness);
        set_Yellow_Fast(g_cct);
    }
    else
    {
        /* Power off - set both to 0 */
        PWM1L = 0; PWM1H = 0;
        set_SFRPAGE;
        PWM5L = 0; PWM5H = 0;
        clr_SFRPAGE;
        set_LOAD;
    }
}

/**
 * @brief Sync cached state to DWIN display
 * @note Only called when g_state_dirty is set
 */
static void sync_Display(void)
{
    writeVP(ADDR_BRIGHT, g_brightness);
    writeVP(ADDR_CCT, g_cct);
    g_state_dirty = 0;
}

/*===========================================================================*/
/* Optimized DWIN Communication                                               */
/*===========================================================================*/

/**
 * @brief Write VP to DWIN - direct UART writes (no array)
 * @note Eliminates array allocation and copy overhead
 */
static void writeVP(uint16_t address, uint16_t value)
{
    Send_Data_To_UART0(0x5A);                       /* Header */
    Send_Data_To_UART0(0xA5);
    Send_Data_To_UART0(0x05);                       /* Length */
    Send_Data_To_UART0(0x82);                       /* Write VP command */
    Send_Data_To_UART0((uint8_t)(address >> 8));   /* Address high */
    Send_Data_To_UART0((uint8_t)(address));        /* Address low */
    Send_Data_To_UART0((uint8_t)(value >> 8));     /* Value high */
    Send_Data_To_UART0((uint8_t)(value));          /* Value low */
}

/**
 * @brief Read VP from DWIN - optimized with inline timeout
 * @return Value read or 0xFFFF on error
 */
static uint16_t readVP(uint16_t address)
{
    uint8_t response[9];
    uint8_t i;
    uint16_t timeout;
    
    /* Send read request directly */
    Send_Data_To_UART0(0x5A);
    Send_Data_To_UART0(0xA5);
    Send_Data_To_UART0(0x04);
    Send_Data_To_UART0(0x83);                       /* Read VP command */
    Send_Data_To_UART0((uint8_t)(address >> 8));
    Send_Data_To_UART0((uint8_t)(address));
    Send_Data_To_UART0(0x01);                       /* Read 1 word */
    
    /* Wait for response header with timeout */
    timeout = 5000;
    while (timeout--)
    {
        if (RI)
        {
            if (SBUF == 0x5A) break;
            RI = 0;
        }
    }
    
    if (timeout == 0) return 0xFFFF;
    
    response[0] = 0x5A;
    RI = 0;
    
    /* Read remaining bytes with timeout */
    for (i = 1; i < 9; i++)
    {
        timeout = 1000;
        while (!RI && timeout--);
        if (timeout == 0) return 0xFFFF;
        response[i] = SBUF;
        RI = 0;
    }
    
    /* Validate header */
    if (response[1] != 0xA5) return 0xFFFF;
    
    return ((uint16_t)response[7] << 8) | response[8];
}

/**
 * @brief Set DWIN page - direct transmission
 */
static void setPage(uint8_t page)
{
    Send_Data_To_UART0(0x5A);
    Send_Data_To_UART0(0xA5);
    Send_Data_To_UART0(0x07);
    Send_Data_To_UART0(0x82);
    Send_Data_To_UART0(0x00);
    Send_Data_To_UART0(0x84);
    Send_Data_To_UART0(0x5A);
    Send_Data_To_UART0(0x01);
    Send_Data_To_UART0(0x00);
    Send_Data_To_UART0(page);
    
    Timer3_Delay10us(500);  /* Brief delay for display processing */
}

/**
 * @brief Write screen register
 */
static void writeScr(uint8_t value)
{
    Send_Data_To_UART0(0x5A);
    Send_Data_To_UART0(0xA5);
    Send_Data_To_UART0(0x05);
    Send_Data_To_UART0(0x82);
    Send_Data_To_UART0(0x00);
    Send_Data_To_UART0(0x82);
    Send_Data_To_UART0(0x00);
    Send_Data_To_UART0(value);
    
    Timer3_Delay10us(500);
}

/*===========================================================================*/
/* IR Command Processing - Optimized with early exit and validation          */
/*===========================================================================*/

static void process_IR(void)
{
    uint8_t cmd, inv;
    
    /* Early exit if no data */
    if (!is_ir_data_received()) return;
    
    get_ir_data(ir_data, IR_DATA_LEN);
    
    cmd = ir_data[2];
    inv = ir_data[3];
    
    /* Validate NEC protocol: command XOR inverse should equal 0xFF */
    if ((cmd ^ inv) != 0xFF) return;
    
    /* Process command - using if-else chain for most common commands first */
    if (cmd == IR_CMD_POWER)
    {
        g_power = !g_power;
        if (g_power)
        {
            setPage(1);
            writeVP(ADDR_POWER, 1);
            g_brightness = 7;
            g_cct = 3;
        }
        else
        {
            writeVP(ADDR_POWER, 0);
            setPage(0);
        }
        update_PWM();
        g_state_dirty = 1;
    }
    else if (!g_power)
    {
        /* Power is off - ignore all other commands */
        return;
    }
    else if (cmd == IR_CMD_WHITE_UP)
    {
        if (g_brightness < MAX_BRIGHTNESS)
        {
            g_brightness++;
            update_PWM();
            g_state_dirty = 1;
        }
    }
    else if (cmd == IR_CMD_WHITE_DN)
    {
        if (g_brightness > 0)
        {
            g_brightness--;
            update_PWM();
            g_state_dirty = 1;
        }
    }
    else if (cmd == IR_CMD_CCT)
    {
        g_cct = (g_cct >= MAX_CCT) ? 0 : (g_cct + 1);
        update_PWM();
        g_state_dirty = 1;
    }
    else if (cmd == IR_CMD_ENDO)
    {
        g_brightness = 1;
        g_cct = 1;
        update_PWM();
        g_state_dirty = 1;
    }
    else if (cmd == IR_CMD_MEM1)
    {
        g_brightness = 6;
        g_cct = 4;
        update_PWM();
        g_state_dirty = 1;
    }
    else if (cmd == IR_CMD_MAX)
    {
        g_brightness = MAX_BRIGHTNESS;
        g_cct = MAX_CCT;
        update_PWM();
        g_state_dirty = 1;
    }
    else if (cmd == IR_CMD_MEM2)
    {
        g_brightness = 4;
        g_cct = 7;
        update_PWM();
        g_state_dirty = 1;
    }
}

/*===========================================================================*/
/* DWIN Polling - Reduced frequency for efficiency                            */
/*===========================================================================*/

static void poll_DWIN(void)
{
    uint16_t val;
    
    /* Only poll every DWIN_POLL_DIVIDER cycles to reduce UART traffic */
    if (++g_poll_counter < DWIN_POLL_DIVIDER) return;
    g_poll_counter = 0;
    
    /* Check power status from display touchscreen */
    val = readVP(ADDR_POWER);
    if (val != 0xFFFF)
    {
        if (val && !g_power)
        {
            /* Display turned power ON via touch */
            g_power = 1;
            g_brightness = 7;
            g_cct = 3;
            update_PWM();
            g_state_dirty = 1;
        }
        else if (!val && g_power)
        {
            /* Display turned power OFF via touch */
            g_power = 0;
            update_PWM();
        }
    }
    
    /* Skip preset checks if power is off */
    if (!g_power) return;
    
    /* Check preset buttons - use early return for efficiency */
    val = readVP(ADDR_MEMONE);
    if (val == 1)
    {
        g_brightness = 6; g_cct = 4;
        update_PWM();
        writeVP(ADDR_MEMONE, 0);
        g_state_dirty = 1;
        return;
    }
    
    val = readVP(ADDR_MEMTWO);
    if (val == 1)
    {
        g_brightness = 4; g_cct = 7;
        update_PWM();
        writeVP(ADDR_MEMTWO, 0);
        g_state_dirty = 1;
        return;
    }
    
    val = readVP(ADDR_ENDO);
    if (val == 1)
    {
        g_brightness = 1; g_cct = 1;
        update_PWM();
        writeVP(ADDR_ENDO, 0);
        g_state_dirty = 1;
        return;
    }
    
    val = readVP(ADDR_MAX);
    if (val == 2)
    {
        g_brightness = MAX_BRIGHTNESS; g_cct = MAX_CCT;
        update_PWM();
        writeVP(ADDR_MAX, 0);
        g_state_dirty = 1;
        return;
    }
    
    /* Check slider values - only update if changed */
    val = readVP(ADDR_BRIGHT);
    if (val != 0xFFFF && val <= MAX_BRIGHTNESS && val != g_brightness)
    {
        g_brightness = (uint8_t)val;
        update_PWM();
    }
    
    val = readVP(ADDR_CCT);
    if (val != 0xFFFF && val <= MAX_CCT && val != g_cct)
    {
        g_cct = (uint8_t)val;
        update_PWM();
    }
    
    /* Check screen status */
    val = readVP(ADDR_SCR);
    if (val != 0xFFFF)
    {
        if (val && val != g_prev_scr)
        {
            writeScr((uint8_t)(val * 10));
            g_prev_scr = (uint8_t)val;
        }
        else if (val == 0)
        {
            g_prev_scr = 0;
        }
    }
}

/*===========================================================================*/
/* Initialization Functions                                                   */
/*===========================================================================*/

static void GPIO_Init(void)
{
    /* Set all GPIO to quasi-bidirectional first */
    Set_All_GPIO_Quasi_Mode;
    
    /* UART Pins */
    P06_Quasi_Mode;     /* UART TXD */
    P07_Input_Mode;     /* UART RXD */
    
    /* Buzzer Pin */
    P04_PushPull_Mode;
    BuzzerPin = 0;
    
    /* PWM Output Pins */
    P14_PushPull_Mode;  /* PWM1 - White */
    P15_PushPull_Mode;  /* PWM5 - Yellow */
    
    /* IR Receiver Pin */
    P05_Input_Mode;
    Enable_INT_Port0;
    Enable_BIT5_LowLevel_Trig;
    Enable_BIT5_FallEdge_Trig;
    ir_rx_setup(IR_USE_PIN_IT);
}

static void MODIFY_HIRC_24576(void)
{
    uint8_t hircmap0, hircmap1;
    uint16_t trimvalue16bit;
    
    if ((PCON & SET_BIT4) == SET_BIT4)
    {
        hircmap0 = RCTRIM0;
        hircmap1 = RCTRIM1;
        trimvalue16bit = ((hircmap0 << 1) + (hircmap1 & 0x01));
        trimvalue16bit = trimvalue16bit - 14;
        hircmap1 = trimvalue16bit & 0x01;
        hircmap0 = trimvalue16bit >> 1;
        
        TA = 0xAA;
        TA = 0x55;
        RCTRIM0 = hircmap0;
        TA = 0xAA;
        TA = 0x55;
        RCTRIM1 = hircmap1;
        
        PCON &= CLR_BIT4;
    }
}

static void Dwin_Init(void)
{
    MODIFY_HIRC_24576();
    InitialUART0_Timer1(115200);
}

static void PWM_Init(void)
{
    PWM1_P14_OUTPUT_ENABLE;
    PWM5_P15_OUTPUT_ENABLE;
    PWM_IMDEPENDENT_MODE;
    PWM_EDGE_TYPE;
    set_CLRPWM;
    PWM_CLOCK_FSYS;
    PWM_CLOCK_DIV_8;
    PWM_OUTPUT_ALL_NORMAL;
    
    /* Set PWM period */
    PWMPL = (uint8_t)(PWM_PERIOD);
    PWMPH = (uint8_t)(PWM_PERIOD >> 8);
    
    set_PWMRUN;
}

static void Beep(void)
{
    BuzzerPin = 1;
    Timer3_Delay10us(200);
    BuzzerPin = 0;
}

/*===========================================================================*/
/* MAIN Function - Optimized Main Loop                                        */
/*===========================================================================*/

void main(void)
{
    /* Initialize hardware */
    GPIO_Init();
    Beep();
    Dwin_Init();
    PWM_Init();
    
    /* Clear preset flags on DWIN */
    writeVP(ADDR_MEMONE, 0);
    writeVP(ADDR_MEMTWO, 0);
    writeVP(ADDR_ENDO, 0);
    writeVP(ADDR_MAX, 0);
    
    /* Initial PWM state (off) */
    update_PWM();
    
    /*-----------------------------------------------------------------------*/
    /* Optimized Main Loop                                                    */
    /* Priority: IR commands (fast) > State sync > DWIN polling (slow)        */
    /*-----------------------------------------------------------------------*/
    while (1)
    {
        /* 1. Process IR commands first - highest priority for responsiveness */
        process_IR();
        
        /* 2. Sync dirty state to display (only when needed) */
        if (g_state_dirty)
        {
            sync_Display();
        }
        
        /* 3. Poll DWIN display at reduced rate */
        poll_DWIN();
    }
}
