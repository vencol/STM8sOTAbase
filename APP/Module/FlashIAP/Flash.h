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
//* FILE NAME: Flash.h
//*
//* DESCRIPTION: STM8S Practice: Bootloader
//*
//* ORIGINATOR: Konglittle
//*
//* DATE: 2016-1-25
//*
//******************************************************************************
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_H
#define __FLASH_H

#include "stm8s.h"

/*****************************************************************************/
/***        Following three address should match icf file definition       ***/
//user app start address: change it if your user app is not this one
#define MAIN_USER_RESET_ADDR  0x9300ul

//BLD start address: change it if your user app is not this one
#define BLD_USER_RESET_ADDR  0x8000ul

//Store main valid flag at the end of flash address: change it if your user app is not this one
#define MAIN_VALID_FLAG_ADDRESS   0x9FF8ul
/*****************************************************************************/

extern uint8_t MainToBld;

//typedef @far void (*)(void) TFunction;
typedef FAR void (*TFunction)(void);


#endif