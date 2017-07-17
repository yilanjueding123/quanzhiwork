/*
*********************************************************************************************************
*											        ePOS
*								the Easy Portable/Player Operation System
*									       board define file
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : board.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#include "const.h"

typedef enum __EPDK_BID_T
{
    //---------------------------------------------------------------------------------------------------
    //board id for 10
    //---------------------------------------------------------------------------------------------------
    EPDK_BID_P1             = (EPDK_SID_INT001_DEV | 1),        /* green board                                      */
    EPDK_BID_P2             = (EPDK_SID_INT001_DEV | 2),        /* red board                                        */

    EPDK_BID_PVP_V01        = (EPDK_SID_PVP001_001 | 1),        /* 800*480 5" ChiHsin lw500,K&D kd50g2              */
    EPDK_BID_PVP_V02        = (EPDK_SID_PVP001_001 | 2),        /* 800*480 4.3" tpo td043m                          */
    EPDK_BID_PVP_V03        = (EPDK_SID_PVP001_001 | 3),        /* 480*272, 4.3" InnoLux at043                      */
    EPDK_BID_PVP_V04        = (EPDK_SID_PVP001_001 | 4),        /* 800*480 5" ChiHsin lw500,K&D kd50g2 HDMI         */
    EPDK_BID_PVP_V05        = (EPDK_SID_PVP002_001 | 5),        /* 800*480 5" ChiHsin lw500, CMMB, tp, HDMI         */
    EPDK_BID_PVP_V06        = (EPDK_SID_PVP002_001 | 6),        /* 800*480 5" ChiHsin lw500, tp,                    */

    EPDK_BID_TVD_V01        = (EPDK_SID_PVP001_002 | 1),        /* tvd 176 hdmi                                     */
    EPDK_BID_TVD_V02        = (EPDK_SID_TVD001_001 | 2),        /* tvd 128                                          */
    EPDK_BID_TVD_V03        = (EPDK_SID_PVP001_002 | 3),        /* tvd 176 no hdmi, 3cvbs, uart                     */
    EPDK_BID_TVD_V04        = (EPDK_SID_PVP001_003 | 1),        /* tvd 176 nor,                                     */

    //---------------------------------------------------------------------------------------------------
    //board id for 15
    //---------------------------------------------------------------------------------------------------
    EPDK_BID_P3             = (EPDK_SID_INT101_DEV | 1),        /* dev board                                        */

    EPDK_BID_PVP_V101       = (EPDK_SID_PVP101_001 | 1),        /* 800*480 5" hdmi                                  */


} __epdk_bid_t;

#endif /* #ifndef _BOARD_H_ */
