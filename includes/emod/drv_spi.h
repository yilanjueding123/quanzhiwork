/*
*********************************************************************************************************
*											        ePOS
*						           the Easy Portable/Player Operation System
*									           time sub-system
*
*                                    (c) Copyright 2007-2008, Jerry China
*                                          All Rights Reserved
*
*                                     module management headfile
*
* File   : drv_spi.h
* Version: V2.0
* By     : victor
*********************************************************************************************************
*/
#ifndef  _DRV_SPI_H_
#define  _DRV_SPI_H_


/* 说明：关于master 和 slave模式
 * 支持master模式；
 * 暂时不支持slave模式,用户通过ioctl设置及释放。
*/



/**************************************************
*用在work_mode参数
* spi work mode    Leading Edge        Trailing Edge
*
*       0   :      Rising, Sample      Falling, Setup
*
*       1   :      Rising, Setup       Falling, Sample
*
*       2   :      Falling, Sample     Rising, Setup
*
*       3   :      Falling, Setup      Rising, Sample
*
****************************************************/
#define  DRV_SPI_WORK_MODE0       0
#define  DRV_SPI_WORK_MODE1       1
#define  DRV_SPI_WORK_MODE2       2
#define  DRV_SPI_WORK_MODE3       3


/* 用在work_slot参数，选定特定的设备,用户需知道设备挂在哪一个spi插槽 */
#define  DRV_SPI_SLOT_0   (0x01) /* cs0 */
#define  DRV_SPI_SLOT_1   (0x02) /* cs1 */
#define  DRV_SPI_SLOT_2   (0x04) /* cs2 */
#define  DRV_SPI_SLOT_3   (0x08) /* cs3 */


//操作的时候，用户传进的数据结构
typedef  struct  __SPI_DEV_SET_XFER
{
    void   *tx_addr;        /*发送的首地址,如果没有置NULL*/
    __u32   tx_count;       /*发送数据字节数*/

    void   *rx_addr;        /*接收的首地址,如果没有置NULL*/
    __u32   rx_count;       /*接收数据字节数*/

    __u32   work_mode;      /*四种工作模式之一，pha和pol的组合，不是master或slave模式。*/
    __u32   work_clk;       /*单位：HZ，仅在master下有效；slave模式下不需要配置这个参数。*/
    __u32   work_slot;      /* spi设备在哪一个cs插槽 */
    void    *reserved;      /*保留，slave模式下可做回调。*/

} __spi_dev_set_xfer_t;

typedef enum __SPI_DEV_CMD_SET
{
    SPI_DEV_CMD_NONE                = 0x01,   //空命令
    SPI_DEV_CMD_MASTER_RW                 ,   /*master模式下的读写操作命令 */
    SPI_DEV_CMD_SLAVE_READ                ,   /*slave模式下，接收数据命令 */
    SPI_DEV_CMD_SLAVE_WRITE               ,   /*slave模式下，发送数据命令 */
    SPI_DEV_CMD_REQUEST_SLAVE             ,   /* master->slave模式的配置命令 */
    SPI_DEV_CMD_RELEASE_SLAE              ,   /* slave->master模式的配置命令 */
    SPI_DEV_CMD_RESERVED,
} __spi_dev_cmd_set_t;




/*SPI作为主控设备时候的用法说明*/
/************************************************************************************
*
*   关于spi作为master设备的使用:
*
*   1.  要使用spi设备，必须先申请一个设备文件，假如设备文件句柄是hspi，那么
*              hspi = eLIBs_fopen("b:\\BUS\\SPI0", "r+");
*       这里需要判断返回成功还是失败,用户需要知道设备挂载在哪一个spi总线上。
*
*   2.  得到文件句柄后，需要配置自己使用的相关参数，包括SPI工作模式，工作频率，片选插槽，
*         这些在数据结构__spi_dev_set_xfer_t中赋值，然后使用SPI_DEV_CMD_MASTER_RW传给驱动。
*          如果定义了一个__spi_dev_set_xfer_t类型的数据结构为spi_xfer，设置工作频率
*       为10MHz，工作模式是3，使用片选0， 那么
*                spi_xfer.work_slot = DRV_SPI_SLOT_0; //片选0,用户须知道设备接在哪一个片选上
*                spi_xfer.work_mode = DRV_SPI_WORK_MODE3; //pha = 1；pol=1；
*                spi_xfer.work_clk  = 10000000;//10MHz，每次读写必须配置
*       此外需要填充读写信息，有三种情况：
*             (1)如果为读则：
*               __u32 user_buf[10]={0};
*               spi_xfer.rx_addr = user_buf;
*               spi_xfer.rx_count = 10;
*               spi_xfer.tx_addr = NULL;
*               spi_xfer.tx_count = 0;
*             (2)如果为写则：
*               __u32 user_buf[10]={1};
*               spi_xfer.rx_addr = NULL;
*               spi_xfer.rx_count = 0;
*               spi_xfer.tx_addr = user_buf;
*               spi_xfer.tx_count = 10;
*             (3)如果先写后读则：(一般用在控制设备时先写命令，再接收数据)
*               __u32 rx_buf[10]={0};
*               __u32 tx_buf[10]={1};
*               spi_xfer.rx_addr = rx_buf;
*               spi_xfer.rx_count = 10;
*               spi_xfer.tx_addr = tx_buf;
*               spi_xfer.tx_count = 10;
*          使用下面的函数传给驱动
*                eLIBs_fioctrl(hspi, SPI_DEV_CMD_MASTER_RW, 0, (void *)&spi_xfer);
*
*    4.    需要释放申请的设备文件句柄
*                eLIBs_fclose(hspi);
*
*    以上是一个完整的SPI设备使用过程
*
**************************************************************************************/



#endif  /*_DRV_SPI_H_*/
