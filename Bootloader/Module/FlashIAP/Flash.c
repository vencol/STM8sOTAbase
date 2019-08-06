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
//* FILE NAME: Flash.c
//*
//* DESCRIPTION: STM8S Practice: Bootloader
//*
//* ORIGINATOR: Konglittle
//*
//* DATE: 2015-08-01
//*
//******************************************************************************
#include <string.h>
#include "stm8s.h"
#include "stm8s_uart1.h"
extern void DelayApi_nMs(uint16_t uwN_1ms);

uint8_t ubAscii[16] =
{
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F'    
};
typedef struct st_flash_data
{
    uint8_t  ubFlashDataSize;
    uint16_t uwAddress;
    uint8_t  ubDataType;
    uint8_t  ubFlashData[16];
    uint8_t  ubCheckSum;
}stFlashData;


#define FLASH_BUF_SIZE  (sizeof(stFlashData) / sizeof(uint8_t))

union FlashData
{
    stFlashData  stFormatedFlashData;
    uint8_t      ubFlashBuf[FLASH_BUF_SIZE];
};

static uint8_t AsciiToInt(uint8_t unAscii);

void FlashHandler(uint8_t *pBuffer, uint8_t ubBufferSize)
{
    union FlashData FlashDataTemp = {0};
    uint8_t ubIndex = 0;
    uint8_t ubBuffPtr = 0;
    uint8_t ubCrc = 0;
    
    //for ascii mode, it will converter one byte data into two ascii
    //for example: 01 --> '0' '1'
    for(ubIndex = 0; ubIndex < FLASH_BUF_SIZE; ubIndex++)
    {
        FlashDataTemp.ubFlashBuf[ubIndex] =   (AsciiToInt(*(pBuffer + ubBuffPtr)) << 4)
                                             | AsciiToInt(*(pBuffer + ubBuffPtr + 1));
        ubBuffPtr += 2;
        
        if(ubBuffPtr > ubBufferSize)
        {
            break;
        }
    }
    
    //Not all flash data has 16byte pload
    if((ubBufferSize / 2) < FLASH_BUF_SIZE)
    {
        FlashDataTemp.stFormatedFlashData.ubCheckSum = FlashDataTemp.ubFlashBuf[ubIndex - 1];
        FlashDataTemp.ubFlashBuf[ubIndex - 1] = 0;
    }
    
    //Verify the checksum
    for(ubBuffPtr = 0; ubBuffPtr < (ubIndex - 1); ubBuffPtr++)
    {
        ubCrc += FlashDataTemp.ubFlashBuf[ubBuffPtr];
    }
    ubCrc = (uint8_t)0xff - ubCrc + (uint8_t)0x01;
    
    //checksum is match, then write to flash.
    //Note: when write to flash, the code should be running in RAM area
    if((ubCrc == FlashDataTemp.stFormatedFlashData.ubCheckSum) &&
      (FlashDataTemp.stFormatedFlashData.uwAddress >= FLASH_PROG_START_PHYSICAL_ADDRESS) &&
      (FlashDataTemp.stFormatedFlashData.uwAddress <= FLASH_PROG_END_PHYSICAL_ADDRESS))
    {
        //Only for debugger: uncomment if want to debug it
        static uint8_t ubResult = 0;
        
        //Get pay-load length that need to write to flash area
        ubIndex = FlashDataTemp.stFormatedFlashData.ubFlashDataSize;
        
        for(ubBuffPtr = 0; ubBuffPtr < ubIndex; ubBuffPtr++)
        {
            FLASH_ProgramByte((uint32_t)(FlashDataTemp.stFormatedFlashData.uwAddress + ubBuffPtr),
                              FlashDataTemp.stFormatedFlashData.ubFlashData[ubBuffPtr]);
            
            ubResult = *(PointerAttr uint8_t *) (uint16_t)(FlashDataTemp.stFormatedFlashData.uwAddress + ubBuffPtr);
            UART1_SendData8(ubAscii[ubResult >> 4]);
            DelayApi_nMs(1);
            UART1_SendData8(ubAscii[ubResult & 0x0f]);
        }
        DelayApi_nMs(1);
        UART1_SendData8(0x0D);
        DelayApi_nMs(1);
        UART1_SendData8(0x0A);
    }
}

static uint8_t AsciiToInt(uint8_t unAscii)
{
    uint8_t ubIntNum = 0;
    
    for(ubIntNum = 0; ubIntNum < 16; ubIntNum++)
    {
        if(ubAscii[ubIntNum] == unAscii) break;
    }
    if(ubIntNum == 16)
    {
        ubIntNum = 0;
    }
    return ubIntNum;
}
