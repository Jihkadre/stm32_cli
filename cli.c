#include "cli.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static char args[ARG_SIZE][ARG_CHAR_SIZE];
static uint8_t cli_parser_state = 1;

static void cli_help(char args[ARG_SIZE][ARG_CHAR_SIZE]);
static void cli_reboot(char args[ARG_SIZE][ARG_CHAR_SIZE]);

static void CLI_Transmit_String(volatile char *data);


const CLI_CMD_t CLI_CMD[] = {
    /* command          function. */
    {"help",            cli_help},
    {"reboot",          cli_reboot},
};


#define cli_parser_run cli_parser_state = 0
#define cli_parser_stop cli_parser_state = 1


USART_TypeDef *CLI_USART_INSTANCE;


void CLI_Init(USART_TypeDef *USARTx){
	CLI_USART_INSTANCE = USARTx;
	LL_USART_EnableIT_RXNE(CLI_USART_INSTANCE);
	LL_mDelay(10);
	CLI_Transmit_String((void *)cli_hello_msg);
}


void CLI_IT_Handler(void){

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
			CLI_Transmit_String("USART Receiver Buffer overflow\r\nMust be max = 20 Byte\r\n");
		}

		if(('\r' == data) || (' ' == data)){

			memcpy(args[args_c] ,rx_buffer,rx_buffer_len - 1);
			memset(args[args_c] + rx_buffer_len - 1, 0, ARG_CHAR_SIZE - rx_buffer_len);

			rx_buffer_len = 0;
			args_c++;

			if(('\r' == data)){
				args_c = 0;
				cli_parser_run;
			}
		}
	//        LL_USART_ClearFlag_RXNE(CLI_USART_INSTANCE);
	}
}


void CLI_Parse(void){
    if(cli_parser_state)
        return;

    uint8_t command_found = 0;
    for(size_t i = 0; i < sizeof(CLI_CMD) / sizeof(CLI_CMD[0]); i++){
        if(strcmp(CLI_CMD[i].pCmd, args[0]) == 0){
            CLI_CMD[i].pFun(args);
            command_found = 1;
            break;
        }
    }

    if (!command_found) {
        CLI_Transmit_String("Unknown command\r\n\r\n");
    }

    cli_parser_stop;
}


static void CLI_Transmit_String(volatile char *data){
    while(*data){
        while(!LL_USART_IsActiveFlag_TXE(CLI_USART_INSTANCE)); //Check Transmit Data Register is empty
        LL_USART_TransmitData8(CLI_USART_INSTANCE, (uint8_t)*data); //Send the char
        data++; //Next character
    }
}


static void cli_help(char args[ARG_SIZE][ARG_CHAR_SIZE]) {
    CLI_Transmit_String((void *)cli_help_msg);
}


static void cli_reboot(char args[ARG_SIZE][ARG_CHAR_SIZE]) {
    CLI_Transmit_String("Reboot command executed\r\n\r\n");
    LL_mDelay(10);
    NVIC_SystemReset();
}


