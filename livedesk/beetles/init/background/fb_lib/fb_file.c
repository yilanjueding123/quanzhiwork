/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_willow sub-system
*
*                               (c) Copyright 2006-2009, ALLWINNERME, China
*                                           All Rights Reserved
*
* File   : fb_file.c
* Version: V1.0
* By     : xiechuanrong
*********************************************************************************************************
*/
#include "mod_init_i.h"
#include "fb_file.h"


/************************************************************************
* Function: backlayer_create_fb_file
* Description: 保存fb到文件
* Input:
*    FB *fb：待保存的fb
*    __u8 * filename：保存fb的文件名
* Output:
* Return:
*     EPDK_OK: 成功
*     EPDK_FAIL: 失败
*************************************************************************/
__s32 create_fb_file (FB *framebuffer, __u8 *filename)
{
    __hdle fd;
    __fb_file_header_t file_header;
    __s32 write_num;

    fd = fopen((char *)filename, "w+");
    if (fd == NULL)
    {
        __wrn("------open file:%s err!------\n", filename);
        return EPDK_FAIL;
    }
    memset(&file_header, 0, sizeof(file_header));
    file_header.version = 0x100;
    file_header.height = framebuffer->size.height;
    file_header.width = framebuffer->size.width;
    file_header.cs_mode = framebuffer->fmt.cs_mode;
    file_header.fmt_type = framebuffer->fmt.type;

    if (framebuffer->fmt.type == FB_TYPE_RGB)
    {
        file_header.pix_fmt = framebuffer->fmt.fmt.rgb.pixelfmt;
        file_header.pix_seq = framebuffer->fmt.fmt.rgb.pixseq;
        file_header.mod_or_br_swap = framebuffer->fmt.fmt.rgb.br_swap;
        file_header.data_len[0] = file_header.height * file_header.width * 4;
    }
    else
    {
        if (framebuffer->fmt.fmt.yuv.mod != YUV_MOD_NON_MB_PLANAR)
        {
            __wrn("not support format!\n");
            fclose(fd);
            return EPDK_FAIL;
        }
        file_header.pix_fmt = framebuffer->fmt.fmt.yuv.pixelfmt;
        file_header.pix_seq = framebuffer->fmt.fmt.yuv.yuvseq;
        file_header.mod_or_br_swap = framebuffer->fmt.fmt.yuv.mod;
        file_header.data_len[0] = file_header.height * file_header.width;
        switch (framebuffer->fmt.fmt.yuv.pixelfmt)
        {
        case PIXEL_YUV444:
        {
            file_header.data_len[1] = file_header.height * file_header.width;
            file_header.data_len[2] = file_header.height * file_header.width;
            break;
        }
        case PIXEL_YUV422:
        {
            file_header.data_len[1] = file_header.height * file_header.width / 2;
            file_header.data_len[2] = file_header.height * file_header.width / 2;
            break;
        }
        case PIXEL_YUV420:
        {
            file_header.data_len[1] = file_header.height * file_header.width / 4;
            file_header.data_len[2] = file_header.height * file_header.width / 4;
            break;
        }
        case PIXEL_YUV411:
        {
            file_header.data_len[1] = file_header.height * file_header.width / 4;
            file_header.data_len[2] = file_header.height * file_header.width / 4;
            break;
        }
        default:
        {
            __wrn("not support format!\n");
            fclose(fd);
            return EPDK_FAIL;
        }

        }
    }
    file_header.offset_data[0] = sizeof(__fb_file_header_t);
    file_header.offset_data[1] = file_header.offset_data[0] + file_header.data_len[0];
    file_header.offset_data[2] = file_header.offset_data[1] + file_header.data_len[1];

    write_num = fwrite(&file_header, 1, sizeof(file_header), fd);
    if(write_num != sizeof(file_header))
    {
        __msg("not enough space...\n");
        fclose(fd);
        return EPDK_FAIL;
    }
    write_num = fwrite(framebuffer->addr[0], 1, file_header.data_len[0], fd);
    if(write_num != file_header.data_len[0])
    {
        __msg("not enough space...\n");
        fclose(fd);
        return EPDK_FAIL;
    }
    if (file_header.data_len[1] != 0)
    {
        write_num = fwrite(framebuffer->addr[1], 1, file_header.data_len[1], fd);
        if(write_num != file_header.data_len[1])
        {
            __msg("not enough space...\n");
            fclose(fd);
            return EPDK_FAIL;
        }
    }
    if (file_header.data_len[2] != 0)
    {
        write_num = fwrite(framebuffer->addr[2], 1, file_header.data_len[2], fd);
        if(write_num != file_header.data_len[2])
        {
            __msg("not enough space...\n");
            fclose(fd);
            return EPDK_FAIL;
        }
    }
    fclose(fd);
    return EPDK_OK;
}



