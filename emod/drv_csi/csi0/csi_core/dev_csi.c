/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									           ir key driver
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : dev_csi.c
* By      : jshwang
* Version : V1.00
*********************************************************************************************************
*/

#include "dev_csi_i.h"

__csi_dev_t     *pCsiDevNode = NULL;

ES_FILE   *fiic = NULL;
__hdle   csi_outclk = 0;//mclk配置句柄
__hdle   csi_outclk_1 = 0;//mclk配置句柄

__u32		first_flag = 0;
__u32		pts = 0;
__u32		valid_mode_idx = 0;
__s32   lost_frame_num = 0;

static	__s32 frame_interval = 0 ;
static	__s32 skip_counter = 0 ;

#if CSI_PORT==0
__u32 csi0_tdm_buf_offset_ch1[3] = {0, 0, 0};
__u32 csi0_tdm_buf_offset_ch2[3] = {0, 0, 0};
__u32 csi0_tdm_buf_offset_ch3[3] = {0, 0, 0};
#endif

//#define WRITE_DATA_TO_FILE
/*******************************************************************************
Function name: generate_frame_id
Description:
    1。只用低8bit，因为display_info的top_index是__u8类型
Parameters:

Return:

Time: 2010/7/13
*******************************************************************************/
//__s32 generate_frame_id(__s32 index)
//{
//    __s32 prefix = capture_on_times%16;
//    __s32 frame_id;
//    frame_id = (prefix<<4 | index);
//    return frame_id;
//}
__s32 translate_frame_id_to_index(__s32 frame_id)
{
    //__s32 index;
    return (frame_id & 0x0F);
}

__s32 verify_frame_id(__s32 frame_id)
{
    //__s32 prefix = capture_on_times%16;
    __s32 prefix = (pCsiDevNode->csi_frame[0].frame_id >> 4) & 0x0F;
    __s32 frame_prefix = (frame_id >> 4) & 0x0F;
    if(prefix != frame_prefix)
    {
        __wrn("frameid[%x] changed, check: prev prefix[%x], cur[%x]\n", frame_id, frame_prefix, prefix);
        return EPDK_FAIL;
    }
    else
    {
        return EPDK_OK;
    }
}

#if CSI_PORT==0
/*******************************************************************************
Function name: cal_tdm_addr
Description:
    1. calculate the offset address for CSI0 TMD multi-channel-receiving
Parameters:

Return:

Time: 2011-8-11
*******************************************************************************/
__s32 cal_tdm_addr(__buf_scrab_mode_t buf_mode)
{
    __s32 cal_ret = 1;

    switch( buf_mode )
    {
    case CSI0_FULL:
    {
        cal_ret = EPDK_FAIL;
        break;
    }//case CSI0_FULL end//

    case CSI0_TDM_2CH_2X1:     //be careful of this LR type
    {
        if (csi_mode.store_mode == YUV_MOD_UV_NON_MB_COMBINED )
        {
            if( csi_mode.color_format == PIXEL_YUV422)
            {
                csi0_tdm_buf_offset_ch1[0] = csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch1[1] = csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch1[2] = 0;//no use
            }

            else if( csi_mode.color_format == PIXEL_YUV420)
            {
                csi0_tdm_buf_offset_ch1[0] = csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch1[1] = csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch1[2] = 0;//no use
                __inf("TDM_2X1 420\n");
            }
        }

        else
        {
            csi0_tdm_buf_offset_ch1[0] = csi_mode.size.width / 2;
            csi0_tdm_buf_offset_ch1[1] = csi_mode.size.width / 4;
            csi0_tdm_buf_offset_ch1[2] = csi_mode.size.width / 4;
        }

        cal_ret = EPDK_OK;
        break;
    }//case CSI0_TDM_2CH_2X1 end//

    case CSI0_TDM_4CH_2X2:
    {
        if (csi_mode.store_mode == YUV_MOD_UV_NON_MB_COMBINED )
        {
            if( csi_mode.color_format == PIXEL_YUV422)
            {
                csi0_tdm_buf_offset_ch1[0] = csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch1[1] = csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch1[2] = 0;//no use

                csi0_tdm_buf_offset_ch2[0] = csi_mode.size.width * csi_mode.size.height / 2;
                csi0_tdm_buf_offset_ch2[1] = csi_mode.size.width * csi_mode.size.height / 2;
                csi0_tdm_buf_offset_ch2[2] = 0;//no use

                csi0_tdm_buf_offset_ch3[0] = csi_mode.size.width * csi_mode.size.height / 2 + csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch3[1] = csi_mode.size.width * csi_mode.size.height / 2 + csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch3[2] = 0;//no use
            }

            else  //if( csi_mode.color_format == PIXEL_YUV420) {
            {
                csi0_tdm_buf_offset_ch1[0] = csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch1[1] = csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch1[2] = 0;//no use

                csi0_tdm_buf_offset_ch2[0] = csi_mode.size.width * csi_mode.size.height / 2;
                csi0_tdm_buf_offset_ch2[1] = csi_mode.size.width * csi_mode.size.height / 4;
                csi0_tdm_buf_offset_ch2[2] = 0;//no use

                csi0_tdm_buf_offset_ch3[0] = csi_mode.size.width * csi_mode.size.height / 2 + csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch3[1] = csi_mode.size.width * csi_mode.size.height / 4 + csi_mode.size.width / 2;
                csi0_tdm_buf_offset_ch3[2] = 0;//no use
                __inf("TDM_2X2 420\n");
            }
        }

        else if (csi_mode.store_mode == YUV_MOD_NON_MB_PLANAR )
        {
            if( csi_mode.color_format == PIXEL_YUV422)
            {
                csi0_tdm_buf_offset_ch1[0] = csi_mode.size.width * csi_mode.size.height / 2;
                csi0_tdm_buf_offset_ch1[1] = csi_mode.size.width * csi_mode.size.height / 4;
                csi0_tdm_buf_offset_ch1[2] = csi_mode.size.width * csi_mode.size.height / 4;
            }

            else  //if( csi_mode.color_format == PIXEL_YUV420) {
            {
                csi0_tdm_buf_offset_ch1[0] = csi_mode.size.width * csi_mode.size.height / 2;
                csi0_tdm_buf_offset_ch1[1] = csi_mode.size.width * csi_mode.size.height / 8;
                csi0_tdm_buf_offset_ch1[2] = csi_mode.size.width * csi_mode.size.height / 8;
            }
        }

        else
        {
            csi0_tdm_buf_offset_ch1[0] = csi_mode.size.width * csi_mode.size.height / 2;
            csi0_tdm_buf_offset_ch1[1] = csi_mode.size.width * csi_mode.size.height / 2;
            csi0_tdm_buf_offset_ch1[2] = csi_mode.size.width * csi_mode.size.height / 2;
        }
        cal_ret = EPDK_OK;
        break;
    }//case CSI0_TDM_4CH_2X2 end//


    default: //same with CSI0_FULL
    {
        cal_ret = EPDK_FAIL;
        break;
    }

    }
    //__msg("return =%d\n",cal_ret);

    return cal_ret;
}
#endif

