
#include "local.h"
#include "serif.h"


static PrintCallback debug_print_cb = nullptr;
char  prbuff[MAX_PRINT_LEN];

void serif_set_debug_print_cb(PrintCallback cb)
{
	debug_print_cb = cb;
	Serial.println("Set debug_print");
}

void serif_debug_print(const char* msg)
{
    if (debug_print_cb)
        debug_print_cb(msg);
	else
	{
		//Serial.print("No debug_print defined: ");
		//Serial.println(msg);
	}
}	

void serif_initialize(void)
{
	debug_print_cb = nullptr;
}
