#include <htc.h>
#include <string.h>
#include <stdio.h>

// CONFIG
__CONFIG(FOSC_XT & WDTE_OFF & PWRTE_ON & CP_OFF & BOREN_ON & LVP_OFF);

#define _XTAL_FREQ 4000000

// LCD Definitions
#define E    RA0
#define RS   RA1
#define RW   RA2

// Button Definitions
#define BTN_START_STOP RD5
#define BTN_MODE  RD3
#define BTN_RESET RD4
#define BTN_INC   RD7
#define BTN_DEC   RD6

// Output Definitions
#define LED    RB0
#define BUZZER RB1

// Global Variables
static unsigned int time;           // In seconds
static unsigned char mode;          // 0 = Stopwatch, 1 = Timer
static unsigned char running;       // Operating state
static unsigned char halfSecCount;  // Counter to generate 1 second intervals

// LCD Prototypes
void ToggleEpinOfLCD(void);
unsigned char ReadBusyFlagLCD(void);
void CheckBusyLCD(void);
void WriteCommandToLCD(unsigned char);
void ClearLCD(void);
void InitLCD(void);
void WriteDataToLCD(char);
void WriteStringToLCD(const char *);
void SetPosLCD(unsigned char);

// New Function Prototypes
void StartTimer1(void);
void StopTimer1(void);
void displayTime(unsigned int);
void displayState(void);
void checkButtons(void);

// Interrupt
void interrupt ISR(void)
{
    if (TMR1IF)
    {
        TMR1IF = 0;
        halfSecCount++;
        
        if (halfSecCount >= 2)
        {
            halfSecCount = 0;
            
            if (running)
            {
                if (mode == 0)
                {
                    if (time < 9999)
                        time++;
                }
                else if (time > 0)
                {
                    time--;
                    if (time == 0)
                    {
                        LED = 1;
                        BUZZER = 1;
                    }
                }
            }
        }
    }
}

void StartTimer1(void)
{
    TMR1H = 0;
    TMR1L = 0;
    T1CON = 0b00110001;  // Timer1 ON, prescaler 1:8
    TMR1IF = 0;
    TMR1IE = 1;
    PEIE = 1;
    GIE = 1;
}

void StopTimer1(void)
{
    TMR1ON = 0;
    TMR1IE = 0;
}

void displayTime(unsigned int t)
{
    char buffer[8];
    unsigned char min = t / 60;
    unsigned char sec = t % 60;
    sprintf(buffer, "%02u:%02u", min, sec);
    WriteStringToLCD(buffer);
}

void displayState(void)
{
    SetPosLCD(0x00);
    if (mode == 0)
    {
        WriteStringToLCD("STOPWATCH    ");
    }
    else
    {
        WriteStringToLCD("TIMER        ");
    }
    
    SetPosLCD(0x40);
    WriteStringToLCD("    ");
    displayTime(time);
    WriteStringToLCD("    ");
}

void checkButtons(void)
{	
    static bit oldStartStop = 0;
    static bit oldMode = 0;
    static bit oldReset = 0;
    static bit oldInc = 0;
    static bit oldDec = 0;

    // MODE Button
    if (BTN_MODE && !oldMode)
    {
        mode = !mode;
        if (running)
        {
            running = 0;
            StopTimer1();
        }
    }
    oldMode = BTN_MODE;
    
    // START/STOP Button
    if (BTN_START_STOP == 0 && oldStartStop == 0)
    {
        running = !running;
        if (running)
        {
            LED = 0;
            BUZZER = 0;
            StartTimer1();
        }
        else
        {
            StopTimer1();
        }
    }
    oldStartStop = BTN_START_STOP;
    
    // RESET Button
    if (BTN_RESET && !oldReset)
    {
        time = 0;
        LED = 0;
        BUZZER = 0;
    }
    oldReset = BTN_RESET;
    
    // INC/DEC Buttons (only in timer mode and stopped)
    if (mode == 1 && !running)
    {
        if (BTN_INC && !oldInc)
        {
            if (time <= 5939)  // Limit 99:59
                time += 1;
        }
        
        if (BTN_DEC && !oldDec)
        {
            if (time >= 1)
                time -= 1;
        }
    }
    oldInc = BTN_INC;
    oldDec = BTN_DEC;
}

