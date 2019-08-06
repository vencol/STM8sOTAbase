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
#include "stm8s.h"
#include "xmodem.h"


/*******************************************************************************
* Function Name  : InitCpuClock.
* Description    : Initial CPU clock, .
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void InitCpuClock(void)
{
  CLK_DeInit();
//  Use HSI @2MHZ, div = 8
  CLK_HSECmd(DISABLE);
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  CLK_HSICmd(ENABLE);
}
//static uint8_t *MainValidFlag = (uint8_t *)MAIN_VALID_FLAG_ADDRESS;

//#pragma location = ".bootflag"
//__no_init volatile uint8_t ubBootFlag[2];
//
//
//void DelayApi_nMs(uint16_t uwN_1ms);
//static void InitCpuClock(void);


uint8_t ubStartTimerFlag = 0;
//uint16_t ISR_uwTimerCount = 0;
//uint8_t FLASH_ubUartDataSize = 0;
//uint8_t ubUart1RxBuf[1036] = {0};
//uint8_t MainToBld = 0;

//Jump to Main APP api
//const TFunction MainUserApplication = (TFunction)MAIN_USER_RESET_ADDR;

//Jump to bootloader(reset) api
//const TFunction BldUserApplication = (TFunction)BLD_USER_RESET_ADDR;
uint8_t ubUart1RxBuf[4096] = {0};

/*******************************************************************************
* Function Name  : main.
* Description    : main function of this project
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
int main( void )
{
    InitCpuClock();
    Xmoden_UartInit();
//    InitUart1();
    //Initialize GPIOB_Pin5 to output
//    GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_LOW_SLOW);

    //Check whether jump from APP: if match, then it means that
    //APP receive flash command
//    if((ubBootFlag[0] == 0xAA) &&
//       (ubBootFlag[1] == 0xBB))
//    {
//        MainToBld = 1;
//        ubBootFlag[0] = 0;
//        ubBootFlag[1] = 0;
//    }
    //Check main APP exist or not
//    else if(subCheckMainApp())
//    {
//        //Mark it when need to jump to APP, then use it as a flag
//        //to determin whether we need redirection vector
//        MainToBld = 2;
//        
//        asm("LDW X,  SP ");
//        asm("LD  A,  $FF");
//        asm("LD  XL, A  ");
//        asm("LDW SP, X  ");
//        
//        // and then jump to user application
//        MainUserApplication();
//    }
    
    //Timer ISR is only for BLD
//    InitTimer2();
//    enableInterrupts();
      
    while(1)
    {
      
      Xmoden_UartReceive(ubUart1RxBuf,4096);
      if(ubUart1RxBuf[0] == '8')
      {
        Xmoden_UartTransmitStr("App65465PAss£º");
        Xmoden_UartTransmitStr(ubUart1RxBuf);
      }
      else if(ubUart1RxBuf[0] == '6')
      {
        Xmoden_UartTransmitStr("App65465PAss£º");
        Xmoden_UartTransmitStr("ha54456haha");
      }
      ubUart1RxBuf[0] = '1';
//      XmodenCheck();
//        FLASH_Unlock(FLASH_MEMTYPE_PROG);
//        if((FLASH_ubUartDataSize != 0) || (1 == MainToBld))
//        {
//            //Delay 20ms to make sure all the data are received
//            DelayApi_nMs(25);
//            if(((ubUart1RxBuf[0] == 'f') &&
//               (ubUart1RxBuf[1] == 'l') &&
//               (ubUart1RxBuf[2] == 'a') &&
//               (ubUart1RxBuf[3] == 's') &&
//               (ubUart1RxBuf[4] == 'h')) ||
//               (1 == MainToBld))
//            {
//                //erase
//                static uint16_t uwBlockNumber = 0;
//                static uint16_t MaxBlockNumber = FLASH_PROG_BLOCKS_NUMBER;
//                
//                
//                //Find the start block number
//                uwBlockNumber = (uint16_t)((MAIN_USER_RESET_ADDR - 0x8000u) / (uint16_t)FLASH_BLOCK_SIZE);
//                MaxBlockNumber = FLASH_PROG_BLOCKS_NUMBER - uwBlockNumber;
//                
//                while(1)
//                {
//                    FLASH_EraseBlock(uwBlockNumber, FLASH_MEMTYPE_PROG);
//                    
//                    FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG);
//                    
//                    if(uwBlockNumber++ >= MaxBlockNumber) 
//                    {
//                        break;
//                    }
//                }
//            }
//            //If it is flash data, then the start code is ':'
//            else if(ubUart1RxBuf[0] == ':')
//            {
//                //Pload is from second byte
//                FlashHandler(&ubUart1RxBuf[1], FLASH_ubUartDataSize - 1);
//            }
//            else if((ubUart1RxBuf[0] == 'e') &&
//                    (ubUart1RxBuf[1] == 'n') &&
//                    (ubUart1RxBuf[2] == 'd'))
//            {
//                //jump to main or reset
//                asm("LDW X,  SP ");
//                asm("LD  A,  $FF");
//                asm("LD  XL, A  ");
//                asm("LDW SP, X  ");
//                
//                // software reset
//                BldUserApplication();
//            }
//            
//            memset(&ubUart1RxBuf[0], 0, sizeof(ubUart1RxBuf) / sizeof(uint8_t));
//            FLASH_ubUartDataSize = 0;
//            
//            MainToBld = 0;
//        }
//        
//        FLASH_Lock(FLASH_MEMTYPE_PROG);
    }
}






#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
