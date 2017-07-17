/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_herb sub-system
*                          vdrv encode framework module
*
*          (c) Copyright 2009-2010, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : cvr_watermark.c
* Version: V1.0
* By     : xiechr
* Date   : 2011-07-12
********************************************************************************
*/

#include "cvr_watermark.h"

static __u32                    pic_cnt;
__lotus_overlay_pic_info_t     *pic_info_640;       // 时间水印资源
__lotus_overlay_pic_info_t     *pic_info_1280;       // 时间水印资源


#define __msg(...)          eLIBs_printf(__VA_ARGS__)
#define __inf(...)          eLIBs_printf(__VA_ARGS__)
#define __wrn(...)          eLIBs_printf(__VA_ARGS__)

static __u32    mark_res_640[OVERLAY_MAX_SRC_NUM_PER_GROUP] = {ID_DV_VGA_WATER_MARK_0_BMP, ID_DV_VGA_WATER_MARK_1_BMP, ID_DV_VGA_WATER_MARK_2_BMP,
                                                               ID_DV_VGA_WATER_MARK_3_BMP, ID_DV_VGA_WATER_MARK_4_BMP, ID_DV_VGA_WATER_MARK_5_BMP, ID_DV_VGA_WATER_MARK_6_BMP,
                                                               ID_DV_VGA_WATER_MARK_7_BMP, ID_DV_VGA_WATER_MARK_8_BMP, ID_DV_VGA_WATER_MARK_9_BMP, ID_DV_VGA_WATER_MARK_MAOHAO_BMP,
                                                               ID_DV_VGA_WATER_MARK_DEC_BMP, ID_DV_VGA_WATER_MARK_BLANK_BMP
                                                              };
static __u32    mark_res_1280[OVERLAY_MAX_SRC_NUM_PER_GROUP] = {ID_DV_720_WATER_MARK_0_BMP, ID_DV_720_WATER_MARK_1_BMP, ID_DV_720_WATER_MARK_2_BMP,
                                                                ID_DV_720_WATER_MARK_3_BMP, ID_DV_720_WATER_MARK_4_BMP, ID_DV_720_WATER_MARK_5_BMP, ID_DV_720_WATER_MARK_6_BMP,
                                                                ID_DV_720_WATER_MARK_7_BMP, ID_DV_720_WATER_MARK_8_BMP, ID_DV_720_WATER_MARK_9_BMP, ID_DV_720_WATER_MARK_MAOHAO_BMP,
                                                                ID_DV_720_WATER_MARK_DEC_BMP, ID_DV_720_WATER_MARK_BLANK_BMP
                                                               };

static  void __cvr_res_get(__u32 res_id, void *buf, __u32 buf_len)
{
    char       *res_data;
    HTHEME       icon_hdle;

    icon_hdle   = dsk_theme_open(res_id);
    res_data    = dsk_theme_hdl2buf(icon_hdle);

    eLIBs_memcpy(buf, res_data + 56, buf_len);

    dsk_theme_close(icon_hdle);
}

void argb2yuv420sp(unsigned char *src_p, unsigned char *alph,
                   unsigned int width, unsigned int height,
                   unsigned char *dest_y, unsigned char *dest_c)
{
    int i, j;
    for(i = 0; i < (int)height; i++)
    {
        if((i & 1) == 0)
        {
            for(j = 0; j < (int)width; j++)
            {
                *dest_y = (299 * src_p[2] + 587 * src_p[1] + 114 * src_p[0]) / 1000;

                if((j & 1) == 0)
                {
                    //cb
                    *dest_c++ = 128 + (564 * (src_p[0] - *dest_y) / 1000);
                }
                else
                {
                    // cr
                    *dest_c++ = 128 + (713 * (src_p[2] - *dest_y) / 1000);
                }

                *alph++ = src_p[3];
                src_p += 4;
                dest_y++;
            }
        }
        else
        {
            for(j = 0; j < (int)width; j++)
            {
                *dest_y = (299 * src_p[2] + 587 * src_p[1] + 114 * src_p[0]) / 1000;
                *alph++ = src_p[3];
                src_p += 4;
                dest_y++;
            }
        }
    }

    return;
}
void    cvr_water_mark_res_init(void)
{
    __u32   i, len;
    char *ptmp_argb_buf ;
    pic_cnt = 13;
    pic_info_640 = esMEMS_Balloc(pic_cnt * sizeof(__lotus_overlay_pic_info_t));
    pic_info_1280 = esMEMS_Balloc(pic_cnt * sizeof(__lotus_overlay_pic_info_t));

    ptmp_argb_buf = esMEMS_Balloc(18 * 24 * 4);
    for(i = 0; i < pic_cnt; i++)
    {
        pic_info_640[i].ID            = i;
        pic_info_640[i].size.width    = 12;
        pic_info_640[i].size.height   = 16;
        len = pic_info_640[i].size.width * pic_info_640[i].size.height;
        pic_info_640[i].y = esMEMS_Balloc(len);
        pic_info_640[i].c = esMEMS_Balloc(len >> 1);
        pic_info_640[i].alph = esMEMS_Balloc(len);
        __cvr_res_get(mark_res_640[i], ptmp_argb_buf, len << 2);
        argb2yuv420sp(ptmp_argb_buf, pic_info_640[i].alph, pic_info_640[i].size.width,
                      pic_info_640[i].size.height, pic_info_640[i].y, pic_info_640[i].c);
    }

    for(i = 0; i < pic_cnt; i++)
    {
        pic_info_1280[i].ID            = OVERLAY_MAX_SRC_NUM_PER_GROUP + i;
        pic_info_1280[i].size.width    = 18;
        pic_info_1280[i].size.height   = 24;
        len = pic_info_1280[i].size.width * pic_info_1280[i].size.height;
        pic_info_1280[i].y = esMEMS_Balloc(len);
        pic_info_1280[i].c = esMEMS_Balloc(len >> 1);
        pic_info_1280[i].alph = esMEMS_Balloc(len);

        __cvr_res_get(mark_res_1280[i], ptmp_argb_buf, len << 2);
        argb2yuv420sp(ptmp_argb_buf, pic_info_1280[i].alph, pic_info_1280[i].size.width,
                      pic_info_1280[i].size.height, pic_info_1280[i].y, pic_info_1280[i].c);

    }

    eLIBs_free(ptmp_argb_buf);

}