/************************************************************************
* Function: convert_fb_yuv420
* Description: 将framebuffer转换为yuv420格式
* Input:
*    FB *in_frame：源frame buffer
*    FB *out_frame：目标frame buffer
*    SIZE *screen_size: 屏幕尺寸
*    __u8 mod: 模式包括:
*       BACKLAYER_MOD_RATIO----适合屏幕尺寸(图片长宽比例不变)
*       BACKLAYER_MOD_STRETCH----拉伸模式,缩放到屏幕尺寸
* Output:
* Return:
*     EPDK_OK: 成功
*     EPDK_FAIL: 失败
*************************************************************************/
__s32 convert_fb (FB *in_frame, FB *out_frame, SIZE *screen_size, __u8 mod)
{
    __s32 scaler_hdle;
    ES_FILE *de_hdle;
    __disp_scaler_para_t param;
    __disp_fb_t			disp_in_frame;
    __disp_fb_t			disp_out_frame;
    __s32 ret_val = EPDK_OK;
    __u32 arg[3];

    //    if (mod == BACKLAYER_MOD_RATIO)
    {
        out_frame->size = in_frame->size ;
        if(out_frame->size.width > screen_size->width)
        {
            out_frame->size.width = screen_size->width;
            out_frame->size.height = out_frame->size.width * in_frame->size.height / in_frame->size.width;
        }

        if(out_frame->size.height > screen_size->height)
        {
            out_frame->size.height = screen_size->height;
            out_frame->size.width = out_frame->size.height * in_frame->size.width / in_frame->size.height;
        }

        if(out_frame->size.width == 0)
        {
            out_frame->size.width = 1;
        }

        if(out_frame->size.height == 0)
        {
            out_frame->size.height = 1;
        }
    }
    //    else
    //	{
    //		out_frame->size = *screen_size;
    //	}
    /*由于带宽问题，最大尺寸为1280*720，大于这个尺寸由scaler mode放大得到*/
    if(out_frame->size.width > 1280)
    {
        out_frame->size.width = 1280;
        out_frame->size.height = out_frame->size.width * in_frame->size.height / in_frame->size.width;
    }

    if(out_frame->size.height > 720)
    {
        out_frame->size.height = 720;
        out_frame->size.width = out_frame->size.height * in_frame->size.width / in_frame->size.height;
    }

    if (out_frame->size.width > SCALE_OUT_MAX_WIDTH)
    {
        out_frame->size.width = SCALE_OUT_MAX_WIDTH;
    }
    if (out_frame->size.height > SCALE_OUT_MAX_HEIGHT)
    {
        out_frame->size.height = SCALE_OUT_MAX_HEIGHT;
    }
    if (in_frame->fmt.type == FB_TYPE_RGB)
    {
        __err("not support fb type!\n");
        return EPDK_FAIL;
    }


    out_frame->size.height = ((out_frame->size.height + 3) >> 2) << 2;
    out_frame->size.width = ((out_frame->size.width + 3) >> 2) << 2;

    if (out_frame->fmt.type != FB_TYPE_YUV && out_frame->fmt.type != FB_TYPE_RGB)
    {
        out_frame->fmt.type = FB_TYPE_YUV;
    }
    out_frame->fmt.cs_mode = in_frame->fmt.cs_mode;
    if (out_frame->fmt.type == FB_TYPE_YUV)
    {
        __u32 tmplen;
        out_frame->fmt.fmt.yuv.mod = YUV_MOD_NON_MB_PLANAR;
        out_frame->fmt.fmt.yuv.pixelfmt = PIXEL_YUV422;
        out_frame->fmt.fmt.yuv.yuvseq = YUV_SEQ_OTHRS;
        tmplen = out_frame->size.height * out_frame->size.width;
        tmplen = ((tmplen >> 6) + 1) << 6;
        out_frame->addr[1] = (__u8 *)out_frame->addr[0] + tmplen;
        out_frame->addr[2] = (__u8 *)out_frame->addr[1] + tmplen / 2;
    }
    else
    {
        out_frame->fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
        out_frame->fmt.fmt.rgb.br_swap = 0;
        out_frame->fmt.fmt.rgb.pixseq = 0;
    }



    de_hdle = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if (de_hdle == ((ES_FILE *)0) )
    {
        __err("can not open display driver\n");
        return EPDK_FAIL;
    }

    {
        arg[0] = 0;
        arg[1] = 0;
        arg[2] = 0;
        scaler_hdle = eLIBs_fioctrl(de_hdle, DISP_CMD_SCALER_REQUEST, 0, (void *)arg);
    }

    if(scaler_hdle <= 0)
    {
        __wrn("request scaler fail\n");
        eLIBs_fclose(de_hdle);
        return EPDK_FAIL;
    }

    GUI_Fb2Dispfb(in_frame, &disp_in_frame);
    //param.input_fb = disp_in_frame;
    eLIBs_memcpy(&(param.input_fb), &disp_in_frame, sizeof(__disp_fb_t));
    param.source_regn.x = 0;
    param.source_regn.y = 0;
    param.source_regn.width = in_frame->size.width;
    param.source_regn.height = in_frame->size.height;
    //param.output_fb = disp_out_frame;
    GUI_Fb2Dispfb(out_frame, &disp_out_frame);
    eLIBs_memcpy(&(param.output_fb), &disp_out_frame, sizeof(__disp_fb_t));

    /*{
    	__u32 i=0;
    	for(i=0; i<(param.source_regn.height/2) * disp_in_frame.size.width; i++)
    	{
    		*(__u32*)(disp_in_frame.addr[0] + i*4) = 0xff00ff00;
    	}
    	for(i=(param.source_regn.height/2) * disp_in_frame.size.width; i<param.source_regn.height * disp_in_frame.size.width; i++)
    	{
    		*(__u32*)(disp_in_frame.addr[0] + i*4) = 0xff0000ff;
    	}
    }*/
    arg[0] = scaler_hdle;
    arg[1] = (__u32)&param;
    arg[2] = 0;
    if(eLIBs_fioctrl(de_hdle, DISP_CMD_SCALER_EXECUTE, 0, (void *)arg))
    {
        __wrn("scaler img fail\n");
        ret_val = EPDK_FAIL;
    }

    arg[0] = scaler_hdle;
    arg[1] = 0;
    arg[2] = 0;
    eLIBs_fioctrl(de_hdle, DISP_CMD_SCALER_RELEASE, 0, (void *)arg);
    eLIBs_fclose(de_hdle);

    return ret_val;
}


