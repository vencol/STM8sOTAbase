
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
    XmodenInit();
    XmodenCheck();
    OTA_Check();
    while(1)//never get here
    {
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
