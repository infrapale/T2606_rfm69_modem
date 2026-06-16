#include "local.h"

#include "uart.h"
#include "rfm69.h"

#define Serial69  Serial1

serif_msg_st         uart;
//static Stream* serif_serial = nullptr; 

PrintCallback debug_print_cb nullptr;


void serif_set_debug_print(PrintCallback cb)
{
	debug_print_cb = cb;
}

static void serif_debug_print(const char* msg)
{
    if (debug_print_cb)
        debug_print_cb(msg);
}	

void serif_rx_task(void);


serif_msg_st *serif_get_data_ptr(void)
{
    return &uart;
}

void serif_initialize(char mod_tag, char mod_addr)
{
    uart.rx.avail = false;
	uart.mod_tag = mod_tag;
	uart.mod_addr = mod_addr;
	debug_print_cb = nullptr;
}

void serif_set_serial(Stream& s)
{
    // serif_serial = &s;
}

void serif_read_uart(void)
{
	// if(serif_serial != nullptr)
	{
		if (Serial69.available())
		{
			String Str;
			Str  = Serial69.readStringUntil('\n');
			#ifdef MODEM_DEBUG_PRINT
			Serial.println(Str);
			#endif  
			if (Str.length()> 0)
			{
				Str.trim();
				uart.rx.len = Str.length();
				Str.toCharArray(uart.rx.msg, MAX_MESSAGE_LEN);
				uart.rx.avail = true;
				//uart.rx.str.remove(uart.rx.str.length()-1);
			}
			#ifdef MODEM_DEBUG_PRINT
			Serial.print("rx is available: "); Serial.println(uart.rx.msg);
			#endif        
		} 
	}
}

void serif_parse_rx_frame(void)
{
    //rfm_send_msg_st *rx_msg = &send_msg; 
    bool do_continue = true;
    uint8_t len;

    if (uart.rx.len < serif_FRAME_POS_END)do_continue = false;
    if (do_continue){
        uart.rx.frame.tag       = uart.rx.msg[serif_FRAME_POS_TO_TAG];
        uart.rx.frame.addr      = uart.rx.msg[serif_FRAME_POS_TO_ADDR];
        uart.rx.frame.from_tag  = uart.rx.msg[serif_FRAME_POS_FROM_TAG];
        uart.rx.frame.from_addr = uart.rx.msg[serif_FRAME_POS_FROM_ADDR];
        uart.rx.frame.function  = uart.rx.msg[serif_FRAME_POS_FUNC];
        uart.rx.frame.index     = uart.rx.msg[serif_FRAME_POS_INDEX];
        uart.rx.frame.action    = uart.rx.msg[serif_FRAME_POS_ACTION];
    }

    if ((uart.rx.msg[0] != '<') || 
        (uart.rx.frame.tag != uart.mod_tag) || 
        (uart.rx.frame.addr  != uart.mod_addr) || 
        (uart.rx.msg[uart.rx.len-1] != '>'))  do_continue = false;

    if (do_continue)
    {   
        #ifdef MODEM_DEBUG_PRINT
        Serial.print("Buffer frame is OK\n");
        #endif

        uart.rx.status = STATUS_CORRECT_FRAME;
        if (uart.rx.frame.function == serif_CMD_TRANSMIT_RAW)  
            uart.rx.format = MSG_FORMAT_SENSOR_JSON;
        else uart.rx.format = MSG_FORMAT_RAW;       
    }
    else uart.rx.status = STATUS_INCORRECT_FRAME;
}

void serif_prepare_reply(void)
{
    uart.tx.msg[serif_FRAME_POS_START] = serif_FRAME_START;
    uart.tx.msg[serif_FRAME_POS_TO_TAG] = uart.rx.frame.from_tag; 
    uart.tx.msg[serif_FRAME_POS_TO_ADDR] = uart.rx.frame.from_addr;
    uart.tx.msg[serif_FRAME_POS_FROM_TAG] = uart.rx.frame.tag; 
    uart.tx.msg[serif_FRAME_POS_FROM_ADDR] = uart.rx.frame.addr;
    uart.tx.msg[serif_FRAME_POS_FUNC] = uart.rx.frame.function;
    uart.tx.msg[serif_FRAME_POS_INDEX] = uart.rx.frame.index;
    uart.tx.msg[serif_FRAME_POS_ACTION] = serif_ACTION_REPLY;
    uart.tx.msg[serif_FRAME_POS_DATA] = serif_FRAME_DUMMY;
    uart.tx.msg[serif_FRAME_POS_END] = serif_FRAME_END;
    uart.tx.msg[serif_FRAME_POS_END+1] = 0x00;
}