void main(void)
{
    // Initial port configuration
    TRISA = 0b11111000;  // RA0-RA2 outputs (LCD)
    TRISB = 0b11110000;  // RB4-RB7 inputs (LCD)
    TRISD = 0b11111111;  // RD3-RD7 inputs (buttons)
    
    ADCON1 = 0b00000110;  // Configure pins as digital
    
    // Variable initialization
    time = 0;
    mode = 0;
    running = 0;
    halfSecCount = 0;
    
    // Peripheral initialization
    InitLCD();
    ClearLCD();
    
    LED = 0;
    BUZZER = 0;
    
    // Main loop
    while (1)
    {
        checkButtons();
        displayState();
        __delay_ms(50);
    }
}

// LCD Functions
void ToggleEpinOfLCD(void)
{
    E = 1;                	
    __delay_us(500);      
    E = 0;             		
    __delay_us(500); 	
}

unsigned char ReadBusyFlagLCD(void)
{
    unsigned char data;
    TRISB = 0xF0;         // Set upper 4 bits of PORTB as input
    RS = 0;
    RW = 1;
    E = 1;
    __delay_us(5);
    data = (PORTB & 0xF0);
    E = 0;
    __delay_us(5);
    E = 1;
    __delay_us(5);
    data |= ((PORTB >> 4) & 0x0F);
    E = 0; 
    RW = 0;
    __delay_us(40);
    TRISB = 0x00;
    return data;
}

void CheckBusyLCD(void)
{
    while ((ReadBusyFlagLCD() & 0x80) == 0x80)
    {
        // Wait while LCD is busy
    }
}

void WriteCommandToLCD(unsigned char Command)
{
    RS = 0;                      // Command mode
    RW = 0;                      // Write mode
    PORTB &= 0x0F;               // Clear high nibble    
    PORTB |= (Command & 0xF0);     // Send high nibble
    ToggleEpinOfLCD();           // Pulse E
    PORTB &= 0x0F;               // Clear high nibble
    PORTB |= ((Command << 4) & 0xF0); // Send low nibble
    ToggleEpinOfLCD();           // Pulse E
}

void WriteDataToLCD(char LCDChar)
{
    CheckBusyLCD();
    RS = 1;                      // Data mode
    RW = 0;                      // Write mode
    PORTB &= 0x0F;               // Clear high nibble
    PORTB |= (LCDChar & 0xF0);     // Send high nibble
    ToggleEpinOfLCD();           // Pulse E
    PORTB &= 0x0F;               // Clear high nibble
    PORTB |= ((LCDChar << 4) & 0xF0); // Send low nibble
    ToggleEpinOfLCD();           // Pulse E
    RS = 0;
}

void InitLCD(void)
{
    TRISB = 0x00;               // RB4-RB7 as outputs for data
    __delay_ms(40);             // Wait >30ms after VDD
    
    // 4-bit mode initialization
    PORTB &= 0x0F;
    PORTB |= 0x30;              // Command 0x3
    ToggleEpinOfLCD();
    __delay_ms(5);              // Wait >4.1ms
    
    ToggleEpinOfLCD();
    __delay_us(150);            // Wait >100us
    
    ToggleEpinOfLCD();
    __delay_us(50);
    
    PORTB &= 0x0F;
    PORTB |= 0x20;              // Command 0x2 - 4-bit mode
    ToggleEpinOfLCD();
    __delay_us(50);
    
    // LCD configuration
    WriteCommandToLCD(0x28);     // 4 bits, 2 lines, 5x7
    __delay_us(50);
    WriteCommandToLCD(0x0C);     // Display ON, cursor off
    __delay_us(50);
    WriteCommandToLCD(0x01);     // Clear display
    __delay_ms(2);
    WriteCommandToLCD(0x06);     // Increment cursor
    __delay_us(50);
}

void ClearLCD(void)
{
    CheckBusyLCD();
    WriteCommandToLCD(0x01);    // Clear display command
    __delay_ms(2);             // Wait for completion
}

void SetPosLCD(unsigned char pos)
{
    CheckBusyLCD();
    WriteCommandToLCD(0x80 | pos);  // Set DDRAM address command
}

void WriteStringToLCD(const char *str)
{
    while (*str)
    {
        WriteDataToLCD(*str++);
    }
}
