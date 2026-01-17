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
#define ADDR_ENDO_MAX       0x1600
#define ADDR_SCR            0x2000

/*===========================================================================*/
/* DWIN Frame Processing - Ring Buffer for UART RX                            */
/*===========================================================================*/
#define RX_BUFFER_SIZE      32      /* Ring buffer size (power of 2) */
#define RX_BUFFER_MASK      (RX_BUFFER_SIZE - 1)
#define FRAME_TIMEOUT_MS    50      /* Max time to receive complete frame */
#define MAX_FRAME_LEN       12      /* Maximum expected DWIN frame length */

#define DWIN_HEADER_H       0x5A
#define DWIN_HEADER_L       0xA5
#define DWIN_CMD_WRITE      0x82
#define DWIN_CMD_READ_RESP  0x83

/* DWIN frame states */
#define FRAME_IDLE          0
#define FRAME_GOT_5A        1
#define FRAME_GOT_A5        2
#define FRAME_GOT_LEN       3
#define FRAME_RECEIVING     4

/* Ring buffer for UART RX - use XDATA to save DATA space */
static volatile uint8_t xdata rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;
static volatile uint8_t rx_overflow_cnt = 0;  /* Overflow counter for diagnostics */

/* Frame parsing state - use XDATA */
static uint8_t frame_state = FRAME_IDLE;
static uint8_t xdata frame_buffer[MAX_FRAME_LEN];
static uint8_t frame_idx = 0;
static uint8_t frame_len = 0;
static uint16_t frame_timeout_cnt = 0;        /* Timeout counter */

/* Parsed frame data */
static volatile bit g_frame_ready = 0;
static volatile uint16_t g_frame_addr = 0;
static volatile uint16_t g_frame_value = 0;

/* Communication statistics */
static uint16_t g_frame_rx_count = 0;         /* Successful frames received */
static uint8_t g_frame_error_count = 0;       /* Frame errors (timeout/invalid) */

/*===========================================================================*/
/* PWM Configuration                                                          */
/*===========================================================================*/
#define PWM_PERIOD          0x0456

static uint16_t code pwm_lut[11] = {
    0, 99, 199, 299, 399, 499, 599, 699, 799, 899, 999
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
static bit g_power = 0;
static uint8_t g_brightness = 7;
static uint8_t g_cct = 3;
static uint8_t g_prev_scr = 0;
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
static void reset_frame_parser(void);
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
        next_head = (rx_head + 1) & RX_BUFFER_MASK;
        
        if (next_head != rx_tail)
        {
            rx_buffer[rx_head] = SBUF;
            rx_head = next_head;
        }
        else
        {
            /* Buffer overflow - track for diagnostics */
            rx_overflow_cnt++;
        }
        RI = 0;
    }
    
    if (TI)
    {
        TI = 0;
    }
}

/*===========================================================================*/
/* Ring Buffer Helper Functions                                               */
/*===========================================================================*/
static uint8_t rx_available(void)
{
    return (rx_head != rx_tail);
}

static uint8_t rx_read(void)
{
    uint8_t d;
    if (rx_head == rx_tail) return 0;
    d = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) & RX_BUFFER_MASK;
    return d;
}

/*===========================================================================*/
/* DWIN Frame Parser with Timeout Protection                                  */
/*===========================================================================*/
static void reset_frame_parser(void)
{
    frame_state = FRAME_IDLE;
    frame_idx = 0;
    frame_len = 0;
    frame_timeout_cnt = 0;
}

