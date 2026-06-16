#ifndef __serif_H__
#define __serif_H__
#include "local.h"
#include "rfm69.h"

#define serif_MAX_BLOCK_LEN  8
#define serif_MAX_REPLY_LEN  80

typedef enum
{
    serif_FRAME_START        = '<',
    serif_FRAME_END          = '>',
    serif_FRAME_DUMMY        = '*'
} serif_frame_et;

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

typedef enum
{
    serif_ACTION_GET    = '?',
    serif_ACTION_SET    = '=',
    serif_ACTION_REPLY  = ':' 
} serif_action_et;

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

typedef struct
{
    String zone;
    String name; 
    String value;
    String remark;
} serif_node_st;

typedef struct
{
    serif_rx_st      rx;
    serif_tx_st      tx;
    serif_node_st    node;
	char 			mod_tag;
	char 			mod_addr;
} serif_msg_st;

/// @brief Clear rx available
/// @param  -
/// @return -
void serif_initialize(char mod_tag, char mod_addr);

void serif_set_serial(Stream& s);


/// @brief  Get pointer to module data
/// @param
/// @return data pointer
serif_msg_st *serif_get_data_ptr(void);

/// @brief  Read uart
/// @note   Save mesage in uart.rx.str
/// @param  -
/// @return -
void serif_read_uart(void);

/// @brief  Parse Rx frame, 
/// @note   check that the frame is valid and addressed to me
/// @param  -
/// @return -
void serif_parse_rx_frame(void);

/// @brief  Print rx metadat for debugging
/// @param  -
/// @return -
void serif_print_rx_metadata(void);

/// @brief  Build radio mesasge
/// @param  -
/// @return -
void serif_rx_build_rfm_array(void);

/// @brief  Execute command
/// @param  UART command
/// @return
void serif_exec_cmnd(serif_cmd_et ucmd);

void serif_radiate_node_json(char *buff);

void serif_copy_tx( char *buff, uint8_t max_len);

void serif_get_decoded_msg( char *buff, uint8_t max_len, bool clr_avail);


void serif_rx_task(void);

#endif