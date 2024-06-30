#include "cli.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

static char args[ARG_SIZE][ARG_CHAR_SIZE] = {0};
static uint8_t cli_parser_state = 0;

static void cli_help(const char args[ARG_SIZE][ARG_CHAR_SIZE],const int q);
static void cli_reboot(const char args[ARG_SIZE][ARG_CHAR_SIZE],const int q);

static void cli_transmit_string(const char *data);

static const CLI_CMD_t CLI_CMD[] = {
    /* command          function. */
    {"help",            cli_help},
    {"reboot",          cli_reboot},
};


#define cli_parser_run cli_parser_state = 0
#define cli_parser_stop cli_parser_state = 1


int cli_printf(const char *__restrict format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    cli_transmit_string(buffer);

    va_end(args);
    return 0;
}


void cli_init(void){
	LL_USART_EnableIT_RXNE(CLI_USART_INSTANCE);
	cli_parser_stop;
	LL_mDelay(10);
	cli_transmit_string(cli_hello_msg);
}


void cli_it_handler(void){

	if (LL_USART_IsActiveFlag_RXNE(CLI_USART_INSTANCE)){

		uint8_t data = 0;
		static uint8_t rx_buffer[ARG_CHAR_SIZE] = {0};
		static uint8_t rx_buffer_len = 0;
		static uint8_t args_c = 0;

		data = LL_USART_ReceiveData8(CLI_USART_INSTANCE);
		rx_buffer[rx_buffer_len++] = data;

		if(rx_buffer_len > ARG_CHAR_SIZE){
			memset(rx_buffer, 0, ARG_CHAR_SIZE);
			memset(&data, 0, sizeof(uint8_t));
			rx_buffer_len = 0;
			cli_printf("USART Receiver Buffer overflow\r\nMust be max = %d Byte\r\n",ARG_CHAR_SIZE);
		}

		if(('\r' == data) || (' ' == data)){

			memcpy(args[args_c] ,rx_buffer,rx_buffer_len - 1);
			memset(args[args_c] + rx_buffer_len - 1, 0, ARG_CHAR_SIZE - rx_buffer_len);

			rx_buffer_len = 0;
			args_c++;

			if('\r' == data){
				args_c = 0;
				cli_parser_run;
			}
		}
	//        LL_USART_ClearFlag_RXNE(CLI_USART_INSTANCE);
	}
}


void cli_parse(void){
    if(cli_parser_state)
        return;

    uint8_t command_found = 0;
    for(int i = 0; i < sizeof(CLI_CMD) / sizeof(CLI_CMD[0]); i++){
		for(int j = 0; j < ARG_SIZE; j++){
			if(strcmp(CLI_CMD[i].pCmd, args[j]) == 0){
				CLI_CMD[i].pFun(args,j);
				command_found = 1;
				break;
			}
		}
    }

    if (!command_found)
    	cli_transmit_string("Unknown command\r\n\r\n");


    cli_parser_stop;
}


static void cli_transmit_string(const char *data){
    while(*data){
        while(!LL_USART_IsActiveFlag_TXE(CLI_USART_INSTANCE)); //Check Transmit Data Register is empty
        LL_USART_TransmitData8(CLI_USART_INSTANCE, (uint8_t)*data); //Send the char
        data++; //Next character
    }
}


static void cli_help(const char args[ARG_SIZE][ARG_CHAR_SIZE], const int q) {
	cli_transmit_string(cli_help_msg);
}


static void cli_reboot(const char args[ARG_SIZE][ARG_CHAR_SIZE],const int q) {
	cli_transmit_string("Reboot command executed\r\n\r\n");
    LL_mDelay(10);
    NVIC_SystemReset();
}


