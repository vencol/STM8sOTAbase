/**
 * @file    uart.h
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module is a layer between the HAL UART functions and my Xmodem protocol.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#ifndef XMODEN_FLASH_H_
#define XMODEN_FLASH_H_
//common
#define FLASH_PAGE_SIZE                     (FLASH_BLOCK_SIZE)
#define FLASH_TOTAL_PAGE                    (64 * 1024 / FLASH_PAGE_SIZE)
#define FLASH_START_ADDR                    (FLASH_PROG_START_PHYSICAL_ADDRESS)
//bootlader
#define FLASH_BOOTLOADER_SIZE               (5 * 1024)
#define FLASH_BOOTLOADER_PAGE               (FLASH_BOOTLOADER_SIZE / FLASH_PAGE_SIZE)
#define FLASH_BOOTLOADER_START_ADDR         (FLASH_START_ADDR)//8000
//app1
#define FLASH_APP_SIZE                      (28 * 1024)
#define FLASH_APP_PAGE                      (FLASH_APP_SIZE / FLASH_PAGE_SIZE)
#define FLASH_APP_START_ADDR                (FLASH_BOOTLOADER_START_ADDR + FLASH_BOOTLOADER_SIZE)//9000
//app2
#define FLASH_OTAAPP_SIZE                   (28 * 1024)
#define FLASH_OTAAPP_PAGE                   (FLASH_OTAAPP_SIZE / FLASH_PAGE_SIZE)
#define FLASH_OTAAPP_START_ADDR             (FLASH_APP_START_ADDR + FLASH_APP_SIZE)//f400
//data
#define FLASH_DATA_SIZE                     (3 * 1024 - FLASH_PAGE_SIZE)
#define FLASH_DATA_PAGE                     (FLASH_DATA_SIZE / FLASH_PAGE_SIZE)
#define FLASH_DATA_START_ADDR               (FLASH_OTAAPP_START_ADDR + FLASH_OTAAPP_SIZE)//168000
//OTA flag
#define FLASH_OTA_FLAG_SIZE                  (FLASH_PAGE_SIZE)
#define FLASH_OTA_FLAG_START_ADDR            ( FLASH_DATA_START_ADDR + FLASH_DATA_SIZE)//168000

#define FLASH_OTA_FLAG_PASS                  ("PASS")
#define FLASH_OTA_FLAG_FAIL                  ("FAIL")

typedef FAR void (*TFunction)(void);

/* Status report for the functions. */
typedef enum {
  FLASH_OK     = 0x00u, /**< The action was successful. */
  FLASH_ERROR  = 0xFFu  /**< Generic error. */
} Xmoden_stFlashStatus;

extern void Xmoden_FlashInit(void);
extern Xmoden_stFlashStatus Xmoden_FlashWrite(uint32_t address, uint8_t *data, uint16_t len);
extern Xmoden_stFlashStatus Xmoden_FlashRead(uint32_t address, uint8_t *data, uint16_t len);
extern Xmoden_stFlashStatus Xmoden_FlashErase(uint32_t address);
extern Xmoden_stFlashStatus Xmoden_WriteFlag(uint8_t type);


#endif /* UART_H_ */
