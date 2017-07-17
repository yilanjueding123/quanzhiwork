/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     Some backup function for shell
*
* File    : Esh_platform.h
* By      : sunny
* Version : V1.00                                                                 2009.3.2
*********************************************************************************************************
*/

#ifndef  _ESH_PLATFORM_H_
#define  _ESH_PLATFORM_H_

/* shell platform choose */
#define  ESH_PLATFORM_EPOS         /* shell under epos platform */

/* other os choose */

/* epos platform */
#ifdef     ESH_PLATFORM_EPOS

#include  "Esh_platform_epos.h"

#endif      /* ESH_PLATFORM_EPOS */

#endif      /* _ESH_PLATFORM_H_ */
