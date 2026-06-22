#include <SPI.h>
#include "main.h"
#include "io.h"


// typedef struct
// {
//     uint8_t  pin;
//     uint16_t tick_cntr;
// } led_ctrl_st;

typedef struct
{
  uint8_t   pin;
  uint32_t  pattern;
  uint16_t  tick_nbr;
  bool      enable;
  bool      forever;
} led_st;

typedef struct
{
  uint8_t pattern_bit;
  uint8_t switches;
  uint8_t tindx;
} io_ctrl_st;

io_ctrl_st io_ctrl;

led_st led[LED_NBR_OF] = 
{
    [LED_INDX_RED]       = {PIN_LED_RED, 0, 0, false},
    [LED_INDX_GREEN]     = {PIN_LED_GREEN, 0, 0, false},
    [LED_INDX_BLUE]      = {PIN_LED_BLUE, 0, 0, false}
};



const uint32_t led_pattern[BLINK_NBR_OF] = 
{
    [BLINK_OFF]       = 0b00000000000000000000000000000000,
    [BLINK_ON]        = 0b11111111111111111111111111111111,
    [BLINK_1_FLASH]   = 0b00000000000000000000000000000001,
    [BLINK_2_FLASH]   = 0b10000000000000001000000000000000,
    [BLINK_4_FLASH]   = 0b10000001000000001000000010000000,
    [BLINK_SLOW]      = 0b11111111111111110000000000000000,
    [BLINK_NORMAL]    = 0b11110000111100001111000011110000,
    [BLINK_FAST]      = 0b11001100110011001100110011001100,
    [BLINK_SOS]       = 0b10010010011100111001110010010010,
    [BLINK_JITTER_1]  = 0b10101010101010101010101010101010,
    [BLINK_JITTER_2]  = 0b10010010010010010010010010010010,
    [BLINK_JITTER_3]  = 0b10001000100010001000100010001000,
};

// #define PIN_RM69_MISO   16
// #define PIN_RFM69_CS    17
// #define PIN_RM69_SCK    18
// #define PIN_RM69_MOSI   19
// #define PIN_RFM69_RST   20
// #define PIN_RFM69_INT   21
// #define RFM69_IRQN      0  // Pin 2 is IRQ 0!


void io_rfm69_spi0_initialize(void) 
{
    SPI.setRX(PIN_RM69_MISO);
    SPI.setTX(PIN_RM69_MOSI);
    SPI.setSCK(PIN_RM69_SCK);
    SPI.setCS(PIN_RFM69_CS);
    pinMode(PIN_RFM69_RST,OUTPUT);
    digitalWrite(PIN_RFM69_RST,HIGH);
    SPI.begin();

    SPI.beginTransaction(SPISettings(
        1000000,      // 8 MHz
        MSBFIRST,
        SPI_MODE0
    ));
}

void io_initialize(void)
{
    pinMode(PIN_LED_ONBOARD, OUTPUT);
    digitalWrite(PIN_LED_ONBOARD, LOW);
    io_ctrl.pattern_bit = 0;

    for (uint8_t i = LED_INDX_RED; i <= LED_INDX_BLUE; i++)
    {
        pinMode(led[i].pin, OUTPUT);
        digitalWrite(led[i].pin, LOW);
    } 

    io_led_flash(LED_INDX_RED, BLINK_SOS, 100);
    io_led_flash(LED_INDX_GREEN, BLINK_1_FLASH, 20);
    io_led_flash(LED_INDX_BLUE, BLINK_4_FLASH, 20);
}

void io_led_flash(led_indx_et lindx, blink_et bindx, uint16_t tick_nbr)
{
  led[lindx].pattern = led_pattern[bindx];
  led[lindx].tick_nbr = tick_nbr;
  if(tick_nbr == BLINK_DISABLE) led[lindx].enable = false;
  else led[lindx].enable = true;
  if(tick_nbr == BLINK_FOREVER) led[lindx].forever = true;
  else led[lindx].forever = false;
}

void io_task(void)
{
    uint32_t patt = 1UL << io_ctrl.pattern_bit;
    for (uint8_t i = LED_INDX_RED; i <= LED_INDX_BLUE; i++)
    {
        if (led[i].enable){
            if ((led[i].tick_nbr > 0) || led[i].forever) {
                if ((led[i].pattern & patt) != 0)
                    digitalWrite(led[i].pin, HIGH);
                else  
                    digitalWrite(led[i].pin, LOW);
            }
            if(!led[i].forever){
                if(led[i].tick_nbr == 0) digitalWrite(led[i].pin, LOW);
                else led[i].tick_nbr--;  
            }
        }
    } 
    if (++io_ctrl.pattern_bit >= 32) io_ctrl.pattern_bit = 0;
}