void    cvr_water_mark_res_uninit(void)
{
    __u32       i;

    for(i = 0; i < pic_cnt; i++)
    {
        if(pic_info_640[i].y)
        {
            esMEMS_Bfree(pic_info_640[i].y, pic_info_640[i].size.width * pic_info_640[i].size.height );
            pic_info_640[i].y = NULL;
        }
        if(pic_info_640[i].alph)
        {
            esMEMS_Bfree(pic_info_640[i].alph, pic_info_640[i].size.width * pic_info_640[i].size.height );
            pic_info_640[i].alph = NULL;
        }
        if(pic_info_640[i].c)
        {
            esMEMS_Bfree(pic_info_640[i].c, pic_info_640[i].size.width * pic_info_640[i].size.height >> 1 );
            pic_info_640[i].c = NULL;
        }

        if(pic_info_1280[i].y)
        {
            esMEMS_Bfree(pic_info_1280[i].y, pic_info_1280[i].size.width * pic_info_1280[i].size.height );
            pic_info_1280[i].y = NULL;
        }
        if(pic_info_1280[i].alph)
        {
            esMEMS_Bfree(pic_info_1280[i].alph, pic_info_1280[i].size.width * pic_info_1280[i].size.height );
            pic_info_1280[i].alph = NULL;
        }
        if(pic_info_1280[i].c)
        {
            esMEMS_Bfree(pic_info_1280[i].c, pic_info_1280[i].size.width * pic_info_1280[i].size.height >> 1 );
            pic_info_1280[i].c = NULL;
        }

    }
    if(pic_info_640)
    {
        esMEMS_Bfree(pic_info_640, pic_cnt * sizeof(__lotus_overlay_pic_info_t));
        pic_info_640 = NULL;
    }
    if(pic_info_1280)
    {
        esMEMS_Bfree(pic_info_1280, pic_cnt * sizeof(__lotus_overlay_pic_info_t));
        pic_info_1280 = NULL;
    }
}

/*******************************************************************************
* Function name: cvr_water_mark_get_overlay_data
* Description:
*    获取时间水印的图标数据
*
* Parameters:
*    @param level:    获取的数据尺寸	(0:640x480、 1:1280x720)
* Outputs:
*    @param overlay_src_pic:   把ARGB数据转换成结构体形式
*
*
* Return:
*    @return: on success return EPDK_OK otherwise return EPDK_FAIL.
* Time: 2011/7/7
*******************************************************************************/
__s32   cvr_water_mark_get_overlay_data(__lotus_overlay_src_init_t  *overlay_src_pic, __u32 level)
{
    __u32   picIndex, pixelIndex, paletteIndex;
    __u32   picSize;
    __u32  *psrcPixel;

    __msg("cvr_water_mark_get_overlay_data\n");

    if(overlay_src_pic == NULL)
    {
        __wrn("input para err!\n");
        return EPDK_FAIL;
    }

    overlay_src_pic->srcPicNum = pic_cnt;
    if(level == 0)
    {
        eLIBs_memcpy((void *)overlay_src_pic->srcPic, (void *)pic_info_640, pic_cnt * sizeof(__lotus_overlay_pic_info_t));
    }
    else
    {
        eLIBs_memcpy((void *)overlay_src_pic->srcPic, (void *)pic_info_1280, pic_cnt * sizeof(__lotus_overlay_pic_info_t));
    }
    return EPDK_OK;
}



