/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : robin_ab.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.27
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.09.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_ab_h
#define  __robin_ab_h


/***************************************************************************************************
*Name        : robin_set_ab_a
*Prototype   : __s32  robin_set_ab_a( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the A point in AB play mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_ab_a( void );

/***************************************************************************************************
*Name        : robin_set_ab_b
*Prototype   : __s32  robin_set_ab_b( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the B point in AB play mode and start AB play mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_ab_b( void );

/***************************************************************************************************
*Name        : robin_set_ab_loop_count
*Prototype   : __s32  robin_set_ab_loop_count( __u32 count )
*Arguments   : count     input. AB loop count.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the loop count in AB play mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_ab_loop_count( __u32 count );

/***************************************************************************************************
*Name        : robin_cancle_ab
*Prototype   : __s32  robin_cancle_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : cancle AB play.
*Other       :
***************************************************************************************************/
extern __s32  robin_cancle_ab( void );



#endif     //  ifndef __robin_ab_h

/* end of robin_ab.h */
