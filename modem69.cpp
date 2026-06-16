#include "Arduino.h"
#include "local.h"
#include <RH_RF69.h>
#include "modem69.h"
#include "rfm69if.h"



static uint32_t led_timeout_ms;
static PrintCallback print_cb = nullptr;

static void led_on(uint32_t duration_ms)
{
    led_timeout_ms = millis() + duration_ms;

}


Modem69::Modem69(RH_RF69 *rf69p, uint8_t pin_rfm69_rst)
{
    _rf69p = rf69p;
    modem.pin_rfm69_rst   = pin_rfm69_rst;
	modem.enable_uart_tx  = true;
 	modem.enable_uart_rx  = true;
}


void Modem69::initialize(uint8_t key[])
{
	//modem.tag             = mod_tag;
	//modem.addr            = mod_addr;
    pinMode(modem.pin_rfm69_rst, OUTPUT);
    rfm69_initialize(_rf69p, modem.pin_rfm69_rst, key);
}

void Modem69::set_debug_print(PrintCallback cb)
{
	//debug_print_cb = cb;
	rfm69_set_print_debug_cb(cb);
}
		
void Modem69::enable_uart(bool enable_tx, bool enable_rx)
{
	modem.enable_uart_tx  = enable_tx;
	modem.enable_uart_rx  = enable_rx;
} 

//void Modem69::set_serial(Stream& s)
//{
//	uart_set_serial(s);
//} 

void Modem69::modem_task(void){
    //if(modem.enable_uart_rx) uart_rx_task();
	rfm69_receive_task();
    // rfm69_receive_message();
}


void Modem69::radiate(char *buff)
{
	
    rfm69_radiate_msg(buff);
}

//void Modem69::radiate_node_json(char *buff)
//{
//	uart_radiate_node_json(buff);
//}

void Modem69::radiate_node_json(char *buff)
{
	char b[MAX_MESSAGE_LEN] = {0};
	b[0] = '<';
	b[1] = 'X';
	b[2] = '1';
	b[3] = modem.tag;
	b[4] = modem.addr;
	b[5] = 'J';
	b[6] = '1';
	b[7] = '=';
	strncpy(&b[8], buff, MAX_MESSAGE_LEN-8);
	uint8_t len = strlen(b);
	b[len] = '>';
	b[len+1] = 0x00;
	
	uart_radiate_node_json(b);
}


bool Modem69::msg_is_avail(void)
{
	return rfm69_receive_message_is_avail();
}

void Modem69::get_msg(char *buff, uint8_t max_len, bool clr_avail)
{
	rfm69_get_message(buff, max_len, clr_avail);
}

void Modem69::get_msg_decode(char *buff, uint8_t max_len, bool clr_avail)
{
	uart_get_decoded_msg(buff, max_len, clr_avail);
	
}

int16_t Modem69::get_last_rssi(void)
{
	return rfm69_get_last_rssi(); 
}
