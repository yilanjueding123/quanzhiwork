/*
*********************************************************************************************************
*                                     eBase
*                           the Abstract of Hardware
*
*                   (c) Copyright 2006-2010, AW China
*                                  All Rights Reserved
*
* File           :  bsp_spi.h
* Date         :  2011-02-12
* Author      :  Victor
* Version     :  v1.00
* Description:
*               Operation for SPI module,aw1619
* History     :
*      <author>          <time>             <version >          <desc>
*       Victor         2011-02-12              1.0           create this file
*********************************************************************************************************
*/

#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_


/* user define call back function,ret */
/*1: slave模式下唤醒读线程，0:成功；-1:失败 -2:还没到传输过程就失败， */


/*
* 1.初始化时，可以将片选信号可以'|'或起来，表示开启哪一路片选；
*   比如: (1) 开启片选cs1,即 cs_bitmap = SPI_CHIP_SELECT_CS1;
*         (2) 开启cs0和cs1,即cs_bitmap = SPI_CHIP_SELECT_CS0|SPI_CHIP_SELECT_CS1;
*
* 读写时，SPI某一个时刻只能对一个片选有效，对chip_select赋值。
*   比如: 调用读写接口时，要指定CS2,chip_select = SPI_CHIP_SELECT_CS2;
*/
#define SPI_CHIP_SELECT_CS0 (0x01)
#define SPI_CHIP_SELECT_CS1 (0x02)
#define SPI_CHIP_SELECT_CS2 (0x04)
#define SPI_CHIP_SELECT_CS3 (0x08)


/* 设置config的bit位 */
/*
*       工作模式，包括4种：
*         0: 工作模式0，POL=0,PAL=0;
*         1: 工作模式1，POL=0,PAL=1;
*         2: 工作模式2，POL=1,PAL=0;
*         3: 工作模式3，POL=1,PAL=1;
*/
#define SPI_PHA_ACTIVE		    (0x01)
#define SPI_POL_ACTIVE		    (0x02)

#define SPI_MODE_0_ACTIVE		(0|0)
#define SPI_MODE_1_ACTIVE		(0|SPI_PHA_ACTIVE)
#define SPI_MODE_2_ACTIVE		(SPI_POL_ACTIVE|0)
#define SPI_MODE_3_ACTIVE		(SPI_POL_ACTIVE|SPI_PHA_ACTIVE) /*默认为模式3*/
/* 下面属性少用 */
#define SPI_CS_HIGH_ACTIVE		    (0x04)  /*默认为片选低电平有效，即低电平选中片选*/
#define SPI_LSB_FIRST_ACTIVE		(0x08)  /*默认为先发送MSB，即先发送最高位*/

#define SPI_DUMMY_ONE_ACTIVE        (0x10)  /*默认为接收时spi控制器自动填充0放在txFIFO */
#define SPI_RECEIVE_ALL_ACTIVE      (0x20)  /*默认为放弃无用的burst，即发送的时候放弃rxFIFO接收到数据 */

#define SPI_RAPID_MODE_ACTIVE       (0x40) /* 默认为一般通信速度，只有在>= 80MHz的时钟下才需要打开这个特性 */


/*调用初始化函数时使用*/
typedef struct aw_spi_info
{
    __s32 bus_num;
    __u32 master;//1:master;0:slave模式
    __u32 config; //POL&PHA; SS_HIGH,BIT_PER_WORD
    __u32 num_chipselect;// 1,2,3,4；20使用两个，开启多少个cs
    __u32 cs_bitmap;/*表示spi bus开启的片选位置 */
    __u32 bits_per_word;/* 0: 8 bit; 1:16 bit，默认为8 */
    __u32 spi_clk;/* spi clock,用户要求的时钟,默认为10Mhz */

    // 申请中断
    __u32 irq_no;
    __u32 irq_flags;
    __u8  irq_prio;

    /* 一般用法:
     *   a. poll_method = 1;dma_poll = 1;
     *   b. poll_method = 0;dma_poll = 0;
     *
     *   使用中断还是轮询方式传输 0-中断，1-轮询，没有注册中断函数;
     *   只有master模式才能使用轮询，slave模式必须借助中断完成传输。
     *
     *   一般来说，使用poll method=1搭配dma poll=1使用，
     *   使用场合:没有dma中断或者irq中断不可用。
    */
    __u32 poll_method;

    /* 对于小数据量来说，采用CPU传输方式(接收+轮询)，不可配置。
     *  对于大数据量传输，采用DMA配置的方式传输。
     *  其中，DMA可采用DMA+POLL或者DMA+dma callback
     *
     *  1: 使用poll不需要DMA回调函数，在当前函数完成DMA资源的释放；
     *  0：使用dma callback需要DMA回调函数，在回调函数完成DMA资源的释放。
     */
    __u32 dma_poll;/* dma传输+dma callback，dma传输+POLL */


    void (*wakeup_callback)(__s32 ret);//para:

} __spi_info_t;