static __s32  csi_irq_handle(void *arg)
{
    __s32 frame_id;//index切切不要用全局变量
    __u32 reg_val;

    //__inf("csi[%d] irq\n",CSI_PORT);
    if(first_flag == 0)
    {
        first_flag = 1;
        __inf("csi[%d] first irq\n", CSI_PORT);
        frame_id = pCsiDevNode->free2->delete_element(pCsiDevNode->free2);
        if (frame_id != -1)
        {
            //在free中请求buffer成功

            __s32 index = translate_frame_id_to_index(frame_id);

            CSI_set_base((CSI_PORT == 0) ? 0x00 : 0x01);
            CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[0]));
            CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[1]));
            CSI_set_buffer_address(CSI_BUF_2_A,  0);

#if CSI_PORT==0
            if( csi_mode.csi_buf_scrab_mode == CSI0_TDM_2CH_2X1 || csi_mode.csi_buf_scrab_mode == CSI0_TDM_4CH_2X2 )
            {
                CSI_set_base(0x10);
                CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[0]) + csi0_tdm_buf_offset_ch1[0]);
                CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[1]) + csi0_tdm_buf_offset_ch1[1]);

                CSI_set_base(0x20);
                CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[0]) + csi0_tdm_buf_offset_ch2[0]);
                CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[1]) + csi0_tdm_buf_offset_ch2[1]);


                CSI_set_base(0x30);
                CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[0]) + csi0_tdm_buf_offset_ch3[0]);
                CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[1]) + csi0_tdm_buf_offset_ch3[1]);

                CSI_set_base(0x00);
            }
#endif

            pCsiDevNode->frame_id_last_b = frame_id;

        }
        else
        {
            __wrn("csi[%d] no buffer!\n", CSI_PORT);
        }

        goto done;
    }

    if (pCsiDevNode->pts_flag == 0)
    {
        pCsiDevNode->pts += (__s64)(csi_mode.frame_period);
    }
    else if(pCsiDevNode->pts_flag == 1)
    {
        if(pCsiDevNode->CB_GetPts)
        {
            csi_CallBack(pCsiDevNode->CB_GetPts, (void *)&pCsiDevNode->pts);

            if((__u32)(pCsiDevNode->pts) - pts > csi_mode_all.csi_mode[valid_mode_idx].frame_period * 1.2)
                __wrn("frame period not match, delta = %d\n", (__u32)(pCsiDevNode->pts) - pts);
            pts = (__u32)(pCsiDevNode->pts);
        }
        else
        {
            //__wrn("CB_GetPts callback is not set!\n");
            pCsiDevNode->pts = 0;
        }
    }
    else
    {
        pCsiDevNode->pts = 0;
    }

    if( skip_counter < frame_interval )
    {

        skip_counter++ ;
        goto done;
    }
    skip_counter = 0 ;
    //__log("pCsiDevNode->pts_flag=%d pCsiDevNode->pts=%d\n",pCsiDevNode->pts_flag,(pCsiDevNode->pts&0xffffffff));
    frame_id = pCsiDevNode->free2->delete_element(pCsiDevNode->free2);
    //    __msg("frame_id=%x\n",frame_id);
    if (frame_id != -1)//在free中请求buffer成功
    {
        __s32 nTmpIndex = translate_frame_id_to_index(frame_id);
        //__inf("frame_id=%d\n",nTmpIndex);
        CSI_set_base((CSI_PORT == 0) ? 0x00 : 0x01);
        //__msg("vaddr0=%x vaddr1=%x vaddr2=%x\n",(__u32)(pCsiDevNode->csi_frame[nTmpIndex].addr[0]),(__u32)(pCsiDevNode->csi_frame[nTmpIndex].addr[1]),(__u32)(pCsiDevNode->csi_frame[nTmpIndex].addr[2]));
        CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[0]));
        CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[1]));
        CSI_set_buffer_address(CSI_BUF_2_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[2]));
        //__msg("paddr0=%x paddr1=%x paddr2=%x\n",(__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[0]),(__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[1]),(__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[2]));
