/**
 * @file    xmodem.c
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module is the implementation of the Xmodem protocol.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#include "stm8s.h"
#include "xmodem.h"
#include "xmoden_uart.h"
#include "xmoden_flash.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////need to be change , if move to other platform///
																//HAL_Delay(100);\////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TRUE 	(1)
#define FALSE (0)

//#define flash_jump_to_app()     NVIC_SystemReset()
#define flash_jump_to_app() //do{\
																HAL_FLASH_Unlock();\
																StmFlash_WriteNoCheck(UPGRADE_INFO_FLASH_START_ADDR,(uint16_t *)IAP_START_UPGRADE,(sizeof(IAP_START_UPGRADE)+1)>>1);\
																HAL_FLASH_Lock();\
																NVIC_SystemReset();\
															}while(0)

/* Global variables. */
static uint8_t x_packet_number = 1u;          /**< Packet number counter. */
static uint32_t x_actual_flash_address = 0u;  /**< Address where we have to write. */
static uint8_t first_packet_received = FALSE; /**< First packet or not. */

/* Local functions. */
static uint16_t xmodem_calc_crc(uint8_t *data, uint16_t length);
static xmodem_status xmodem_handle_packet(uint8_t size);

/**
 * @brief   This function is the base of the Xmodem protocol.
 *          When we receive a header from UART, it decides what action it shall take.
 * @param   void
 * @return  void
 */
#define CONTROL_CCOUNTER (0X51)
static void xmodem_receive(void)
{
  volatile xmodem_status status = X_OK;
  uint8_t poweron=CONTROL_CCOUNTER,error_number = 0u;

  first_packet_received = FALSE;
  x_packet_number = 1u;
  x_actual_flash_address = FLASH_OTAAPP_START_ADDR;

  /* Loop until there isn't any error (or until we jump to the user application). */
  while (X_OK == status && poweron)
  {
    uint8_t header = 0x00u;

    /* Get the header from UART. */
    Xmoden_stUartStatus comm_status = Xmoden_UartReceive(&header, 1);

    /* Spam the host (until we receive something) with ACSII "C", to notify it, we want to use CRC-16. */
    if ((UART_OK != comm_status) && (!first_packet_received))
    {
        if(poweron > CONTROL_CCOUNTER)
            (void)Xmoden_UartTransmitByte(X_C);
    }
		
    if(poweron && poweron > CONTROL_CCOUNTER-1)
    {
      if(header == ' ')
      {
          (void)Xmoden_UartTransmitByte(X_C+1);
          poweron = 0xff;
          continue;
      }
      if(--poweron == CONTROL_CCOUNTER-1)
          return;
      if(UART_TIME_OUT == comm_status)
          continue;
    }

    /* If the number of errors reached critical, then abort. */
    if (error_number >= X_MAX_ERRORS)
    {
      /* Graceful abort. */
      (void)Xmoden_UartTransmitByte(X_CAN);
      (void)Xmoden_UartTransmitByte(X_CAN);
      status = X_ERROR;
    }

    /* The header can be: SOH, STX, EOT and CAN. */
    switch(header)
    {
      /* 128 or 1024 bytes of data. */
      case X_SOH:
      case X_STX:
				poweron = 0xff;
        /* If the handling was successful, then send an ACK, otherwise NAK. */
        if (X_OK == xmodem_handle_packet(header))
        {
          (void)Xmoden_UartTransmitByte(X_ACK);
        }
        else
        {
          (void)Xmoden_UartTransmitByte(X_NAK);
          ++error_number;
        }
        break;
      /* End of Transmission. */
      case X_EOT:
        /* ACK, feedback to user (as a text), then jump to user application. */
        (void)Xmoden_UartTransmitByte(X_ACK);
        (void)Xmoden_UartTransmitStr((uint8_t*)"\n\rFirmware updated!\n\r");
        (void)Xmoden_UartTransmitStr((uint8_t*)"reset to make flash pass,Jumping to user application...\n\r");
        flash_jump_to_app();
        break;
      /* Abort from host. */
      case X_CAN:
        status = X_ERROR;
        break;
      default:
        /* Wrong header. */
        if ((UART_OK == comm_status))
        {
          if(header != ' ')
          {
            ++error_number;
            (void)Xmoden_UartTransmitByte(X_NAK);
          }
        }
        break;
    }
  }
}