void serif_build_node_from_rx_str(void)
{
    uint8_t indx1;
    uint8_t indx2;
    String Str =uart.rx.msg;
    Str = Str.substring(serif_FRAME_POS_DATA, uart.rx.len-1);
    //Serial.print("serif_build_node_from_rx_str: ");Serial.print(uart.rx.len); Serial.println(Str);
    indx1 = 0;  //uart.rx.str.indexOf(':')
    indx2 = Str.indexOf(';');
    uart.node.zone = Str.substring(indx1,indx2);
    indx1 = indx2+1;
    indx2 = Str.indexOf(';',indx1+1);
    uart.node.name = Str.substring(indx1,indx2);
    indx1 = indx2+1;
    indx2 = Str.indexOf(';',indx1+1);
    uart.node.value = Str.substring(indx1,indx2);
    indx1 = indx2+1;
    indx2 = Str.indexOf(';',indx1+1);
    uart.node.remark = Str.substring(indx1,indx2);
    indx1 = indx2+1;
    indx2 = Str.indexOf(';',indx1+1);
    
}


void serif_decode_node_str(char *msg_decoded, char *msg_in, uint8_t max_len)
{
	String StrRadio = msg_in;  
	String StrTx ="";
	
	StrTx += json_parse_tag(StrRadio, "{\"Z");
	StrTx += ';';
	StrTx += json_parse_tag(StrRadio, ",\"S");
	StrTx += ';';
	StrTx += json_parse_tag(StrRadio, ",\"V");
	StrTx += ';';
	StrTx += json_parse_tag(StrRadio, ",\"R");
	//Serial.println(StrRadio); Serial.println(StrTx);
	StrTx.toCharArray(msg_decoded, max_len);
}


void serif_build_node_tx_str(void)
{
    serif_prepare_reply();
    if(rfm69_receive_message_is_avail()){
        rfm_receive_msg_st *receive_p = rfm69_get_receive_data_ptr();
        String StrRadio = (char*) receive_p->radio_msg;  
		serif_decode_node_str(&uart.tx.msg[serif_FRAME_POS_DATA],
				receive_p->radio_msg, 
				serif_MAX_REPLY_LEN - serif_FRAME_POS_END -3);
        uint8_t len = strlen(uart.tx.msg);
        uart.tx.msg[len-1] = '>';
        uart.tx.msg[len] = 0x00;
    }
    else {
        // return an empty frame  starting with '*'
    }
}

void serif_get_decoded_msg( char *buff, uint8_t max_len, bool clr_avail)
{
	buff[0] = 0x00;
	rfm69_receive_message();
	if(rfm69_receive_message_is_avail())
	{
        rfm_receive_msg_st *receive_p = rfm69_get_receive_data_ptr();
		//strncpy(buff,(char*)receive_msg.radio_msg, max_len);
		if (clr_avail) rfm69_clr_receive_message_flag();
		serif_decode_node_str(buff, receive_p->radio_msg, max_len);
	}
}


void serif_build_raw_tx_str(void)
{
    rfm_receive_msg_st *receive_p = rfm69_get_receive_data_ptr();
    // uart.tx.str = "<#X1r:";
    serif_prepare_reply();
    memcpy(&uart.tx.msg[serif_FRAME_POS_DATA],(char*) receive_p->radio_msg, MAX_MESSAGE_LEN);
    uint8_t len  = strlen(uart.tx.msg);
    uart.tx.msg[len] = '>';  uart.tx.msg[len+1] = 0x00;
}

void serif_rx_send_rfm_from_raw(void)
{
    uart.rx.msg[uart.rx.len-1] = 0x00;
    // Serial.print("serif_rx_send_rfm_from_raw: "); Serial.println(uart.rx.msg);
    // Serial.print("...len: "); Serial.println(uart.rx.len);
    rfm69_radiate_msg(&uart.rx.msg[serif_FRAME_POS_DATA]);
}

