/**
 * @file    uart.c
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module is a layer between the HAL UART functions and my Xmodem protocol.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#include "stm8s.h"
#include "stm8s_uart1.h"
#include "xmoden_uart.h"

#define USE_ST_LIB (0)
#define XMODEN_RECEIVE_TIMEOUT (40000)//(0X3FFFFUL)


void Xmoden_UartInit(void)
{
//  GPIO_Init(GPIOA, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);			//PA4设为悬浮输入
//  GPIO_Init(GPIOA, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);		//PA5设为推挽输出，最大速率10MHZ
  
#if USE_ST_LIB
  UART1_DeInit();
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
  UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, 
              UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);//UART1_MODE_TXRX_ENABLE
//  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  UART1_Cmd(ENABLE);
#else
  (void)UART1->SR;
  (void)UART1->DR;
  UART1->BRR2 = UART1_BRR2_RESET_VALUE;  /* Set UART1_BRR2 to reset value 0x00 */
  UART1->BRR1 = UART1_BRR1_RESET_VALUE;  /* Set UART1_BRR1 to reset value 0x00 */
  UART1->CR1 = UART1_CR1_RESET_VALUE;  /* Set UART1_CR1 to reset value 0x00 */
  UART1->CR2 = UART1_CR2_RESET_VALUE;  /* Set UART1_CR2 to reset value 0x00 */
  UART1->CR3 = UART1_CR3_RESET_VALUE;  /* Set UART1_CR3 to reset value 0x00 */
  UART1->CR4 = UART1_CR4_RESET_VALUE;  /* Set UART1_CR4 to reset value 0x00 */
  UART1->CR5 = UART1_CR5_RESET_VALUE;  /* Set UART1_CR5 to reset value 0x00 */
  UART1->GTR = UART1_GTR_RESET_VALUE;
  UART1->PSCR = UART1_PSCR_RESET_VALUE;
  CLK->PCKENR2 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_PERIPHERAL_UART1 & (uint8_t)0x0F));
    
  /* Set the UART1 BaudRates in BRR1 and BRR2 registers according to UART1_BaudRate value */
  uint32_t BaudRate_Mantissa = 0, BaudRate_Mantissa100 = 0, BaudRate = 115200;
  BaudRate_Mantissa    = ((uint32_t)CLK_GetClockFreq() / (BaudRate << 4));
  BaudRate_Mantissa100 = (((uint32_t)CLK_GetClockFreq() * 100) / (BaudRate << 4));
  UART1->BRR2 |= (uint8_t)((uint8_t)(((BaudRate_Mantissa100 - (BaudRate_Mantissa * 100)) << 4) / 100) & (uint8_t)0x0F); 
  UART1->BRR2 |= (uint8_t)((BaudRate_Mantissa >> 4) & (uint8_t)0xF0); 
  UART1->BRR1 |= (uint8_t)BaudRate_Mantissa;  
  
  UART1->CR2 = (UART1_CR2_TEN | UART1_CR2_REN); 
  UART1->CR1 &= (uint8_t)(~UART1_CR1_UARTD); 
#endif
  
//    Xmoden_UartTransmitStr("this is test\r\n");
    Xmoden_UartTransmitStr("this is APP ota test\r\n");
}

Xmoden_stUartStatus Xmoden_UartTransmitByte(uint8_t data)
{
  uint32_t UartTimeout=100;
  
#if USE_ST_LIB
  while(UART1_GetFlagStatus(UART1_FLAG_TC) == RESET)
    if(--UartTimeout == 0)
      return UART_TIME_OUT;
  UART1_SendData8(data);
#else
  while((UART1->SR & (uint8_t)UART1_FLAG_TC) == RESET)
    if(--UartTimeout == 0)
      return UART_TIME_OUT;
  UART1->DR = data;
#endif
  return UART_OK;
}

Xmoden_stUartStatus Xmoden_UartTransmitStr(uint8_t *data)
{
  uint16_t length = 0u;
  /* Calculate the length. */
  while ('\0' != data[length])
  {
    if(UART_ERROR == Xmoden_UartTransmitByte(data[length]))
      return UART_TIME_OUT;
    length++;
  }
  return UART_OK;
}

Xmoden_stUartStatus Xmoden_UartReceive(uint8_t *data, uint16_t len)
{
  uint16_t i = 0;
  uint32_t UartTimeout=XMODEN_RECEIVE_TIMEOUT;
  if(len == 0)
    return UART_ERROR;
//  while ((UART1->SR & (uint8_t)UART1_FLAG_RXNE) == RESET);//Loop until the UART1 Receive Data Register is not empty//UART1_FLAG_RXNE
//      if(--UartTimeout == 0)
//        return UART_TIME_OUT;
//  *(data + i++) = UART1_ReceiveData8();
//  UART1_ClearFlag(UART1_FLAG_RXNE);
//  Xmoden_UartTransmitByte(*(data + i));
  
  
  while(len)
  {
    len--;
    UartTimeout=XMODEN_RECEIVE_TIMEOUT;
#if USE_ST_LIB
    while(UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET)//Loop until the UART1 Receive Data Register is not empty//UART1_FLAG_RXNE
      if(--UartTimeout == 0)
        return UART_TIME_OUT;
    *(data + i++) = UART1_ReceiveData8();
    UART1_ClearFlag(UART1_FLAG_RXNE);
#else
    while ((UART1->SR & (uint8_t)UART1_FLAG_RXNE) == RESET)//Loop until the UART1 Receive Data Register is not empty//UART1_FLAG_RXNE
      if(--UartTimeout == 0)
        return UART_TIME_OUT;
    *(data + i++) = (uint8_t)UART1->DR;
    UART1->SR = (uint8_t)~(UART1_SR_RXNE);
#endif
    
//  Xmoden_UartTransmitByte(*(data + i));
  }
  
  return UART_OK;
}