//******************************************************************************
//*
//* This Information Proprietary  To Kong Studio Corporation
//*
//******************************************************************************
//*
//*           Copyright (c) 2015 Kong Studio Corporation
//*                  ALL RIGHTS RESERVED
//*
//******************************************************************************
    
//******************************************************************************
//* FILE NAME: Main.c
//*
//* DESCRIPTION: STM8S Lesson1 example
//*
//* ORIGINATOR: Konglittle
//*
//* DATE: 2015-03-15
//*
//******************************************************************************
#include <string.h>
#include "SerialPort.h"
#include "stm8s_uart1.h"

#define SendData_Byte(x)     UART1_SendData8(x)

#define SendDataComplete()   UART1_GetFlagStatus(UART1_FLAG_TXE)

#define DisableRxData()   UART1_ITConfig(UART1_IT_RXNE_OR, DISABLE)
#define EnaleRxData()   UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE)


/*******************************************************************************
* Function Name  : SendData_String.
* Description    : Send a string data
* Input          : String address that need to send
* Output         : None.
* Return         : None.
*******************************************************************************/
void SendData_String(uint8_t *pBuf)
{
    //Disable RX ISR in case that it receive its own tx data
    DisableRxData();
    while(*pBuf != 0)
    {
        SendData_Byte(*pBuf++);
        while(!SendDataComplete());
    }
    EnaleRxData();
}


/*******************************************************************************
* Function Name  : SendData_Number.
* Description    : Convert a number to string then send it to PC
* Input          : String address that need to send
* Output         : None.
* Return         : None.
*******************************************************************************/
void SendData_Number(int16_t wNumber)
{
  uint8_t ubTempBuf[7] = {0};
    uint8_t ubPreNoneZero = 0;
    uint16_t uwDivide = 0;
    uint16_t uwTempValue = 0;
    uint8_t ubLength = 0;
    
    //If the value is negative, then print "-" before value
    if(wNumber < 0)
    {
        ubTempBuf[ubLength++] = '-';
        wNumber = wNumber * (-1);
    }
    
    if(wNumber == 0)
    {
        ubTempBuf[0] = '0';
    }
    else
    {
        //Convert number to string
        for (uwDivide = 10000; uwDivide > 0; uwDivide /= 10)
        {
            uwTempValue = wNumber / uwDivide;
    
            if (uwTempValue || ubPreNoneZero)
            {
                //Convert number to ascii
                ubTempBuf[ubLength++] = uwTempValue + '0';
                wNumber -= (uwTempValue * uwDivide);
                //Set this flag, so we can recognize 0 as payload because the higher digit is non-zero
                ubPreNoneZero = 1;
            }
        }
    }
    
    //Set 0 at the end of buffer
    ubTempBuf[ubLength++] = 0;
    
    SendData_String(&ubTempBuf[0]);
}