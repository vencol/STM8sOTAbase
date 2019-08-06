/**
 * @file    uart.h
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module is a layer between the HAL UART functions and my Xmodem protocol.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#ifndef XMODEN_UART_H_
#define XMODEN_UART_H_


/* Timeout for HAL. */
#define UART_TIMEOUT ((uint32_t)0xffffUL)

/* Status report for the functions. */
typedef enum {
  UART_OK     = 0x00u, /**< The action was successful. */
  UART_TIME_OUT,  /**< TIMEOUT error. */
  UART_ERROR  = 0xFFu  /**< Generic error. */
} Xmoden_stUartStatus;

extern void Xmoden_UartInit(void);
extern Xmoden_stUartStatus Xmoden_UartReceive(uint8_t *data, uint16_t len);
extern Xmoden_stUartStatus Xmoden_UartTransmitByte(uint8_t data);
extern Xmoden_stUartStatus Xmoden_UartTransmitStr(uint8_t *data);


#endif /* UART_H_ */