#if CSI_PORT==0
        if( csi_mode.csi_buf_scrab_mode == CSI0_TDM_2CH_2X1 || csi_mode.csi_buf_scrab_mode == CSI0_TDM_4CH_2X2 )
        {
            CSI_set_base(0x10);
            CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[0]) + csi0_tdm_buf_offset_ch1[0]);
            CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[1]) + csi0_tdm_buf_offset_ch1[1]);

            CSI_set_base(0x20);
            CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[0]) + csi0_tdm_buf_offset_ch2[0]);
            CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[1]) + csi0_tdm_buf_offset_ch2[1]);


            CSI_set_base(0x30);
            CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[0]) + csi0_tdm_buf_offset_ch3[0]);
            CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[nTmpIndex].addr[1]) + csi0_tdm_buf_offset_ch3[1]);

            CSI_set_base(0x00);
        }
#endif

        nTmpIndex = translate_frame_id_to_index(pCsiDevNode->frame_id_last);
        pCsiDevNode->csi_frame[nTmpIndex].uPts = pCsiDevNode->pts;//

        pCsiDevNode->full2->insert_element(pCsiDevNode->full2, pCsiDevNode->frame_id_last);
        pCsiDevNode->frame_id_last = pCsiDevNode->frame_id_last_b;
        pCsiDevNode->frame_id_last_b = frame_id;
    }
    else
    {
        __wrn("****csi[%d] lost frame[%d] ****\n", CSI_PORT, lost_frame_num++);
    }

    reg_val = *((volatile __u32 *)(0xf1cb0000 + 0x34) );
    if(reg_val != 0x82)
        eLIBs_printf("warming CSI[%d] FIFO error! reg=%x\n", CSI_PORT, reg_val);

done:
    CSI_set_base((CSI_PORT == 0) ? 0x00 : 0x01);
    CSI_int_clear_status();

    return EPDK_OK;
}



