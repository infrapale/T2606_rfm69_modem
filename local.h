#ifndef __LOCAL_H__
#define __LOCAL_H__


#undef      MODEM_DEBUG_PRINT
//#define     MODEM_DEBUG_PRINT

//typedef void (*led_on_callback(uint32_t duration_ms));

typedef void (*PrintCallback)(const char* msg);

typedef enum
{
    MSG_FORMAT_RAW = 0,
    MSG_FORMAT_SENSOR_JSON,
    MSG_FORMAT_RELAY_JSON
}  msg_format_et;

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