/* 读写的时候需要配置的信息 */
typedef struct aw_spi_setting
{
    __s32 bus_num;//intial which num

    /*使用宏SPI_CHIP_SELECT_CSx赋值,可以SPI_CHIP_SELECT_CS0|SPI_CHIP_SELECT_CS1，表示使能了两个cs控制片选。*/
    __u32 chip_select;

    __u32 bits_per_word;/* 0: 8 bit; 1:16 bit */

    __u32 config;   /* POL和PHA,CS_HIGH_ACTIVE,LSB，DUMMY */
    __u32 spi_clk;/* spi clock,用户要求的时钟 */

} __spi_setting_t;



/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_init
*
*  Description:
*       采用默认的方式配置，使能SPI模块，设为master/slave模式，设置DHB/DDB/SMC，复位FIFO，
*   片选低电平有效SSPOL=0，burst期间保持低电平SSCTL=1，调用内部处理配置片选引脚，
*   设置时钟AHB。开启中断错误类型underrun/overflow，传输模式0，DRQ关等，最后申请SPI中断处理函数。
*
*  Parameters:
*       spi       :   外部的资源句柄
*  Return value:
*       初始化成功返回0，失败返回-1
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_SPI_init(__spi_info_t *spi);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_exit
*
*  Description:
*           完成SPI的退出清理功能，释放SPI资源句柄，关闭SPI。
*       取消注册SPI中断处理函数。把AHB时钟关闭，释放pin脚。
*
*  Parameters:
*       spi:  外部的资源句柄
*
*  Return value:
*         退出成功返回0，失败返回-1
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_SPI_exit(__spi_info_t *spi);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_master_txrx
*
*  Description:
*           master模式可调用，通过spi的资源句柄传入dma_used和dma_poll选择不同的传输方式，
*       内部设置BC/WTC。对于master模式来说，一般先发命令，即写操作，之后再读操作，结合使用；
*       DMA方式传输下有INT方式和POLL方式，CPU传输方式下有POLL方式，传输完成均由中断TC完成唤醒上层休眠线程,
*       或忙等程序。
*
*  Parameters:
*       spi:     外部的资源句柄
*       tx_buf:  发送buffer首地址
*       tx_len:  发送数据长度
*       rx_buf:  接收buffer首地址
*       rx_len:  接收数据长度
*  Return value:
*       成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*       传输过程中，始终保持片选。SMC模式，自动填充dummy。借助chipselect，每传输完一个transfer，根据
*   传输的需求是否保持片选。外部
****************************************************************************************************
*/
__s32 BSP_SPI_master_txrx (__spi_setting_t *spi, void *tx_buf, u32 tx_len, void *rx_buf, u32 rx_len);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_master_waitclk
*
*  Description:
*        设置等待时钟数目， 在每个data transfer 中，插入N个等待状态，spi模块计数SPI_SCLK作为N个WCC，
*         对下一个data transfer作延迟。
*
*  Parameters:
*         bus_num: spi的寄存器组句柄
*         waitclk_cnt:  插入的等待时钟数目
*
*  Return value:
*         成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*       仅仅在master模式下有效。
****************************************************************************************************
*/
__s32 BSP_SPI_master_waitclk(__s32 bus_num, __u32 waitclk_cnt);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_master_sampledly
*
*  Description:
*        设置采样延时，高速传输下使用。
*
*  Parameters:
*         bus_num: spi的寄存器组句柄
*         on_off:  1-打开，0-关闭
*
*  Return value:
*         成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*       仅仅在master下有效。
****************************************************************************************************
*/
__s32 BSP_SPI_master_sampledly(__s32 bus_num, __u32 on_off);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_set_mode
*
*  Description:
*         设置模式主从模式。
*
*  Parameters:
*         bus_num: spi的寄存器组句柄
*         master: master/slave模式，0：slave；1：master
*
*  Return value:
*         成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_SPI_set_mode(__s32 bus_num, __u32 master);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_config
*
*  Description:
*         实际上调用发送或接收接口时，已经设置了下面的所有属性。
*      master:可配置,模式-POL,PHA,片选极性-SSPOL,传输bit为开始-LMTF,自动填充0/1-DDB,放弃无效burst-DHB.
*      slave : POL,PHA,SSPOL,LMTF,
*
*  Parameters:
*       bus_num：  spi的总线号
*       bitmap:   设置参数，NULL表示保持默认值。
*
*  Return value:
*       成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*           每次传输过程中，已经设置了此接口的参数。
****************************************************************************************************
*/
__s32 BSP_SPI_config(__s32 bus_num, __u32 bitmap);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_ss_ctrl
*
*  Description:
*         手工控制chip select信号，enable为1：用户控制片选高低电平；0：SPI控制器自动控制SS电平。
*     用户调用完后要将其disable。当master模式下，出现自动片选信号无效时采用此接口，存在于19/20芯片中。
*
*  Parameters:
*       bus_num:   spi的总线号
*       on_off: 使能参数，0: disable,1:enable.
*
*  Return value:
*          成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*   注意事项
****************************************************************************************************
*/
__s32 BSP_SPI_cs_ctrl(__s32 bus_num, __u32 on_off);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_ss_level
*
*  Description:
*         用户控制片选的高低电平。只有BSP_SPI_ctrl将片选设定为用户控制后使用这个函数才有意义。
*     当master模式下，出现自动片选信号无效时采用此接口，存在于19/20芯片中。
*    (       __u32 cs_change;
*       表示传输过程中，是否始终保持片选,0表示保持片选，如果跟之前一次相同则保持片选，
*       实现的时候借助20的手动控制片选，在下一个transfer的时候如果还是片选的芯片，则仍保持片选 .)
*  Parameters:
*         hSPI:  spi的寄存器组句柄；
*         hi_lo：设置高低电平，0: 低电平；1：高电平。
*
*  Return value:
*           成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*
****************************************************************************************************
*/
__s32 BSP_SPI_cs_level(__s32 bus_num, __u32 hi_lo);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_slave_tx
*
*  Description:
*                slave模式可调用，通过spi的资源句柄传入dma_used和dma_poll选择不同的传输方式。
*        对于slave模式来说，一般先收到命令，即读操作，之后再写操作；采用txFIFO empty作为传输完成中断。
*       DMA方式传输下有INT方式和POLL方式，CPU传输方式下有POLL方式，传输完成由中断TE完成唤醒上层休眠
*       线程或忙等程序。发送过程中，先关掉RR。发送什么数据，是由上层决定的。
*       出错处理: 开启TO。
*
*  Parameters:
*       spi:     外部的资源句柄
*       tx_buf:  发送buffer首地址
*       tx_len:  发送数据长度
*
*  Return value:
*       成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*       slave没有等待轮询方式。
****************************************************************************************************
*/
__s32 BSP_SPI_slave_tx(__spi_setting_t *spi, void *tx_buf, __u32 tx_len);