__s32 csi_check_mode_valid(void)
{
    __u32 i;
    for (i = 0; i < csi_mode_all.number; i++)
    {
        __inf("mode%d:%dx%d@%d#color_fmt=%d buf_scmode=%d\n", i, csi_mode_all.csi_mode[i].size.width, csi_mode_all.csi_mode[i].size.height, csi_mode_all.csi_mode[i].frame_rate, (__u32)csi_mode_all.csi_mode[i].color_format, csi_mode_all.csi_mode[i].csi_buf_scrab_mode);
        if ((csi_mode_all.csi_mode[i].color_format == csi_mode.color_format) &&
                (csi_mode_all.csi_mode[i].color_space == csi_mode.color_space) &&
                (csi_mode_all.csi_mode[i].component_seq == csi_mode.component_seq) &&
                (csi_mode_all.csi_mode[i].frame_rate == csi_mode.frame_rate) &&
                (csi_mode_all.csi_mode[i].frame_period == csi_mode.frame_period) &&
                (csi_mode_all.csi_mode[i].store_mode == csi_mode.store_mode) &&
                (csi_mode_all.csi_mode[i].size.width == csi_mode.size.width) &&
                (csi_mode_all.csi_mode[i].size.height   == csi_mode.size.height) &&
                (csi_mode_all.csi_mode[i].csi_buf_scrab_mode    == csi_mode.csi_buf_scrab_mode) )
        {
            __inf("csi[%d]_check_mode_valid:%d\n", CSI_PORT, i);
            //csi_dev_set_mode(i); //设置模式
            valid_mode_idx = i;
            return EPDK_OK;
        }
    }
    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               FUNCTION
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void CSI_OpLock(void)
{
    __u8  err;

    csi_SemPend(pCsiDevNode->csi_lock, 0, &err);
}
/*
**********************************************************************************************************************
*                                               FUNCTION
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void CSI_OpUnlock(void)
{
    csi_SemPost(pCsiDevNode->csi_lock);
}
/*
****************************************************************************************************
*
*             DEV_CSI_Open
*
*  Description:
*       DEV_CSI_Open
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__hdle DEV_CSI_Open(void *open_arg, __u32 mode)
{
    __inf("DEV_CSI_Open[%d]\n", CSI_PORT);
    if(!pCsiDevNode->iic_file)
    {
        __wrn("pCsiDevNode->iic_file = NULL\n");
        return (__hdle)0;
    }
    else
    {
        return (__hdle) & (pCsiDevNode);
    }
}
/*
****************************************************************************************************
*
*             DEV_CSI_Close
*
*  Description:
*       DEV_CSI_Close
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DEV_CSI_Close(__hdle hcsi)
{
    __inf("DEV_CSI_Close[%d]\n", CSI_PORT);
    //fiic = NULL;

    return EPDK_OK;
}
/*
****************************************************************************************************
*
*             DEV_CSI_Read
*
*  Description:
*       DEV_CSI_Read
*
*  Parameters:
*
*  Return value:
*       size*n
*
****************************************************************************************************
*/
__u32 DEV_CSI_Read(void *pdata, __u32 size, __u32 n, __hdle hPower)
{
    return 0;
}
/*
****************************************************************************************************
*
*             DEV_CSI_Write
*
*  Description:
*       DEV_CSI_Write
*
*  Parameters:
*
*  Return value:
*       size*n
*
****************************************************************************************************
*/
__u32 DEV_CSI_Write(const void *pdata, __u32 size, __u32 n, __hdle hPower)
{
    return size * n;
}
/*
****************************************************************************************************
*
*             DEV_CSI_Ioctrl
*
*  Description:
*       DEV_CSI_Ioctrl
*
*  Parameters:
*
*  Return value:
*       EPDK_OK
*       EPDK_FAIL
****************************************************************************************************
*/
__s32 DEV_CSI_Ioctrl(__hdle hPower, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32                ret;
    __s32 i;

    //__inf("DEV_CSI_Ioctrl\n");

    ret = EPDK_FAIL;

    if(!pCsiDevNode->iic_file)
    {
        return ret;
    }

    CSI_OpLock();

    switch(cmd)
    {
    case DRV_CSI_CMD_INIT:
    {
        __inf("csi[%d] buffer number = %d\n", CSI_PORT, CSI_BUFFER_NUM);
        for (i = 0; i < CSI_BUFFER_NUM; i++)
            pCsiDevNode->p_csi_frame[i] = &pCsiDevNode->csi_frame[i];

        CSI_init();
        CSI_set_base((CSI_PORT == 0) ? 0x00 : 0x01);
        CSI_scale_down(CSI_ORIGIN, CSI_ORIGIN);
        CSI_int_enable((__csi_int_t)(CSI_INT_VSYNC_TRIG));
        CSI_open();

        csi_dev_init();
        esINT_InsISR(INTC_IRQNO_CSI, (__pISR_t)csi_irq_handle, 0);
        esINT_EnableINT(INTC_IRQNO_CSI);

        __inf("DRV_CSI_CMD_INIT[%d]\n", CSI_PORT);
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_EXIT:
    {
        csi_dev_exit();

        esINT_UniISR(INTC_IRQNO_CSI, (__pISR_t)csi_irq_handle);
        esINT_DisableINT(INTC_IRQNO_CSI);

        CSI_set_base((CSI_PORT == 0) ? 0x00 : 0x01);
        CSI_close();
        CSI_exit();

        __inf("DRV_CSI_CMD_EXIT[%d]\n", CSI_PORT);
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_SET_MODE:
    {
        __csi_mode_t *p = pbuffer;
        csi_mode = *p;

        __inf("DRV_CSI_CMD_SET_MODE[%d]\n", CSI_PORT);
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_GET_MODE:
    {
        __csi_mode_t *p = pbuffer;
        *p = csi_mode;

        __inf("DRV_CSI_CMD_GET_MODE[%d]\n", CSI_PORT);
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_GET_MODE_ALL:
    {
        __csi_mode_all_t *p = pbuffer;
        *p = csi_mode_all;

        __inf("DRV_CSI_CMD_GET_MODE_ALL[%d]\n", CSI_PORT);
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_SET_FRAME_QUEUE:
    {
        __csi_frame_queue_t *pFrameQueue = (__csi_frame_queue_t *)pbuffer;

        if(pFrameQueue->num != CSI_BUFFER_NUM)
        {
            __wrn("careful! num[%d] != CSI_BUFFER_NUM[%d]\n", pFrameQueue->num, CSI_BUFFER_NUM);
            //return EPDK_FAIL;
        }
        else
        {
            __wrn("csi_frame_num[%d]\n", pFrameQueue->num);
        }
        if(pFrameQueue->num > CSI_BUFFER_NUM)
        {
            __wrn("fatal error, num[%d] > CSI_BUFFER_NUM[%d]\n", pFrameQueue->num, CSI_BUFFER_NUM);
            return EPDK_FAIL;
        }
        pCsiDevNode->csi_frame_num = pFrameQueue->num;
        for(i = 0; i < pFrameQueue->num; i++) //现在num就等于CSI_BUFFER_NUM，为下一步修改做准备
        {
            //CSI_BUFFER_NUM
            csi_memcpy(&pCsiDevNode->csi_frame[i], &pFrameQueue->csi_frame_array[i], sizeof(__csi_frame_t));
        }
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_CAPTURE_ON:
    {
        __s32 frame_id;
        __s32 index;//index切切不要用全局变量

        if (csi_check_mode_valid() != EPDK_OK)
        {
            __wrn("csi[%d] mode invalid!\n", CSI_PORT);
            break;
        }
        else
        {
            csi_dev_set_mode(valid_mode_idx); //设置模式
        }

        pCsiDevNode->pts = 0;

        pCsiDevNode->full2->initial(pCsiDevNode->full2, CSI_LISTTYPE_FULL);
        pCsiDevNode->free2->initial(pCsiDevNode->free2, CSI_LISTTYPE_FREE);

        for (i = 0; i < pCsiDevNode->csi_frame_num; i++)
        {
            pCsiDevNode->csi_frame[i].bProgressiveSrc   = bProgressiveSrc;
            pCsiDevNode->csi_frame[i].bTopFieldFirst    = bTopFieldFirst;
            pCsiDevNode->csi_frame[i].eAspectRatio      = eAspectRatio;
            pCsiDevNode->csi_frame[i].color_format      = csi_mode.color_format;
            pCsiDevNode->csi_frame[i].src_rect.width    = pCsiDevNode->csi_frame[i].dst_rect.width = csi_mode.size.width;
            pCsiDevNode->csi_frame[i].src_rect.height   = pCsiDevNode->csi_frame[i].dst_rect.height = csi_mode.size.height;
            pCsiDevNode->csi_frame[i].frame_rate        = csi_mode.frame_rate;
            pCsiDevNode->csi_frame[i].uPts              = pCsiDevNode->pts;
            pCsiDevNode->csi_frame[i].bPtsValid         = 1;
            pCsiDevNode->csi_frame[i].component_seq     = csi_mode.component_seq;;
            pCsiDevNode->csi_frame[i].store_mode        = csi_mode.store_mode;;

            if(EPDK_OK != pCsiDevNode->free2->insert_element(pCsiDevNode->free2, pCsiDevNode->csi_frame[i].frame_id))
            {
                __wrn("impossible free2_insert fail,check code![%d]\n", CSI_PORT);
            }
        }
        pCsiDevNode->csi_mode_last = csi_mode;

#if CSI_PORT==0
        if( cal_tdm_addr(csi_mode.csi_buf_scrab_mode) == EPDK_OK )
        {
            __inf("csi0_ch1 offset[0,1,2]=%x, %x, %x\n", csi0_tdm_buf_offset_ch1[0], csi0_tdm_buf_offset_ch1[1], csi0_tdm_buf_offset_ch1[2]);
            __inf("csi0_ch2 offset[0,1,2]=%x, %x, %x\n", csi0_tdm_buf_offset_ch2[0], csi0_tdm_buf_offset_ch2[1], csi0_tdm_buf_offset_ch2[2]);
            __inf("csi0_ch3 offset[0,1,2]=%x, %x, %x\n", csi0_tdm_buf_offset_ch3[0], csi0_tdm_buf_offset_ch3[1], csi0_tdm_buf_offset_ch3[2]);

        }
#endif

        frame_id = pCsiDevNode->free2->delete_element(pCsiDevNode->free2);
        if (frame_id != -1)
        {
            //在free中请求buffer成功
            index = translate_frame_id_to_index(frame_id);
            CSI_set_base((CSI_PORT == 0) ? 0x00 : 0x01);
            CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[0]));
            CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[1]));
            CSI_set_buffer_address(CSI_BUF_2_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[2]));

#if CSI_PORT==0
            if( csi_mode.csi_buf_scrab_mode == CSI0_TDM_2CH_2X1 || csi_mode.csi_buf_scrab_mode == CSI0_TDM_4CH_2X2 )
            {
                CSI_set_base(0x10);
                CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[0]) + csi0_tdm_buf_offset_ch1[0]);
                CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[1]) + csi0_tdm_buf_offset_ch1[1]);
                CSI_set_buffer_address(CSI_BUF_2_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[2]) + csi0_tdm_buf_offset_ch1[2]);

                CSI_set_base(0x20);
                CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[0]) + csi0_tdm_buf_offset_ch2[0]);
                CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[1]) + csi0_tdm_buf_offset_ch2[1]);
                CSI_set_buffer_address(CSI_BUF_2_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[2]) + csi0_tdm_buf_offset_ch2[2]);

                CSI_set_base(0x30);
                CSI_set_buffer_address(CSI_BUF_0_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[0]) + csi0_tdm_buf_offset_ch3[0]);
                CSI_set_buffer_address(CSI_BUF_1_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[1]) + csi0_tdm_buf_offset_ch3[1]);
                CSI_set_buffer_address(CSI_BUF_2_A, (__u32)esMEMS_VA2PA((void *)pCsiDevNode->csi_frame[index].addr[2]) + csi0_tdm_buf_offset_ch3[2]);

                CSI_set_base(0x00);
            }
