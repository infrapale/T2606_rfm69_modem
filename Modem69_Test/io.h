#ifndef __IO_H__
#define __IO_H__


#define RFM69_CS      17
#define RFM69_INT     21
#define RFM69_IRQN    0  // Pin 2 is IRQ 0!
#define RFM69_RST     20

// LED Definitions
#define PIN_LED_ONBOARD 13  // onboard blinky
#define LED_NBR_OF      4
#define PIN_LED_RED     10
#define PIN_LED_GREEN   11
#define PIN_LED_BLUE    12

typedef enum
{
    LED_INDX_ONBOARD = 0,
    LED_INDX_RED,
    LED_INDX_GREEN,
    LED_INDX_BLUE,
    LED_INDX_NBR_OF
} led_index_et;

void io_initialize(void);

void io_led_flash(led_index_et led_indx, uint16_t nbr_ticks );

void io_run_100ms(void);


#endif