/**
 * @brief   Calculates the CRC-16 for the input package.
 * @param   *data:  Array of the data which we want to calculate.
 * @param   length: Size of the data, either 128 or 1024 bytes.
 * @return  status: The calculated CRC.
 */
static uint16_t xmodem_calc_crc(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0u;
    while (length)
    {
        --length;
        crc = crc ^ ((uint16_t)*data++ << 8u);
        for (uint8_t i = 0u; i < 8u; ++i)
        {
            if (crc & 0x8000u)
            {
                crc = (crc << 1u) ^ 0x1021u;
            }
            else
            {
                crc = crc << 1u;
            }
        }
    }
    return crc;
}

/**
 * @brief   This function handles the data packet we get from the xmodem protocol.
 * @param   header: SOH or STX.
 * @return  status: Report about the packet.
 */
static xmodem_status xmodem_handle_packet(uint8_t header)
{
  xmodem_status status = X_OK;
  uint16_t size = 0u;
  if (X_SOH == header)
  {
    size = X_PACKET_128_SIZE;
  }
  else if (X_STX == header)
  {
    size = X_PACKET_1K_SIZE;
  }
  else
  {
    /* Wrong header type. */
    status = X_ERROR;
  }
  uint16_t length = size + X_PACKET_DATA_INDEX + X_PACKET_CRC_SIZE;
  uint8_t received_data[X_PACKET_1K_SIZE + X_PACKET_DATA_INDEX + X_PACKET_CRC_SIZE];

  /* Get the packet (except for the header) from UART. */
  Xmoden_stUartStatus comm_status = Xmoden_UartReceive(&received_data[0u], length);
  /* The last two bytes are the CRC from the host. */
  uint16_t crc_received = ((uint16_t)received_data[length-2u] << 8u) | ((uint16_t)received_data[length-1u]);
  /* We calculate it too. */
  uint16_t crc_calculated = xmodem_calc_crc(&received_data[X_PACKET_DATA_INDEX], size);

  /* If it is the first packet, then erase the memory. */
  if (FALSE == first_packet_received)
  {
    if (FLASH_OK == Xmoden_FlashErase(FLASH_OTA_FLAG_START_ADDR))
    {
      first_packet_received = TRUE;
    }
    else
    {
      status |= X_ERROR_FLASH;
    }
  }

  /* Error handling. */
  if (UART_OK != comm_status)
  {
    /* UART error. */
    status |= X_ERROR_UART;
  }
  if (x_packet_number != received_data[X_PACKET_NUMBER_INDEX])
  {
    /* Packet number counter mismatch. */
    status |= X_ERROR_NUMBER;
  }
  if (255u != (received_data[X_PACKET_NUMBER_INDEX] +  received_data[X_PACKET_NUMBER_C_INDEX]))
  {
    /* The sum of the packet number and packet number complement aren't 255. */
    /* The sum always has to be 255. */
    status |= X_ERROR_NUMBER;
  }
  if (crc_calculated != crc_received)
  {
    /* The calculated and received CRC are different. */
    status |= X_ERROR_CRC;
  }
  /* Do the actual flashing. */
  if (FLASH_OK != Xmoden_FlashWrite(x_actual_flash_address, (uint8_t*)&received_data[X_PACKET_DATA_INDEX], (uint16_t)size/4u))
  {
    /* Flashing error. */
    status |= X_ERROR_FLASH;
  }

  /* Raise the packet number and the address counters (if there wasn't any error). */
  if (X_OK == status)
  {
    ++x_packet_number;
    x_actual_flash_address += size;
  }

  return status;
}


void XmodenInit(void)
{
  Xmoden_UartInit();
  Xmoden_FlashInit();
}
void XmodenCheck(void)
{
  xmodem_receive();
}

