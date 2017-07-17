/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Standby Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : standby_iokey.c
* By      : Alex.Leaf
* Version : v1.0
* Date    : 2010-7-8 19:21
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#include "ePDK.h"
#include "standby_spi.h"
#include "standby_iokey.h"


#if(ALLOW_USREVENT_WAKEUP_STANDBY)


static __u32    spi_group;


/*
*********************************************************************************************************
*                                   KEY BOARD WRITE REGISTER
*
* Description: key board write register.
*
* Arguments  : device
*              address
*              data
*
* Returns    : result
*
* Note       :
*********************************************************************************************************
*/
static __s32 KBWriteReg(unsigned char device,  unsigned char address, unsigned char data)
{
    __u8    senddata[2] = {0};

    senddata[0] = ((address & 0x3f) | device | 0x80);
    senddata[1] = data;

    if (EPDK_FAIL == standby_spi_txrx(spi_group, 2, senddata, 0, NULL))
    {
        return EPDK_FAIL;
    }
    else
    {
        return EPDK_OK;
    }
}


/*
*********************************************************************************************************
*                                   KEY BOARD READ REGISTER
*
* Description: key board read register.
*
* Arguments  : device
*              address
*
* Returns    : result
*
* Note       :
*********************************************************************************************************
*/
static __s32 KBReadReg(unsigned char device,  unsigned char address)
{
    __u8    senddata = (((address & 0x3f) | device) & 0x7f);
    __u8    readdata = 0;

    if (EPDK_FAIL == standby_spi_txrx(spi_group, 1, &senddata, 1, &readdata))
    {
        return EPDK_FAIL;
    }
    else
    {
        return readdata;
    }
}


/*
*********************************************************************************************************
*                                   KEY BOARD RESET
*
* Description: key board reset.
*
* Arguments  : none
*
* Returns    : result
*
* Note       :
*********************************************************************************************************
*/
static __s32 KBReset(void)
{
    __u8    senddata = 0xff;

    if (EPDK_FAIL == standby_spi_txrx(spi_group, 1, &senddata, 0, NULL))
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   KEY BOARD INITIATE
*
* Description: key board initiate.
*
* Arguments  : none
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
static void KBInitial(void)
{
    KBReset();                                      // Reset GPBA02

    //124569,10,12 input 对应PA0开始

    KBWriteReg( Device0, PA_Att, 0x00 );			// Device0,PA Output Buffer
    KBWriteReg( Device0, PA_Dir, 0xff );
    KBWriteReg( Device0, PA_Buf, 0xff );

    KBWriteReg( Device0, PB_Att, 0x00 );			// Device0,PB Output Buffer
    KBWriteReg( Device0, PB_Dir, (unsigned char )~((1 << 4) | ( 1 << 6) | ( 1 << 7) ));
    KBWriteReg( Device0, PB_Buf, 0xff );

    KBWriteReg( Device0, PC_Att, 0x00 );			// Device0,PC Pull Low
    KBWriteReg( Device0, PC_Dir, (unsigned char )~((1 << 1) | (1 << 2) | (1 << 3) | (1 << 6) | (1 << 7)) );
    KBWriteReg( Device0, PC_Buf, 0xff );
}


/*
*********************************************************************************************************
*                                   SPI KEY BOARD INITIATE
*
* Description: key board initiate.
*
* Arguments  : clk      current clock of ac320 run.
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void IOKEY_StandbyInit(__u32 clk)
{
    spi_group = SPI_GROUPB_2;

    //initial pin, hclk and controller
    standby_spi_init(spi_group);

    //config cs and mode
    standby_spi_cfg(spi_group, 0, 0, clk);

    //initial iokey
    KBInitial();
}


/*
*********************************************************************************************************
*                                   SPI KEY BOARD EXIT
*
* Description: key board exit.
*
* Arguments  : none
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void IOKEY_StandbyExit(void)
{
    standby_spi_exit(spi_group);
}


/*
*********************************************************************************************************
*                                   SPI KEY QUERY
*
* Description: qurery spi key board to dectect if any key is pressed.
*
* Arguments  : none
*
* Returns    : none
*
* Note       : result
*********************************************************************************************************
*/
__s32 IOKEY_Wakeup(void)
{
    __u8 datab, datac;

    //read reg
    datab = KBReadReg(Device0, PB_Data);
    datab &= 0xd0;
    datac = KBReadReg(Device0, PC_Data);
    datac &= 0xce;

    if (datab || datac)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}


#endif

