/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/******************************************************************************
 * @file     main.c
 * @brief    Touch Controller with IR Remote for MS51FB9AE
 * @version  3.0.0
 * @date     2025-01-17
 * @note     Ported from N76E003 to MS51FB9AE @ 24MHz HIRC
 *           
 * Architecture:
 *   - UART RX Interrupt for DWIN auto-upload frame processing
 *   - Ring buffer for incoming DWIN frames
 *   - Non-blocking frame parser
 *   - IR interrupt for remote control
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
/* DWIN Frame Processing - Ring Buffer for UART RX                            */
/*===========================================================================*/
#define RX_BUFFER_SIZE      32      /* Ring buffer size (power of 2 for efficiency) */
#define RX_BUFFER_MASK      (RX_BUFFER_SIZE - 1)

#define DWIN_HEADER_H       0x5A
#define DWIN_HEADER_L       0xA5
#define DWIN_CMD_WRITE      0x82    /* Write VP command */
#define DWIN_CMD_READ_RESP  0x83    /* Read VP response */

/* DWIN frame states */
#define FRAME_IDLE          0
#define FRAME_GOT_5A        1
#define FRAME_GOT_A5        2
#define FRAME_GOT_LEN       3
#define FRAME_RECEIVING     4

/* Ring buffer for UART RX */
static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;    /* Write index (ISR writes here) */
static volatile uint8_t rx_tail = 0;    /* Read index (main loop reads here) */

/* Frame parsing state */
static uint8_t frame_state = FRAME_IDLE;
static uint8_t frame_buffer[12];        /* Max frame: 5A A5 len cmd addr[2] count data[4] */
static uint8_t frame_idx = 0;
static uint8_t frame_len = 0;

/* Parsed frame data (set when complete frame received) */
static volatile bit g_frame_ready = 0;
static volatile uint16_t g_frame_addr = 0;
static volatile uint16_t g_frame_value = 0;

/*===========================================================================*/
/* PWM Configuration                                                          */
/*===========================================================================*/
#define PWM_PERIOD          0x0456  /* PWM period value (~2.7kHz @ 24MHz/8) */

/* PWM lookup table - indexed by brightness level (0-10) */
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
/* State Variables                                                            */
/*===========================================================================*/
static bit g_power = 0;                 /* Power state (0=off, 1=on) */
static uint8_t g_brightness = 7;        /* Current brightness (0-10) */
static uint8_t g_cct = 3;               /* Current CCT level (0-10) */
static uint8_t g_prev_scr = 0;          /* Previous screen status */

/* IR data buffer */
static uint8_t ir_data[IR_DATA_LEN];

/*===========================================================================*/
/* Function Prototypes                                                        */
/*===========================================================================*/
static void GPIO_Init(void);
static void UART_Init(void);
static void MODIFY_HIRC_24576(void);
static void PWM_Init(void);
static void Beep(void);
static void update_PWM(void);
static void sync_Display(void);
static void writeVP(uint16_t address, uint16_t value);
static void setPage(uint8_t page);
static void writeScr(uint8_t value);
static void process_IR(void);
static void process_DWIN_Frames(void);
static void handle_DWIN_VP(uint16_t address, uint16_t value);

/*===========================================================================*/
/* UART0 Receive Interrupt Service Routine                                    */
/*===========================================================================*/
void UART0_ISR(void) interrupt 4
{
    if (RI)
    {
        uint8_t next_head;
        
        /* Store received byte in ring buffer */
        next_head = (rx_head + 1) & RX_BUFFER_MASK;
        
        /* Check for buffer overflow */
        if (next_head != rx_tail)
        {
            rx_buffer[rx_head] = SBUF;
            rx_head = next_head;
        }
        /* else: buffer full, discard byte */
        
        RI = 0;  /* Clear receive interrupt flag */
    }
    
    if (TI)
    {
        TI = 0;  /* Clear transmit interrupt flag (handled in Send_Data) */
    }
}

/*===========================================================================*/
/* Ring Buffer Helper Functions                                               */
/*===========================================================================*/

/**
 * @brief Check if data available in RX buffer
 * @return 1 if data available, 0 if empty
 */
static uint8_t rx_available(void)
{
    return (rx_head != rx_tail);
}

/**
 * @brief Read one byte from RX buffer (non-blocking)
 * @return Byte read, or 0 if empty
 */
static uint8_t rx_read(void)
{
    uint8_t data;
    
    if (rx_head == rx_tail) return 0;  /* Empty */
    
    data = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) & RX_BUFFER_MASK;
    
    return data;
}

/*===========================================================================*/
/* DWIN Frame Parser - State Machine                                          */
/*===========================================================================*/

/**
 * @brief Process incoming bytes and parse DWIN frames
 * @note Called from main loop, processes all available bytes
 * 
 * DWIN Auto-upload frame format:
 *   5A A5 [len] [cmd] [addr_h] [addr_l] [count] [data_h] [data_l]
 *   
 * Example: Touch slider at 0x1100, value 5
 *   5A A5 06 83 11 00 01 00 05
 */