static void process_DWIN_Frames(void)
{
    uint8_t byte;
    uint8_t had_data = 0;
    
    /* Timeout check - reset parser if stuck mid-frame */
    if (frame_state != FRAME_IDLE)
    {
        frame_timeout_cnt++;
        if (frame_timeout_cnt > 5000)  /* ~50ms at typical main loop speed */
        {
            reset_frame_parser();
            if (g_frame_error_count < 255) g_frame_error_count++;
        }
    }
    
    while (rx_available())
    {
        byte = rx_read();
        had_data = 1;
        frame_timeout_cnt = 0;  /* Reset timeout on data */
        
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
                    /* Possible new frame start - restart */
                    frame_idx = 0;
                    frame_buffer[frame_idx++] = byte;
                }
                else
                {
                    reset_frame_parser();
                }
                break;
                
            case FRAME_GOT_A5:
                frame_len = byte;
                frame_buffer[frame_idx++] = byte;
                /* Validate length: min 3 (cmd + addr), max 9 for our use */
                if (frame_len >= 3 && frame_len <= 9)
                {
                    frame_state = FRAME_RECEIVING;
                }
                else
                {
                    reset_frame_parser();
                    if (g_frame_error_count < 255) g_frame_error_count++;
                }
                break;
                
            case FRAME_RECEIVING:
                /* Buffer overflow protection */
                if (frame_idx >= MAX_FRAME_LEN)
                {
                    reset_frame_parser();
                    if (g_frame_error_count < 255) g_frame_error_count++;
                    break;
                }
                
                frame_buffer[frame_idx++] = byte;
                if (frame_idx >= (3 + frame_len))
                {
                    /* Complete frame received */
                    uint8_t cmd = frame_buffer[3];
                    if (cmd == DWIN_CMD_READ_RESP || cmd == DWIN_CMD_WRITE)
                    {
                        g_frame_addr = ((uint16_t)frame_buffer[4] << 8) | frame_buffer[5];
                        /* For auto-upload: 5A A5 Len Cmd AddrH AddrL Count DataH DataL */
                        if (frame_len >= 5)
                        {
                            g_frame_value = ((uint16_t)frame_buffer[7] << 8) | frame_buffer[8];
                            g_frame_ready = 1;
                            g_frame_rx_count++;
                        }
                    }
                    reset_frame_parser();
                }
                break;
                
            default:
                reset_frame_parser();
                break;
        }
    }
    
    if (g_frame_ready)
    {
        handle_DWIN_VP(g_frame_addr, g_frame_value);
        g_frame_ready = 0;
    }
}

/*===========================================================================*/
/* DWIN VP Handler                                                            */
/*===========================================================================*/
static void handle_DWIN_VP(uint16_t address, uint16_t value)
{
    switch (address)
    {
        case ADDR_POWER:
            if (value && !g_power)
            {
                g_power = 1;
                g_brightness = 7;
                g_cct = 3;
                update_PWM();
                sync_Display();
            }
            else if (!value && g_power)
            {
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
                writeVP(ADDR_MEMONE, 0);
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
            
        case ADDR_ENDO_MAX:
            if (g_power && value == 1)  /* Endo mode */
            {
                g_brightness = 1;
                g_cct = 1;
                update_PWM();
                writeVP(ADDR_ENDO_MAX, 0);
                sync_Display();
            }
            else if (g_power && value == 2)  /* Max mode */
            {
                g_brightness = MAX_BRIGHTNESS;
                g_cct = MAX_CCT;
                update_PWM();
                writeVP(ADDR_ENDO_MAX, 0);
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
/* DWIN Communication - TX Only                                               */
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
        return;
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
    
    Timer3_Delay10us(100);
}

/*===========================================================================*/
/* Initialization Functions                                                   */
/*===========================================================================*/
static void GPIO_Init(void)
{
    Set_All_GPIO_Quasi_Mode;
    
    P06_Quasi_Mode;
    P07_Input_Mode;
    
    P04_PushPull_Mode;
    BuzzerPin = 0;
    
    P14_PushPull_Mode;
    P15_PushPull_Mode;
    
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

static void UART_Init(void)
{
    MODIFY_HIRC_24576();
    InitialUART0_Timer1(115200);
    
    ENABLE_UART0_INTERRUPT;   /* Enable Serial interrupt */
    ENABLE_GLOBAL_INTERRUPT;   /* Enable global interrupts */
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
/* MAIN Function                                                              */
/*===========================================================================*/
void main(void)
{
    GPIO_Init();
    Beep();
    UART_Init();
    PWM_Init();
    
    writeVP(ADDR_MEMONE, 0);
    writeVP(ADDR_MEMTWO, 0);
    writeVP(ADDR_ENDO_MAX, 0);
    
    update_PWM();
    
    while (1)
    {
        process_IR();        process_DWIN_Frames();
    }
}   