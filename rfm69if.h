#ifndef __RFM69IF_H__
#define __RFM69IF_H__
#include "local.h"
#include <RH_RF69.h>

//*********************************************************************************************
// *********** IMPORTANT SETTINGS - YOU MUST CHANGE/ONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NETWORKID         100  //the same on all nodes that talk to each other
#define NODEID            10  
#define BROADCAST         255

#define RECEIVER          BROADCAST    // The recipient of packets
//Match frequency to the hardware version of the radio on your Feather
//#define FREQUENCY       RF69_433MHZ
//#define FREQUENCY       RF69_868MHZ
//#define FREQUENCY       RF69_915MHZ
//#define ENCRYPTKEY        "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HCW       true // set to 'true' if you are using an RFM69HCW module

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ     434.0  //915.0

typedef void (*led_on_function)(uint32_t duration_ms);

void rfm69_initialize(RH_RF69 *rf69_p, uint8_t pin_rst, uint8_t key[]);



typedef struct
{
    char            radio_msg[MAX_MESSAGE_LEN];
    uint8_t         len;
    bool            avail;
    int8_t          rssi;
	uint8_t			state;
} rfm_receive_msg_st;

/// @brief  Get pointer to module data
/// @note
/// @param
/// @return pointer
rfm_receive_msg_st *rfm69_get_receive_data_ptr(void);

/// @brief  Receive message when available
/// @note   save message in radio_msg
/// @param  -
/// @return -
void rfm69_receive_message(void);

/// @brief  Check if a radio message is available
/// @note
/// @param
/// @return
bool rfm69_receive_message_is_avail(void);

uint8_t rfm69if_send_queue_avail(void);

uint8_t rfm69if_enqueue_msg(const char *text);

static const char* rfm69if_dequeue_msg(void);

void rfm69if_send_task(void);

void rfm69_receive_task(void);

void rfm69_get_message(char *buff, uint8_t max_len, bool clr_avail);

void rfm69_clr_receive_message_flag(void);

int16_t rfm69_get_last_rssi(void);

typedef struct
{
    char            radio_msg[MAX_MESSAGE_LEN];
} rfm_send_msg_st;


/// @brief  Get pointer to module data 
/// @note   
/// @return pointer to data
rfm_send_msg_st *rfm69_get_send_data_ptr(void);

/// @brief  Send message
/// @param  message to send
/// @return
void rfm69_radiate_msg( const char *radio_msg );





#endif
