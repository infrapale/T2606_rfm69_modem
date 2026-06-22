#include "Arduino.h"
#include "local.h"
#include <RH_RF69.h>
#include "modem69.h"
#include "rfm69if.h"
#include "serif.h"



static uint32_t led_timeout_ms;
// static PrintCallback print_cb = nullptr;

static void led_on(uint32_t duration_ms)
{
    led_timeout_ms = millis() + duration_ms;

}


Modem69::Modem69(RH_RF69 *rf69p, uint8_t pin_rfm69_rst)
{
    _rf69p = rf69p;
    modem.pin_rfm69_rst   = pin_rfm69_rst;
}


void Modem69::initialize(uint8_t key[])
{
    pinMode(modem.pin_rfm69_rst, OUTPUT);
    rfm69_initialize(_rf69p, modem.pin_rfm69_rst, key);
	serif_debug_print("Modem69::initialize\n");
}

		 
void Modem69::set_debug_print(PrintCallback cb)
{
	//debug_print_cb = cb;
	serif_set_debug_print_cb(cb);
	//Serial.println("Modem69::set_debug_print\n");
}
		
void Modem69::modem_task(void){
	rfm69if_send_task();
	rfm69_receive_task();
    // rfm69_receive_message();
}

uint8_t Modem69::send_queue_avail(void) {
	return rfm69if_send_queue_avail();
}

uint8_t Modem69::add_to_send_queue(const char *buff)
{
	return rfm69if_enqueue_msg(buff);
}
void Modem69::radiate(char *buff)
{	
	serif_debug_print("Modem69::radiate: ");
	serif_debug_print(buff);
	serif_debug_print("\n");
    rfm69_radiate_msg(buff);
}


bool Modem69::msg_is_avail(void)
{
	return rfm69_receive_message_is_avail();
}

void Modem69::get_msg(char *buff, uint8_t max_len, bool clr_avail)
{
	rfm69_get_message(buff, max_len, clr_avail);
}


int16_t Modem69::get_last_rssi(void)
{
	return rfm69_get_last_rssi(); 
}