static void process_DWIN_Frames(void)
{
    uint8_t byte;
    
    while (rx_available())
    {
        byte = rx_read();
        
        switch (frame_state)
        {
            case FRAME_IDLE:
                if (byte == DWIN_HEADER_H)
                {
                    frame_state = FRAME_GOT_5A;
                    frame_idx = 0;
                    frame_buffer[frame_idx++] = byte;
                }
                break;
                
            case FRAME_GOT_5A:
                if (byte == DWIN_HEADER_L)
                {
                    frame_state = FRAME_GOT_A5;
                    frame_buffer[frame_idx++] = byte;
                }
                else if (byte == DWIN_HEADER_H)
                {
                    /* Stay in GOT_5A state, might be new frame */
                    frame_idx = 0;
                    frame_buffer[frame_idx++] = byte;
                }
                else
                {
                    frame_state = FRAME_IDLE;  /* Invalid, reset */
                }
                break;
                
            case FRAME_GOT_A5:
                /* This byte is the length */
                frame_len = byte;
                frame_buffer[frame_idx++] = byte;
                
                if (frame_len > 0 && frame_len <= 9)  /* Valid length range */
                {
                    frame_state = FRAME_RECEIVING;
                }
                else
                {
                    frame_state = FRAME_IDLE;  /* Invalid length */
                }
                break;
                
            case FRAME_RECEIVING:
                frame_buffer[frame_idx++] = byte;
                
                /* Check if we have received all data bytes */
                /* frame_len bytes after the length byte itself */
                if (frame_idx >= (3 + frame_len))  /* 3 = header(2) + len(1) */
                {
                    /* Complete frame received! */
                    uint8_t cmd = frame_buffer[3];
                    
                    if (cmd == DWIN_CMD_READ_RESP || cmd == DWIN_CMD_WRITE)
                    {
                        /* Extract address and value */
                        /* Format: [cmd] [addr_h] [addr_l] [count] [data_h] [data_l] */
                        g_frame_addr = ((uint16_t)frame_buffer[4] << 8) | frame_buffer[5];
                        
                        /* Data starts at index 7 (after count byte at index 6) */
                        if (frame_len >= 5)  /* At least cmd+addr+count+data */
                        {
                            g_frame_value = ((uint16_t)frame_buffer[7] << 8) | frame_buffer[8];
                            g_frame_ready = 1;
                        }
                    }
                    
                    frame_state = FRAME_IDLE;  /* Ready for next frame */
                }
                break;
                
            default:
                frame_state = FRAME_IDLE;
                break;
        }
    }
    
    /* Process completed frame if available */
    if (g_frame_ready)
    {
        handle_DWIN_VP(g_frame_addr, g_frame_value);
        g_frame_ready = 0;
    }
}

/*===========================================================================*/
/* DWIN VP Handler - Process auto-uploaded values                             */
/*===========================================================================*/

/**
 * @brief Handle received VP value from DWIN display
 * @param address VP address
 * @param value VP value
 */
static void handle_DWIN_VP(uint16_t address, uint16_t value)
{
    switch (address)
    {
        case ADDR_POWER:
            if (value && !g_power)
            {
                /* Power ON via touch */
                g_power = 1;
                g_brightness = 7;
                g_cct = 3;
                update_PWM();
                sync_Display();
            }
            else if (!value && g_power)
            {
                /* Power OFF via touch */
                g_power = 0;
                update_PWM();
            }
            break;
            
        case ADDR_BRIGHT:
            if (g_power && value <= MAX_BRIGHTNESS && value != g_brightness)
            {
                g_brightness = (uint8_t)value;
                update_PWM();
            }
            break;
            
        case ADDR_CCT:
            if (g_power && value <= MAX_CCT && value != g_cct)
            {
                g_cct = (uint8_t)value;
                update_PWM();
            }
            break;
            
        case ADDR_MEMONE:
            if (g_power && value == 1)
            {
                g_brightness = 6;
                g_cct = 4;
                update_PWM();
                writeVP(ADDR_MEMONE, 0);  /* Clear flag */
                sync_Display();
            }
            break;
            
        case ADDR_MEMTWO:
            if (g_power && value == 1)
            {
                g_brightness = 4;
                g_cct = 7;
                update_PWM();
                writeVP(ADDR_MEMTWO, 0);
                sync_Display();
            }
            break;
            
        case ADDR_ENDO:
            if (g_power && value == 1)
            {
                g_brightness = 1;
                g_cct = 1;
                update_PWM();
                writeVP(ADDR_ENDO, 0);
                sync_Display();
            }
            break;
            
        case ADDR_MAX:
            if (g_power && value == 2)
            {
                g_brightness = MAX_BRIGHTNESS;
                g_cct = MAX_CCT;
                update_PWM();
                writeVP(ADDR_MAX, 0);
                sync_Display();
            }
            break;
            
        case ADDR_SCR:
            if (value && value != g_prev_scr)
            {
                writeScr((uint8_t)(value * 10));
                g_prev_scr = (uint8_t)value;
            }
            else if (value == 0)
            {
                g_prev_scr = 0;
            }
            break;
            
        default:
            /* Unknown address, ignore */
            break;
    }
}

