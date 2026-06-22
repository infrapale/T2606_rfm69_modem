#include <Arduino.h>
#include <Wire.h>
#include <RH_RF69.h>
#include <SPI.h>

#include "local.h"
#include "rfm69if.h"
#include "serif.h"

RH_RF69 *rf69p;

#define SEND_BUF_SIZE 8
#define MSG_MAX_LEN   60

typedef struct {
    char msg[SEND_BUF_SIZE][MSG_MAX_LEN];
    uint8_t head;     // next message to send
    uint8_t tail;     // next free slot
    uint8_t count;    // how many messages are queued
} send_buffer_t;

static send_buffer_t send_buf = {0};

rfm_receive_msg_st  receive_msg = {0};



extern char  prbuff[];

void rfm69_initialize(RH_RF69 *rf69_p, uint8_t pin_rst, uint8_t key[])
{
    rf69p = rf69_p;
    pinMode(pin_rst, OUTPUT);
    digitalWrite(pin_rst, HIGH);    	delay(100);
    digitalWrite(pin_rst, LOW);    delay(100);

	serif_debug_print("RFM69 Initialize:\n");
    if (!rf69p->init()) {
		serif_debug_print("RFM69 radio init failed\n");
		while (1){ 
			serif_debug_print("@rfm#");
			delay(5000);
		};
    }
    serif_debug_print("RFM69 radio init OK!");
    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
    // No encryption
    if (!rf69p->setFrequency(RF69_FREQ)) {
        serif_debug_print("RF69 setFrequency failed!!\n");
    }
    // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
    // ishighpowermodule flag set like this:
    rf69p->setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
 
    //uint8_t key[] = RFM69_KEY; //exactly the same 16 characters/bytes on all nodes!   
    rf69p->setEncryptionKey(key);
  
	sprintf(prbuff,"RFM69 radio @ %d MHz\n",(int)RF69_FREQ);
    // serif_debug_print(("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(F(" MHz"));   

    // Initialize Receive 
    receive_msg.avail = false;

}

uint8_t rfm69if_send_queue_avail(void)
{
	return (SEND_BUF_SIZE - send_buf.count);
}

uint8_t rfm69if_enqueue_msg(const char *text)
{
    if (send_buf.count >= SEND_BUF_SIZE)
        return 0; // full

    strncpy(send_buf.msg[send_buf.tail], text, MSG_MAX_LEN - 1);
    send_buf.msg[send_buf.tail][MSG_MAX_LEN - 1] = '\0';

    send_buf.tail = (send_buf.tail + 1) % SEND_BUF_SIZE;
    send_buf.count++;

    return 1;
}

static const char* rfm69if_dequeue_msg(void)
{
    if (send_buf.count == 0)
        return NULL;

    const char *m = send_buf.msg[send_buf.head];
    send_buf.head = (send_buf.head + 1) % SEND_BUF_SIZE;
    send_buf.count--;

    return m;
}

void rfm69if_send_task(void)
{
    static uint32_t wait_until = 0;
	static uint8_t	state= 0;


	switch(state)
	{
		case 0:
			state = 10;
			break;
		case 10:
			if (send_buf.count > 0)
			{
				const char *msg = rfm69if_dequeue_msg();
				if (msg) 
				{
					rfm69_radiate_msg( msg );
					wait_until = millis() + 1500;
					state = 20;
				}
			}
			break;
		case 20:
			if(millis() > wait_until)state = 10;
			break;
		case 30:
			state = 10;
			break;
		case 40:
			state = 10;
			break;
	}
}


int16_t rfm69_get_last_rssi(void)
{
    return (receive_msg.rssi);
}

//*****************   Receive   *****************************************

void rfm69_receive_message(void)
{
    if (rf69p->available()) 
    {
        receive_msg.len = sizeof(receive_msg.radio_msg);
        if (rf69p->recv((uint8_t*)receive_msg.radio_msg, &receive_msg.len)) 
        {
            receive_msg.avail = true;
            if (receive_msg.len > 0)
            {   
                if (receive_msg.len >= MAX_MESSAGE_LEN) receive_msg.len = MAX_MESSAGE_LEN -1;
                receive_msg.radio_msg[receive_msg.len] = 0;
				sprintf(prbuff,"Received [%d] %s len=%d RSSI= %d\n",
					receive_msg.len,
					(char*)receive_msg.radio_msg,
					receive_msg.len,
					rf69p->lastRssi()
				);
                //Serial.print(F("Received [)"));Serial.print(receive_msg.len);Serial.print("]: ");
                //Serial.println((char*)receive_msg.radio_msg);               
                //Serial.print(F("len: "));
                //Serial.print(receive_msg.len, DEC);
                //Serial.print(F("  RSSI: "));
                //Serial.println(rf69p->lastRssi(), DEC);
                receive_msg.rssi = rf69p->lastRssi();
            }
        }
    }
}

void rfm69_get_message(char *buff, uint8_t max_len, bool clr_avail)
{
	buff[0] = 0x00;
	rfm69_receive_message();
	if(receive_msg.avail)
	{
		strncpy(buff,(char*)receive_msg.radio_msg, max_len);
		if (clr_avail) receive_msg.avail = false;
	}	
}	



bool rfm69_receive_message_is_avail(void)
{
    return  receive_msg.avail;
}

void rfm69_clr_receive_message_flag(void)
{
    receive_msg.avail = false;
}

void rfm69_receive_task(void)
{
	switch(receive_msg.state)
	{
		case 0:
			receive_msg.avail = false;
			receive_msg.state = 10;
			break;
		case 10:
			if(!receive_msg.avail){
				rfm69_receive_message();
				// if(receive_msg.avail) receive_msg.state = 20; 
			}
			break;
		case 20:
			receive_msg.state = 10;
			break;
	}
}

//*****************   Send   *****************************************
void rfm69_radiate_msg( const char *radio_msg )
{
    //Serial.print("rfm69_radiate_msg: "); Serial.println(radio_msg); 
    if (radio_msg[0] != 0)
    {
		sprintf(prbuff,"Radiate: %s\n", radio_msg);
        //Serial.println(radio_msg);
        //rf69p->waitPacketSent();
        rf69p->send((uint8_t *)radio_msg, strlen(radio_msg));      
    }
}