void serif_rx_send_rfm_from_node(void)
{
	uint16_t len = strlen(uart.rx.msg);
	uart.rx.msg[len-1] = 0x00;
    serif_build_node_from_rx_str();
    rfm_send_msg_st *send_p = rfm69_get_send_data_ptr();
    json_convert_serif_node_to_json(send_p->radio_msg, &uart);
    rfm69_radiate_msg(send_p->radio_msg);
}

void serif_radiate_node_json(char *buff)
{
	memcpy(uart.rx.msg,buff,MAX_MESSAGE_LEN);
	serif_rx_send_rfm_from_node();
}


void serif_exec_cmnd(serif_cmd_et ucmd)
{
	// if(serif_serial != nullptr)
	{
		switch(ucmd)
		{
			case serif_CMD_TRANSMIT_RAW:
				serif_rx_send_rfm_from_raw();
				break;
			case serif_CMD_TRANSMIT_NODE:
				serif_rx_send_rfm_from_node();
				break;
			case serif_CMD_GET_AVAIL:
				serif_prepare_reply(); 
				if(rfm69_receive_message_is_avail()) uart.tx.msg[serif_FRAME_POS_DATA] = '1';
				else uart.tx.msg[serif_FRAME_POS_DATA] = '0';
				Serial69.println(uart.tx.msg);
				break;
			case serif_CMD_GET_RSSI:
				serif_prepare_reply(); 
				if(rfm69_receive_message_is_avail()){
					String Str = String(rfm69_get_last_rssi());
					Str.toCharArray(&uart.tx.msg[serif_FRAME_POS_DATA], serif_MAX_REPLY_LEN - serif_FRAME_POS_DATA -3);
					//Serial.println(uart.tx.msg);
					uint8_t len = strlen(uart.tx.msg);
					uart.tx.msg[len] = serif_FRAME_END;
					uart.tx.msg[len+1] = 0x00;
				}
				else uart.tx.msg[serif_FRAME_POS_DATA] = serif_FRAME_DUMMY;
				Serial69.println(uart.tx.msg);
				break;
			case serif_CMD_READ_RAW:
				serif_build_raw_tx_str();
				rfm69_clr_receive_message_flag();
				Serial69.println(uart.tx.msg);          
				break;
			case serif_CMD_READ_NODE:
				serif_build_node_tx_str();
				rfm69_clr_receive_message_flag();
				Serial69.println(uart.tx.msg);          
				break;
			case serif_CMD_SET_PARAM:
				rfm69_set_transparent(uart.rx.msg[serif_FRAME_POS_DATA] == '1');
				break;

		}
	}
}


void serif_print_rx_metadata(void)
{
    #ifdef MODEM_DEBUG_PRINT
    Serial.print(F("Length      ")); Serial.println(uart.rx.len);
    Serial.print(F("Avail       ")); Serial.println(uart.rx.avail);
    Serial.print(F("Status      ")); Serial.println(uart.rx.status);
    Serial.print(F("Tag         ")); Serial.println(uart.rx.frame.tag);
    Serial.print(F("Address     ")); Serial.println(uart.rx.frame.addr);
    Serial.print(F("Function    ")); Serial.println(uart.rx.frame.function);
    Serial.print(F("Index       ")); Serial.println(uart.rx.frame.index);
    Serial.print(F("Action      ")); Serial.println(uart.rx.frame.action);
    Serial.print(F("Format      ")); Serial.println(uart.rx.format);
    #endif
}    

/*
void serif_rx_task(void)
{
    static uint8_t rx_state = 0;
    switch(rx_state)
    {
        case 0:
            rx_state = 10;
            break;
        case 10:
            serif_read_uart();    // if available -> uart->prx.str uart->rx.avail
            if(uart.rx.avail) rx_state = 20;
            break;
        case 20:
                serif_parse_rx_frame();
                #ifdef MODEM_DEBUG_PRINT
                Serial.println(uart.rx.msg);
                serif_print_rx_metadata();
                #endif
                uart.rx.avail = false;
                //Serial.print("uart.rx.status="); Serial.println(uart.rx.status);
                if (uart.rx.status == STATUS_CORRECT_FRAME) rx_state = 30;
                else rx_state = 10;
            break;
        case 30:
            serif_exec_cmnd((serif_cmd_et)uart.rx.frame.function);
            rx_state = 10;
            break;
        case 40:
            rx_state = 10;
            break;
    }
}
*/