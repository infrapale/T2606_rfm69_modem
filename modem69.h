#ifndef __MODEM69_H__
#define __MODEM69_H__

#include "local.h"
#include <RH_RF69.h>

//typedef void (*PrintCallback)(const char* msg);

typedef struct
{
    uint8_t 	pin_rfm69_rst;
} rf_modem_st;

static void led_on(uint32_t duration_ms);


class Modem69
{
    private:
        rf_modem_st modem;

    public:
		RH_RF69   *_rf69p;
		Modem69(RH_RF69 *rf69p, uint8_t pin_rfm69_rst);

        void initialize(uint8_t key[]);
		
		void set_debug_print(PrintCallback cb);

        void modem_task(void);

        void radiate(char *buff);
				
		bool msg_is_avail(void);
		
		void get_msg(char *buff, uint8_t max_len, bool clr_avail);
		
		int16_t get_last_rssi(void);	

};

#endif