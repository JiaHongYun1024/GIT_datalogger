/****************************************************************************
 *
 *   Copyright (c) 2012-2015 L3YZ Development Team. All rights reserved.
 *
 ****************************************************************************/

/**
* @file typedef.h
* Type define file.
*
* @author Z1
*
*/

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <stdbool.h>

typedef unsigned char uint8_t;
typedef   signed char int8_t;

typedef unsigned short uint16_t;
typedef   signed short int16_t;

typedef unsigned int  uint32_t;
typedef   signed int int32_t;


typedef unsigned long long uint64_t;
typedef   signed long long int64_t;

#undef NULL
#define NULL 0

#undef null
#define null 0

#define O_RDONLY    (1 << 0)        /* Open for read access (only) */
#define O_RDOK      O_RDONLY        /* Read access is permitted (non-standard) */
#define O_WRONLY    (1 << 1)        /* Open for write access (only) */
#define O_WROK      O_WRONLY        /* Write access is permitted (non-standard) */
#define O_RDWR      (O_RDOK|O_WROK) /* Open for both read & write access */
#define O_CREAT     (1 << 2)        /* Create file/sem/mq object */

#define pthread_t	void*

#endif /*_TYPEDEF_H_*/
