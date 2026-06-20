#ifndef __SERIF_H__
#define __SERIF_H__
#include <Arduino.h>
#include "local.h"
//#include "rfm69.h"

#define serif_MAX_BLOCK_LEN  8
#define serif_MAX_REPLY_LEN  80

typedef enum
{
    serif_FRAME_START        = '<',
    serif_FRAME_END          = '>',
    serif_FRAME_DUMMY        = '*'
} serif_frame_et;
/*
typedef enum
{
    serif_FRAME_POS_START        = 0,
    serif_FRAME_POS_TO_TAG,
    serif_FRAME_POS_TO_ADDR,
    serif_FRAME_POS_FROM_TAG,
    serif_FRAME_POS_FROM_ADDR,
    serif_FRAME_POS_FUNC,
    serif_FRAME_POS_INDEX,
    serif_FRAME_POS_ACTION,
    serif_FRAME_POS_DATA,
    serif_FRAME_POS_END,
} serif_frame_pos_et;
*/

/*
typedef enum
{
    serif_CMD_TRANSMIT_RAW   = 'T',
    serif_CMD_TRANSMIT_NODE  = 'J',
    serif_CMD_GET_AVAIL      = 'A',
    serif_CMD_READ_RAW       = 'R',
    serif_CMD_READ_NODE      = 'D', 
    serif_CMD_GET_RSSI       = 'I',
    serif_CMD_SET_PARAM      = 'S',
} serif_cmd_et;

typedef enum
{
    serif_REPLY_AVAILABLE    = 'a',
    serif_REPLY_READ_RAW     = 'r',
    serif_REPLY_READ_NODE    = 'o' 
} serif_reply_et;
*/
/*
typedef enum
{
    serif_ACTION_GET    = '?',
    serif_ACTION_SET    = '=',
    serif_ACTION_REPLY  = ':' 
} serif_action_et;
*/
/*
typedef struct 
{
    char            tag;
    char            addr;     
    char            from_tag;
    char            from_addr; 
    char            function;
    char            index;   
    char            action;
}   msg_frame_st;

*/
/*
typedef struct
{
    char            msg[MAX_MESSAGE_LEN];
    uint8_t         len;
    bool            avail;
    msg_frame_st    frame;
    serif_cmd_et     cmd;
    msg_format_et   format;
    msg_status_et   status;
} serif_rx_st;
*/

/*
typedef struct
{
    char            msg[serif_MAX_REPLY_LEN];
    uint8_t         len;
    bool            avail;
    char            module;
    char            addr;         
    serif_cmd_et     cmd;
    msg_format_et   format;
    msg_format_et   cmd_format;
    msg_status_et   status;
} serif_tx_st;
*/
/*
typedef struct
{
    String zone;
    String name; 
    String value;
    String remark;
} serif_node_st;
*/

/*
typedef struct
{
    serif_rx_st      rx;
    serif_tx_st      tx;
    serif_node_st    node;
	char 			mod_tag;
	char 			mod_addr;
} serif_msg_st;
*/

/// @brief Clear rx available
/// @param  -
/// @return -
void serif_initialize(void);

void serif_set_debug_print_cb(PrintCallback cb);

void serif_debug_print(const char* msg);

#endif