/*===========================================================================*/
/* PWM Control Functions                                                      */
/*===========================================================================*/

static void set_White_Fast(uint8_t level)
{
    uint16_t pwm_val;
    
    level = (level > MAX_BRIGHTNESS) ? MAX_BRIGHTNESS : level;
    pwm_val = pwm_lut[level];
    
    PWM1L = (uint8_t)(pwm_val);
    PWM1H = (uint8_t)(pwm_val >> 8);
    set_LOAD;
}

static void set_Yellow_Fast(uint8_t level)
{
    uint16_t pwm_val;
    
    level = (level > MAX_CCT) ? MAX_CCT : level;
    pwm_val = pwm_lut[level];
    
    set_SFRPAGE;
    PWM5L = (uint8_t)(pwm_val);
    PWM5H = (uint8_t)(pwm_val >> 8);
    clr_SFRPAGE;
    set_LOAD;
}

static void update_PWM(void)
{
    if (g_power)
    {
        set_White_Fast(g_brightness);
        set_Yellow_Fast(g_cct);
    }
    else
    {
        PWM1L = 0; PWM1H = 0;
        set_SFRPAGE;
        PWM5L = 0; PWM5H = 0;
        clr_SFRPAGE;
        set_LOAD;
    }
}

static void sync_Display(void)
{
    writeVP(ADDR_BRIGHT, g_brightness);
    writeVP(ADDR_CCT, g_cct);
}

/*===========================================================================*/
/* DWIN Communication - TX Only (RX is interrupt-driven)                      */
/*===========================================================================*/

static void writeVP(uint16_t address, uint16_t value)
{
    Send_Data_To_UART0(0x5A);
    Send_Data_To_UART0(0xA5);
    Send_Data_To_UART0(0x05);
    Send_Data_To_UART0(0x82);
    Send_Data_To_UART0((uint8_t)(address >> 8));
    Send_Data_To_UART0((uint8_t)(address));
    Send_Data_To_UART0((uint8_t)(value >> 8));
    Send_Data_To_UART0((uint8_t)(value));
}

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
    
    Timer3_Delay10us(500);
}

static void writeScr(uint8_t value)
{
    Send_Data_To_UART0(0x5A);
    Send_Data_To_UART0(0xA5);
    Send_Data_To_UART0(0x04);
    Send_Data_To_UART0(0x82);
    Send_Data_To_UART0(0x00);
    Send_Data_To_UART0(0x82);
    Send_Data_To_UART0(value);
    
    Timer3_Delay10us(10000);
}

/*===========================================================================*/
/* IR Command Processing                                                      */
/*===========================================================================*/

static void process_IR(void)
{
    uint8_t cmd, inv;
    
    if (!is_ir_data_received()) return;
    
    get_ir_data(ir_data, IR_DATA_LEN);
    
    cmd = ir_data[2];
    inv = ir_data[3];
    
    /* Validate NEC protocol */
    if ((cmd ^ inv) != 0xFF) return;
    
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
        sync_Display();
    }
    else if (!g_power)
    {
        return;  /* Ignore other commands when power off */
    }
    else if (cmd == IR_CMD_WHITE_UP)
    {
        if (g_brightness < MAX_BRIGHTNESS)
        {
            g_brightness++;
            update_PWM();
            sync_Display();
        }
    }
    else if (cmd == IR_CMD_WHITE_DN)
    {
        if (g_brightness > 0)
        {
            g_brightness--;
            update_PWM();
            sync_Display();
        }
    }
    else if (cmd == IR_CMD_CCT)
    {
        g_cct = (g_cct >= MAX_CCT) ? 0 : (g_cct + 1);
        update_PWM();
        sync_Display();
    }
    else if (cmd == IR_CMD_ENDO)
    {
        g_brightness = 1;
        g_cct = 1;
        update_PWM();
        sync_Display();
    }
    else if (cmd == IR_CMD_MEM1)
    {
        g_brightness = 6;
        g_cct = 4;
        update_PWM();
        sync_Display();
    }
    else if (cmd == IR_CMD_MAX)
    {
        g_brightness = MAX_BRIGHTNESS;
        g_cct = MAX_CCT;
        update_PWM();
        sync_Display();
    }
    else if (cmd == IR_CMD_MEM2)
    {
        g_brightness = 4;
        g_cct = 7;
        update_PWM();
        sync_Display();
    }
    
    Timer3_Delay10us(100);  /* Debounce */
}

/*==========================================================