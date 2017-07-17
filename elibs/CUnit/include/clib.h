/*
****************************************************************************************************
*                       Test---CUnit
*                (c) Copyright 2006-2010, All winners Co,Ld.
*                        All Right Reserved
*
* FileName:    clib.c
* Author:      Grace
* Date:        2010-6-4
* Description: platform adaption for standard C lib
*
* Others:      none
* History:
*      <author>    <time>       <version >    <desc>
*       Grace    2010-6-4         1.0         build this moudle
****************************************************************************************************
*/
#ifndef  __CLIB_H__
#define  __CLIB_H__

#define EPDK_TEST_CUNIT

#ifdef EPDK_TEST_CUNIT

#include "epdk.h"
#include "epos_cunit.h"



/****************** memory *********************************/

#define malloc(size)           esMEMS_Malloc(0, size)
#define free(p)                esMEMS_Mfree(0, p)
#define realloc(p, size)       esMEMS_Realloc(0, p, size)
#define calloc(n, m)           esMEMS_Calloc(0, n, m)
//#define memset(p, value, size) eLIBs_memset(p, value, size)

/***********************file systerm**********************/
#define  FILE    ES_FILE

#define fopen     eLIBs_fopen
#define fclose    eLIBs_fclose
#define fread     eLIBs_fread


/************************* stdio ***************************/

#define fgets            eLIBs_fgets
#define getchar          __getc

#define fprintf          epos_fprintf

/************************* time ****************************/

#define  time_t          int
#define  clock_t         int



#define   stdin    (ES_FILE *)1
//#define   stderr   (ES_FILE *)2


/********************* assert *******************************/

#define assert(exp)  { if (!(exp)) \
                      __err( "assertion failed \n"); }







#endif   // #ifdef EPDK_TEST_CUNIT





#include "epdk.h"


#endif   //#ifndef  __CLIB_H__
