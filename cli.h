#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_ll_usart.h"


#define ARG_CHAR_SIZE 25
#define ARG_SIZE 5


extern void LL_mDelay(uint32_t Delay);

typedef struct {
    const char *pCmd;
    void (*pFun)(char [ARG_SIZE][ARG_CHAR_SIZE]);
} CLI_CMD_t;


static const char cli_help_msg[] =
    "STM32 CLI Help\r\n"
    "------------------------------\r\n"
    "Available commands:\r\n"
    "1. help   - Display this help message.\r\n"
    "2. reboot - Reboot the STM32 microcontroller.\r\n"
    "\r\n"
    "Usage examples:\r\n"
    "1. To see this help message, type: help\r\n"
    "2. To reboot the device, type: reboot\r\n"
    "------------------------------\r\n\r\n\r\n";

static const char cli_hello_msg[] = {
		"Welcome to the STM32 Based CLI (Command Line Interface)\r\n"
		"---------------------------------------------------------\r\n"
		"This CLI allows you to interact with the STM32 microcontroller.\r\n"
		"You can use various commands to control and monitor the system.\r\n"
		"For a list of available commands, type 'help'.\r\n"
		"---------------------------------------------------------\r\n"
		"Please enter your command:\r\n\r\n\r\n"
};


void CLI_Init(USART_TypeDef *USARTx);
void CLI_IT_Handler(void);
void CLI_Parse(void);

