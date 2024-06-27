#pragma once

#include "stm32h7xx_ll_usart.h"

#define CLI_USART_INSTANCE	USART1

#define ARG_CHAR_SIZE 20
#define ARG_SIZE	  5

typedef struct {
    const char *pCmd;
    void (*pFun)(char [ARG_SIZE][ARG_CHAR_SIZE]);
} CLI_CMD_t;

void CLI_Init(void);
void CLI_IT_Handler(void);
void CLI_Parse(void);



