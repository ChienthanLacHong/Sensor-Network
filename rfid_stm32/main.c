#include "stm32f10x.h"                             
#include "stm32f10x_usart.h"  
#include "stm32f10x_rcc.h"              
#include "stm32f10x_tim.h"              
#include "stm32f10x_gpio.h" 
#include <stdio.h>                  
#include "spi.h"
#include "rfid.h"



void Delay_us(uint32_t delay) {
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < delay);
}

void Delay_ms(uint32_t delay) {
    while (delay--) {
        Delay_us(1000);
    }
}

void USART1_Config(void) {
    
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    
    USART_InitStruct.USART_BaudRate = 9600;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStruct);

    
    USART_Cmd(USART1, ENABLE);
}

void Timer_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Period = 0xFFFF;
    timerInitStructure.TIM_Prescaler = 72 - 1;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_Cmd(TIM2, ENABLE);
}


void USART1_Send_Char(char chr)
{
    while (!(USART1->SR & ( 1 << 7)));
    USART1->DR = chr;
}

void USART1_Send_String(char* str)
{
    while(*str) {
        while( !(USART1->SR & ( 1 << 7)));
        USART1->DR = *str++;
    }
}

void USART1_Send_Hex(uint8_t num)
{
    uint8_t temp;
    temp = num >> 4;
    if(temp > 9) {
        temp += 0x37;
    } else {
        temp += 0x30;
    }
    USART1_Send_Char(temp);
    temp = num & 0x0F;
    if(temp > 9) {
        temp += 0x37;
    } else {
        temp += 0x30;
    }
    USART1_Send_Char(temp);
}


void RFID_Debug_ReadCard(void)
{
    uint8_t status;
    uint8_t tagType[2];
    uint8_t serialNum[5];
    uint8_t i;

    // Reset variables
    for (i = 0; i < 5; i++)
    {
        serialNum[i] = 0;
    }

    // Find cards
    status = RFID_Request(PICC_REQIDL, tagType);
    if (status == 1)
    {
        USART1_Send_String("Card detected! Type: ");
        USART1_Send_Hex(tagType[0]);
        USART1_Send_Hex(tagType[1]);
        USART1_Send_String("\r\n");
        
        // Get card serial number
        status = RFID_Anticoll(serialNum);
        if (status == 1)
        {
            USART1_Send_String("Card UID: ");
            for (i = 0; i < 4; i++)
            {
                USART1_Send_Hex(serialNum[i]);
                USART1_Send_Char(' ');
            }
            USART1_Send_String("\r\n");
        }
        else
        {
            USART1_Send_String("Anticoll error\r\n");
        }
        
        RFID_Halt(); // Halt PICC
    }
}




int main(void) {
    
    USART1_Config(); 
    Timer_Init();
		RFID_Init();

    while (1) {
        RFID_Debug_ReadCard();
				Delay_ms(1000);
    }
}
	