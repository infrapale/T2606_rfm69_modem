#ifndef __IO_H__
#define __IO_H__


#define PIN_RM69_MISO   16
#define PIN_RFM69_CS    17
#define PIN_RM69_SCK    18
#define PIN_RM69_MOSI   19
#define PIN_RFM69_RST   20
#define PIN_RFM69_INT   21
#define RFM69_IRQN      0  // Pin 2 is IRQ 0!

// LED Definitions
#define PIN_LED_ONBOARD 13  // onboard blinky
#define LED_NBR_OF      4
#define PIN_LED_RED     10
#define PIN_LED_GREEN   11
#define PIN_LED_BLUE    12

#define BLINK_DISABLE  (9998)
#define BLINK_FOREVER  (9999)

typedef enum
{
  BLINK_OFF = 0,
  BLINK_ON,
  BLINK_1_FLASH,
  BLINK_2_FLASH,
  BLINK_4_FLASH,
  BLINK_SLOW,
  BLINK_NORMAL,
  BLINK_FAST,
  BLINK_SOS,
  BLINK_JITTER_1,
  BLINK_JITTER_2,
  BLINK_JITTER_3,
  BLINK_NBR_OF
} blink_et;

typedef enum
{
    LED_INDX_RED = 0,
    LED_INDX_GREEN,
    LED_INDX_BLUE,
    LED_INDX_NBR_OF
} led_indx_et;

void io_initialize(void);

void io_rfm69_spi0_initialize(void) ;

void io_task_initialize(void);

void io_led_flash(led_indx_et lindx, blink_et bindx, uint16_t tick_nbr);

void io_task(void);



#endif