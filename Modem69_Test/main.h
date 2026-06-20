#ifndef __MAIN_H__
#define __MAIN_H__
#include <Arduino.h>
#include "WString.h"
#define   __APP__ ((char*)"T2606_Modem69_Test")

#define DEBUG_PRINT 

typedef struct
{
    uint32_t next_io_tick;
    uint32_t next_comm_tick;
} main_ctrl_st;

#define LED_INDICATION

#define MY_MODULE_TAG   'R'
#define MY_MODULE_ADDR  '1'

typedef struct
{
    char            tag;
    char            addr;         
} modem_data_st;




#endif