/*
****************************************************************************************************
*
*  FunctionName:           BSP_SPI_slave_rx
*
*  Description:
*           slave模式可调用，通过spi的资源句柄传入dma_used和dma_poll选择不同的传输方式。
*       对于slave模式来说，一般先收到命令，即读操作，之后再写操作；采用rxFIFO ready作为接收开始中断，
*       调用上层接收时，屏蔽这个RR中断，接收完成再开启RR中断，注意接收时要对txFIFO填充数据，不然出现txFIFO
*       underrun错误（可以考虑关闭这个中断），接收处理不过来会出现rxFIFO overflow。
*       DMA方式传输下有INT方式和POLL方式，CPU传输方式下有POLL方式，传输开始由中断RR唤醒上层休眠线程。
*       接收流程: RR-->关闭RR-->调用本接口接收-->根据接收的命令解析-->发送数据，或继续接收数据。
*
*  Parameters:
*       spi:     外部的资源句柄
*       rx_buf:  接收buffer首地址
*       rx_len:  接收数据长度
*
*  Return value:
*       成功返回EBSP_TRUE，失败返回EBSP_FALSE。
*  Notes:
*       slave没有等待轮询方式。
****************************************************************************************************
*/
__s32 BSP_SPI_slave_rx(__spi_setting_t *spi, void *rx_buf, u32 rx_len);



#endif
