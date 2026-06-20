#ifndef __LOCAL_H__
#define __LOCAL_H__


#undef      MODEM_DEBUG_PRINT
#define 	MAX_MESSAGE_LEN   	61
#define		MAX_PRINT_LEN		80

//typedef void (*led_on_callback(uint32_t duration_ms));

typedef void (*PrintCallback)(const char* msg);

typedef enum
{
    STATUS_UNDEFINED = 0,
    STATUS_OK_FOR_ME,
    STATUS_NOT_FOR_ME,
    STATUS_UNKNOWN_COMMAND,
    STATUS_CORRECT_FRAME,
    STATUS_INCORRECT_FRAME,
} msg_status_et;


#endif