/************************************************************************
* Function: get_fb_from_file
* Description: 从文件中取出fb
* Input:
*    FB *framebuffer：frame buffer.
*            设置输入的fb type为FB_TYPE_RGB或者FB_TYPE_YUV
*    __u8 *buffer: framebuffer空间。默认为获取yuv422格式，大小为2*height*width
*    __u8 *screen_size: 屏幕尺寸
*    __u8 mod: 模式包括:
*       BACKLAYER_MOD_RATIO----适合屏幕尺寸(图片长宽比例不变)
*       BACKLAYER_MOD_STRETCH----拉伸模式,缩放到屏幕尺寸
*    __u8 * filename：保存fb的文件名
* Output:
*    FB *framebuffer：读取到的的frame buffer
* Return:
*     EPDK_OK: 成功
*     EPDK_FAIL: 失败
*************************************************************************/
__s32 get_fb_from_file (FB *framebuffer, __u8 *buffer, SIZE *screen_size, __u8 mod, __u8 *filename)
{
    __hdle fd;
    __u32 tmp_len;
    __fb_file_header_t file_header;
    __s32 ret_val = EPDK_FAIL;
    FB tmp_frame;
    char *pbuf;
    __u32 file_len;
    __u32 read_len;
    char *uncompress_buf;
    __u32 uncompress_len;

    __msg("filename=%s\n", filename);
    fd = fopen((char *)filename, "rb");
    if (fd == NULL)
    {
        __wrn("------open file:%s err!------\n", filename);
        return EPDK_FAIL;
    }

    fseek(fd, 0, SEEK_END);
    file_len = ftell(fd);
    __msg("file_len=%d\n", file_len);

    pbuf = palloc((file_len + 1023) / 1024, 0);
    __msg("pbuf=%x\n", pbuf);
    if(!pbuf)
    {
        fclose(fd);
        __msg("palloc fail...\n");
        return EPDK_FAIL;
    }
    fseek(fd, 0, SEEK_SET);
    read_len = fread(pbuf, 1, file_len, fd);
    if(read_len != file_len)
    {
        pfree(pbuf, (file_len + 1023) / 1024);
        fclose(fd);
        __msg("fread fail...\n");
        return EPDK_FAIL;
    }

    if(0)
    {

        int i;

        __msg("begin log pbuf\n");
        for(i = 0 ; i < 500 ; i++)
        {
            eLIBs_printf("%2x ", pbuf[i]);
            if((i % 17) == 16)
            {
                eLIBs_printf("\n");
            }
        }
    }

    __here__;
    //未压缩的背景
    if(EPDK_FALSE == AZ100_IsCompress(pbuf, file_len))
    {
        __msg("bg pic is uncompress...\n");
        uncompress_buf = pbuf;
        uncompress_len = file_len;
        __msg("uncompress_buf=%x\n", uncompress_buf);
    }
    else//带压缩的背景
    {
        __msg("bg pic is compress...\n");
        uncompress_len = AZ100_GetUncompressSize(pbuf, file_len);

        uncompress_buf = palloc((uncompress_len + 1023) / 1024, 0);
        __msg("uncompress_buf=%x\n", uncompress_buf);

        if(!uncompress_buf)
        {
            pfree(pbuf, (file_len + 1023) / 1024);
            fclose(fd);
            __msg("palloc fail...\n");
            return EPDK_FAIL;
        }

        ret_val = AZ100_Uncompress(pbuf, file_len, uncompress_buf, uncompress_len);
        if(EPDK_FAIL == ret_val)
        {
            pfree(pbuf, (file_len + 1023) / 1024);
            pfree(uncompress_buf, (uncompress_len + 1023) / 1024);
            fclose(fd);
            __msg("palloc fail...\n");
            return EPDK_FAIL;
        }
        __here__;
        pfree(pbuf, (file_len + 1023) / 1024);
        pbuf = NULL;
        __here__;
    }

    pbuf = uncompress_buf;

    if(0)
    {
        int i;

        __msg("begin log uncompress_buf\n");

        for(i = 0 ; i < 500 ; i++)
        {
            eLIBs_printf("%2x ", uncompress_buf[i]);
            if((i % 17) == 16)
            {
                eLIBs_printf("\n");
            }
        }
    }

    eLIBs_memcpy((void *)&file_header, pbuf, sizeof(__fb_file_header_t));
    pbuf += sizeof(__fb_file_header_t);

    memset(&tmp_frame, 0x0, sizeof(FB));
    tmp_frame.size.height = file_header.height;
    tmp_frame.size.width = file_header.width;
    tmp_frame.fmt.cs_mode = (__cs_mode_t)file_header.cs_mode;
    tmp_frame.fmt.type = (__fb_type_t)file_header.fmt_type;

    if (tmp_frame.fmt.type == FB_TYPE_RGB)
    {
        tmp_frame.fmt.fmt.rgb.pixelfmt = (__pixel_rgbfmt_t)file_header.pix_fmt;
        tmp_frame.fmt.fmt.rgb.pixseq = (__u8)file_header.pix_seq;
        tmp_frame.fmt.fmt.rgb.br_swap = (__bool)file_header.mod_or_br_swap;
    }
    else
    {
        if (file_header.mod_or_br_swap != YUV_MOD_NON_MB_PLANAR)
        {
            __wrn("not support format!\n");
            goto EXIT_GET_FB0;
        }
        tmp_frame.fmt.fmt.yuv.pixelfmt = (__pixel_yuvfmt_t)file_header.pix_fmt;
        tmp_frame.fmt.fmt.yuv.mod = (__yuv_mod_t)file_header.mod_or_br_swap;
        tmp_frame.fmt.fmt.yuv.pixelfmt = (__pixel_yuvfmt_t)file_header.pix_fmt;
        tmp_frame.fmt.fmt.yuv.yuvseq = (__yuv_seq_t)file_header.pix_seq;
    }

    tmp_frame.addr[0] = pbuf;
    pbuf +=  file_header.data_len[0];

    if (file_header.data_len[1] != 0)
    {
        tmp_frame.addr[1] = pbuf;
        pbuf +=  file_header.data_len[1];
    }

    if (file_header.data_len[2] != 0)
    {
        tmp_frame.addr[2] = pbuf;
    }

    framebuffer->addr[0] = buffer;
    ret_val = convert_fb(&tmp_frame, framebuffer, screen_size, mod);

EXIT_GET_FB0:
    fclose(fd);
    __here__;
    pfree(uncompress_buf, (uncompress_len + 1023) / 1024);
    __here__;
    return ret_val;
}