#endif
            pCsiDevNode->frame_id_last = frame_id;

        }
        else
        {
            __wrn("csi[%d] no buffer!\n", CSI_PORT);
        }

        CSI_set_base((CSI_PORT == 0) ? 0x00 : 0x01);
        CSI_capture_video_start();
        //            if(aux==0)//连续抓拍
        //            {
        //
        //            }
        //            else if(aux>0)//抓拍张数等于aux
        //            {
        //
        //            }
        //			CSI_int_enable((__csi_int_t)(CSI_INT_FRAME_DONE));

        __inf("DRV_CSI_CMD_CAPTURE_ON[%d]\n", CSI_PORT);
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_CAPTURE_OFF:
    {
        //            __csi_capture_status status;
        CSI_set_base((CSI_PORT == 0) ? 0x00 : 0x01);
        CSI_capture_video_stop();//关中断发生器
        first_flag = 0;
        lost_frame_num = 0;
        //        	do
        //        	{
        //        	    CSI_capture_get_status(&status);
        //
        //        	    __inf("video_in_progress=%d\n",status.video_in_progress);
        //        	}while(status.video_in_progress);//等最后一个中断发生。此后再不会有中断来了，但不能保证该中断的中断处理程序被调用。
        __inf("DRV_CSI_CMD_CAPTURE_OFF[%d]\n", CSI_PORT);
        ret = EPDK_OK;
        esKRNL_TimeDly(5);  //确保最后一个中断处理程序被执行。所以delay 50ms

        //清掉csi_frame数组，即full2, free2数组，也可以不清. 为简单起见,这里不清数组.
        //initial_csi_linklist_manager(&full2, CSI_LISTTYPE_FULL);
        //initial_csi_linklist_manager(&free2, CSI_LISTTYPE_FREE);
        pCsiDevNode->full2->initial(pCsiDevNode->full2, CSI_LISTTYPE_FULL);
        pCsiDevNode->free2->initial(pCsiDevNode->free2, CSI_LISTTYPE_FREE);
        break;
    }
    case DRV_CSI_CMD_SET_PTS:
    {
        pCsiDevNode->pts = aux;
        __inf("DRV_CSI_CMD_SET_PTS[%d]\n", CSI_PORT);
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_GET_PTS:
    {
        ret = pCsiDevNode->pts;
        __inf("DRV_CSI_CMD_GET_PTS[%d]\n", CSI_PORT);
        //ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_REQUEST_FRAME:
    {
        __s32 index;
        __s32 frame_id;//index切切不要用全局变量

#ifdef WRITE_DATA_TO_FILE
        static __s32 frame_counter = 0, has_write_file = 0;
        frame_counter++;

#endif
        //__inf("requesting csi frame\n");

        //index = linklist_delete(&full);
        //frame_id = full2_delete();
        frame_id = pCsiDevNode->full2->delete_element(pCsiDevNode->full2);
        if (frame_id != -1)
        {
            __csi_frame_t **pp = (__csi_frame_t ** )pbuffer;
            index = translate_frame_id_to_index(frame_id);
            *pp = pCsiDevNode->p_csi_frame[index];

            //  __inf("frame pointer %x\n",*pp);

            //ERIC_MSG("[%x]\n", (__s32)(*pp)->uPts/1000);
            //                frame_count++;
            //                __inf("request index: %d, frame_count=%d\n", index, frame_count);

            //  __inf("requesting csi frame ok: #%d\n",index);

#ifdef WRITE_DATA_TO_FILE
            if( (frame_counter >= 200) && ( has_write_file == 0) )
            {
                ES_FILE *fp ;
                fp = eLIBs_fopen("f:\\yuv420uvcombine-y.bin", "w") ;
                eLIBs_printf("f:yuv420uvcombine-y.binfp=%x\n", fp);
                if(fp)
                {
                    eLIBs_fwrite((void *)pCsiDevNode->p_csi_frame[index]->addr[0], pCsiDevNode->p_csi_frame[index]->src_rect.width * pCsiDevNode->p_csi_frame[index]->src_rect.height, 1, fp);
                    eLIBs_fclose( fp ) ;
                }
                fp = eLIBs_fopen("f:\\yuv420uvcombine-uv.bin", "w") ;
                eLIBs_printf("f:\\yuv420uvcombine-uv.binfp=%x\n", fp);
                if(fp)
                {
                    eLIBs_fwrite((void *)pCsiDevNode->p_csi_frame[index]->addr[1], pCsiDevNode->p_csi_frame[index]->src_rect.width * pCsiDevNode->p_csi_frame[index]->src_rect.height / 2, 1, fp);
                    eLIBs_fclose( fp ) ;
                }
                has_write_file = 1;
            }
#endif

            ret = EPDK_OK;
        }
        //            else {
        //            __inf("requesting csi frame fail\n");
        //            }
        //            else{
        //                __inf("full buffer invalid\n");
        //            }
        //__inf("DRV_CSI_CMD_REQUEST_FRAME\n");
        //ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_RELEASE_FRAME:
    {
        //linklist_insert(&csi_free, aux);
        //free2_insert(aux);
        pCsiDevNode->free2->insert_element(pCsiDevNode->free2, aux);
        //__inf("DRV_CSI_CMD_RELEASE_FRAME\n");
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_SET_PTS_MODE:
    {
        pCsiDevNode->pts_flag = aux;
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_INSTALL_CALLBACK_GET_PTS:
    {
        pCsiDevNode->CB_GetPts = (__pCBK_t)pbuffer;
        ret = EPDK_OK;
        break;
    }
    case DRV_CSI_CMD_SET_COLOR_EFFECT:
    {
        csi_color_effect = (__csi_color_effect_t)aux;
        csi_dev_set_color_effect(csi_color_effect);
        ret = EPDK_OK;
        __inf("DRV_CSI_CMD_SET_COLOR_EFFECT %d,%d\n", CSI_PORT, aux);
        break;
    }
    case DRV_CSI_CMD_GET_COLOR_EFFECT:
    {
        ret = csi_color_effect;
        __inf("DRV_CSI_CMD_GET_COLOR_EFFECT[%d]\n", CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_SET_AWB:
    {
        csi_awb = (__csi_awb_t)aux;
        csi_dev_set_awb(csi_awb);
        ret = EPDK_OK;
        __inf("DRV_CSI_CMD_SET_AWB %d[%d]\n", aux, CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_GET_AWB:
    {
        ret = csi_awb;
        __inf("DRV_CSI_CMD_GET_AWB[%d]\n", CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_SET_AE:
    {
        csi_ae = (__csi_ae_t)aux;
        csi_dev_set_ae(csi_ae);
        ret = EPDK_OK;
        __inf("DRV_CSI_CMD_SET_AE %d[%d]\n", aux, CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_GET_AE:
    {
        ret = csi_ae;
        __inf("DRV_CSI_CMD_GET_AE[%d]\n", CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_SET_BRIGHT:
    {
        csi_bright = (__csi_bright_t)aux;
        csi_dev_set_bright(csi_bright);
        ret = EPDK_OK;
        __inf("DRV_CSI_CMD_SET_BRIGHT %d[%d]\n", aux, CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_GET_BRIGHT:
    {
        ret = csi_bright;
        __inf("DRV_CSI_CMD_GET_BRIGHT[%d]\n", CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_SET_CONTRAST:
    {
        csi_contrast = (__csi_contrast_t)aux;
        csi_dev_set_contrast(csi_contrast);
        ret = EPDK_OK;
        __inf("DRV_CSI_CMD_SET_CONTRAST %d[%d]\n", aux, CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_GET_CONTRAST:
    {
        ret = csi_contrast;
        __inf("DRV_CSI_CMD_GET_CONTRAST[%d]\n", CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_SET_BAND:
    {
        csi_band = (__csi_band_t)aux;
        csi_dev_set_band(csi_band);
        ret = EPDK_OK;
        __inf("DRV_CSI_CMD_SET_BAND %d[%d]\n", aux, CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_GET_BAND:
    {
        ret = csi_band;
        __inf("DRV_CSI_CMD_GET_BAND[%d]\n", CSI_PORT);
        break;
    }
    case DRV_CSI_CMD_GET_PROGRSSV_INFO:
    {
        ret = bProgressiveSrc;
        __inf("DRV_CSI_CMD_GET_PROGRSSV_INFO[%d]\n", bProgressiveSrc);
        break;
    }
    default:
    {
        __wrn("Unkonwn Command...\n");
        ret = EPDK_FAIL;
        break;
    }
    }

    CSI_OpUnlock();

    return ret;
}

static  __s32   csi_clk_init(void)
{
    __u32 pa_addr, va_addr;
    __u32 temp_rd, mclk_div;
    user_gpio_set_t csi_pin_set[CSI_PIN_NUM] = {0};

    //pin settings
    if(  csi_GetPinInfo((CSI_PORT == 0) ? "csi0_para" : "csi1_para", csi_pin_set, CSI_PIN_NUM) )
    {
        __msg("Fail to get csi[%d] pins info from script.\n", CSI_PORT);
        return EPDK_FAIL;
    }

    pCsiDevNode->csi_pin = csi_PinGrpReq(csi_pin_set, CSI_PIN_NUM);
    if(!pCsiDevNode->csi_pin)
    {
        __msg("Fail to request pins for csi[%d].\n", CSI_PORT);
        return EPDK_FAIL;
    }

    //ahb bus clock
    pCsiDevNode->csi_ahbclk = csi_OpenMclk(CSP_CCM_MOD_CLK_AHB_CSI);
    if(!pCsiDevNode->csi_ahbclk)
    {
        __wrn("can NOT open csi[%d] ahb clock\n", CSI_PORT);
        return EPDK_FAIL;
    }

    //to sdram clock
    pCsiDevNode->csi_sdramclk = csi_OpenMclk(CSP_CCM_MOD_CLK_SDRAM_CSI);
    if(!pCsiDevNode->csi_sdramclk)
    {
        __wrn("can NOT open csi[%d] dram clock\n", CSI_PORT);
        return EPDK_FAIL;
    }

    //output clock
    csi_outclk = pCsiDevNode->csi_outclk = csi_OpenMclk(CSP_CCM_MOD_CLK_CSI);
    if(!pCsiDevNode->csi_outclk)
    {
        __wrn("can NOT open csi[%d] output clock\n", CSI_PORT);
        //    return EPDK_FAIL;	// fixed me
    }
    if(csi_mclk = 24)
    {

        csi_SetMclkSrc(csi_outclk, CSP_CCM_SYS_CLK_HOSC);
        //*((volatile __u32*)(0xf1c20000+0x134)) = 0x8500;
    }
    else
    {
        csi_SetMclkSrc(csi_outclk, CSP_CCM_SYS_CLK_VIDEO_PLL_1X);
        temp_rd = csi_GetSrcFreq(CSP_CCM_SYS_CLK_VIDEO_PLL_1X);
        mclk_div = temp_rd / (csi_mclk * 1000000);
        __inf("csi[%d] mclk=%dMHz, div=%d\n", CSI_PORT, csi_mclk, mclk_div);
        csi_SetMclkDiv(csi_outclk, mclk_div);
    }

    esCLK_MclkReset(csi_outclk, 0);
    //*((volatile __u32*)(0xf1c20000+0x2c4)) |= 1<<8;

    //open clock at last
    csi_MclkOnOff(pCsiDevNode->csi_ahbclk, CLK_ON);
    //*((volatile __u32*)(0xf1c20000+0x064)) |= 1<<8;
    csi_MclkOnOff(pCsiDevNode->csi_sdramclk, CLK_ON);
    //*((volatile __u32*)(0xf1c20000+0x100)) |= 1<<1;
    return EPDK_OK;
}

static  __s32   csi_clk_exit(void)
{
    __u32 pa_addr, va_addr;
    //csi ahb clock
    if(pCsiDevNode->csi_ahbclk)
    {
        csi_MclkOnOff(pCsiDevNode->csi_ahbclk, CLK_OFF);
        csi_CloseMclk(pCsiDevNode->csi_ahbclk);
        pCsiDevNode->csi_ahbclk = NULL;
    }

    //sdram to csi clock
    if(pCsiDevNode->csi_sdramclk)
    {
        csi_MclkOnOff(pCsiDevNode->csi_sdramclk, CLK_OFF);
        csi_CloseMclk(pCsiDevNode->csi_sdramclk);
        pCsiDevNode->csi_sdramclk = NULL;
    }

    //csi output clock CCMU_MCLK_EX_SENSOR
    if(pCsiDevNode->csi_outclk)
    {
        csi_SetMclkSrc(pCsiDevNode->csi_outclk, CSP_CCM_SYS_CLK_VIDEO_PLL_1X);//default
        csi_MclkOnOff(pCsiDevNode->csi_outclk, CLK_OFF);
        esCLK_MclkReset(csi_outclk, 1);

        csi_CloseMclk(pCsiDevNode->csi_outclk);
        pCsiDevNode->csi_outclk = NULL;
        csi_outclk = NULL;
    }
    if(pCsiDevNode->csi_pin)
    {
        csi_PinGrpRel(pCsiDevNode->csi_pin, 1);
        pCsiDevNode->csi_pin = NULL;

    }

    //    pa_addr=0x01c20800;
    //		va_addr=csi_pa2va(pa_addr,1);
    //		C(va_addr + 0x90,0x33333333);//CSI0
    //		C(va_addr + 0x94,0x00003333);//CSI0
    //		C(va_addr + 0x98,0x33333333);//CSI1
    //		C(va_addr + 0x94,0x33330000);//CSI1
    return EPDK_OK;
}
//=========================================================================================

/*******************************************************************************
Function name: CSI_DEV_NODE_Init_Part1
Description:
    1.初始化分多个阶段，在不同的时机调用.此为第一阶段.
Parameters:

Return:
    EPDK_OK/EPDK_FAIL
Time: 2010/1/4
*******************************************************************************/
__s32 CSI_DEV_NODE_Init_Part1()
{
    __u8    err;
    if(pCsiDevNode)
    {
        __wrn("csi device node resource has been requested already!\n");
        return EPDK_OK;
    }
    pCsiDevNode = (__csi_dev_t *)csi_malloc(sizeof(__csi_dev_t));
    if(!pCsiDevNode)
    {
        __wrn("request memory for csi device handle failed!\n");
        return EPDK_FAIL;
    }
    csi_memset(pCsiDevNode, 0, sizeof(__csi_dev_t));

    pCsiDevNode->csi_lock = csi_SemCreate(1);
    if(NULL == pCsiDevNode->csi_lock)
    {
        __wrn("create semaphore csi_lock fail\n");
        goto __err0;
    }
    pCsiDevNode->full2 = csi_linklist_manager_init();
    pCsiDevNode->free2 = csi_linklist_manager_init();
    if(NULL == pCsiDevNode->full2 || NULL == pCsiDevNode->free2)
    {
        goto __err1;
    }
    pCsiDevNode->full2->initial(pCsiDevNode->full2, CSI_LISTTYPE_FULL);
    pCsiDevNode->free2->initial(pCsiDevNode->free2, CSI_LISTTYPE_FREE);
    return EPDK_OK;
__err1:
    if(pCsiDevNode->full2)
    {
        pCsiDevNode->full2->exit(pCsiDevNode->full2);
        pCsiDevNode->full2 = NULL;
    }
    if(pCsiDevNode->free2)
    {
        pCsiDevNode->free2->exit(pCsiDevNode->free2);
        pCsiDevNode->free2 = NULL;
    }
    csi_SemDel(pCsiDevNode->csi_lock, OS_DEL_ALWAYS, &err);
    pCsiDevNode->csi_lock = NULL;
__err0:
    csi_free(pCsiDevNode);
    pCsiDevNode = NULL;
    return EPDK_FAIL;
}
__s32 CSI_DEV_NODE_Init_Part2()
{
    __s32   ret;
    if(NULL == pCsiDevNode)
    {
        __wrn("fatal error!, check pCsiDevNode == NULL\n");
        return EPDK_FAIL;
    }

    pCsiDevNode->iic_file = csi_fopen(CSI_IIC_PORT, "r+");
    if(!pCsiDevNode->iic_file)
    {
        __wrn("Open IIC Dev File Failed\n");
        return EPDK_FAIL;
    }
    fiic = pCsiDevNode->iic_file;
    __inf("open iic port = %s ok!\n", CSI_IIC_PORT);
    //add your code here
    ret = csi_clk_init();
    if(ret != EPDK_OK)
    {
        goto _err0;
    }
    return EPDK_OK;
_err0:
    csi_clk_exit();
    csi_fclose(pCsiDevNode->iic_file);
    pCsiDevNode->iic_file = NULL;
    return EPDK_FAIL;

}

__s32 CSI_DEV_NODE_Exit()
{
    __u8    err;
    if(NULL == pCsiDevNode)
    {
        __wrn("csi[%d] dev node already exit!\n", CSI_PORT);
        return EPDK_OK;
    }
    if(pCsiDevNode->full2)
    {
        pCsiDevNode->full2->exit(pCsiDevNode->full2);
        pCsiDevNode->full2 = NULL;
    }
    if(pCsiDevNode->free2)
    {
        pCsiDevNode->free2->exit(pCsiDevNode->free2);
        pCsiDevNode->free2 = NULL;
    }
    if(pCsiDevNode->csi_lock)
    {
        csi_SemDel(pCsiDevNode->csi_lock, OS_DEL_ALWAYS, &err);
        pCsiDevNode->csi_lock = NULL;
    }
    if(pCsiDevNode->iic_file)
    {
        csi_fclose(pCsiDevNode->iic_file);
        pCsiDevNode->iic_file = NULL;
        fiic = NULL;
    }
    csi_clk_exit();
    csi_free(pCsiDevNode);
    pCsiDevNode = NULL;
    return EPDK_OK;
}

void csi_mclk_on_off(__bool para)
{
    if (para == 0)
    {
        csi_MclkOnOff(pCsiDevNode->csi_outclk, CLK_OFF);
        *((volatile __u32 *)(0xf1c00000 + 0xf0)) = 0;
    }
    else if(para == 1)
    {
        csi_MclkOnOff(pCsiDevNode->csi_outclk, CLK_ON);
        *((volatile __u32 *)(0xf1c00000 + 0xf0)) = 0x81000000;
    }

}

void csi_pwdn(__bool para)
{
    __s32 ret = csi_WritePinData(pCsiDevNode->csi_pin, para, (CSI_PORT == 0) ? "CSI0_PWDN" : "CSI1_PWDN");
    //	__inf("set io %s\n",((ret==EPDK_OK)?"OK":"Fail"));
}

void csi_rst(__bool para)
{
    __s32 ret = csi_WritePinData(pCsiDevNode->csi_pin, para, (CSI_PORT == 0) ? "CSI0_RST" : "CSI1_RST");
    //	__inf("set io %s\n",((ret==EPDK_OK)?"OK":"Fail"));
}

void csi_pwren(__bool para)
{
    __s32 ret = csi_WritePinData(pCsiDevNode->csi_pin, para, (CSI_PORT == 0) ? "CSI0_PWREN" : "CSI1_PWREN");
    //	__inf("set io %s\n",((ret==EPDK_OK)?"OK":"Fail"));
}



