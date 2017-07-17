/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dode.c
* By      : sunny
* Version : V2.00                                                                         2009.10.30
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                              dode()
*
*
*  Parameters:
*
*  Return value:
*****************************************************************************
*/


int dode(struct op *t, const char **args)
{
    ES_FILE *dispfile;
    __s32 ret = 0;
    __u32 pbuffer[3] = {0};

    esDEV_Plugin("\\drv\\display.drv", 0, 0, 1);
    dispfile = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if(!dispfile)
    {
        Esh_printf("open display device fail!\n");
        return DOFAILED;
    }

    if(Esh_strcmp(args[1], "info") == 0 || Esh_strcmp(args[1], "INFO") == 0)
    {
        __disp_layer_info_t     layer_para;
        __s32 i = 0;
        char *wm[6] = {"NORMAL", "PALETTE", "INTER_BUF", "GAMMA", "SCALER", "reserved"};
        char *format[17] = {"1-bpp", "2-bpp", "4-bpp", "8-bpp", "rgb655", "rgb565", "rgb556",
                            "argb1555", "rgba5551", "rgb888", "argb8888",
                            "YUV444", "YUV422", "YUV420", "YUV411", "CSI RGB", "reserved"
                           };
        char *yuv_mode[6] = {"non-macro block planar", "interleaved", "non-macro block UV combined",
                             "macro block planar", "macro block UV combined", "reserved"
                            };

        Esh_printf(".Layers Information Begin.................................................................................................\n");
        for(i = 0; i < 4; i++)
        {
            __s32 pixel_fmt = 0;
            __s32 pixel_mode = 0;
            __s32 pixel_seq = 0;

            pbuffer[0] = i + 100;
            pbuffer[1] = (__u32)&layer_para;
            ret = eLIBs_fioctrl(dispfile, DISP_CMD_LAYER_GET_PARA, 0, (void *)pbuffer);
            if(ret != EPDK_OK)
            {
                Esh_printf(".\tLayer %d:close\n", i);
                continue;
            }
            Esh_printf(".\tLayer %d:open\n", i);

            Esh_printf(".\t\twork mode:%s\n", ((__s32)layer_para.mode >= 0 && layer_para.mode < 5) ? wm[layer_para.mode] : wm[5]);
            Esh_printf(".\t\tpriority:%d\n", layer_para.prio);

            pixel_fmt = layer_para.fb.format;
            pixel_mode = layer_para.fb.mode;
            pixel_seq = layer_para.fb.seq;
            Esh_printf(".\t\tformat:%s,", (pixel_fmt >= 0 && pixel_fmt < 16) ? format[pixel_fmt] : format[16]);
            Esh_printf("\tmode:%s,", (pixel_mode >= 0 && pixel_mode < 5) ? yuv_mode[pixel_mode] : yuv_mode[5]);


            if(pixel_fmt == DISP_FORMAT_ARGB8888 && pixel_mode == DISP_MOD_INTERLEAVED)
            {
                Esh_printf("\tpixels sequence:%s,", (pixel_seq == DISP_SEQ_ARGB) ? "ARGB" : "BGRA");
            }
            else if(pixel_fmt == DISP_FORMAT_YUV422 && pixel_mode == DISP_MOD_INTERLEAVED)
            {
                if(pixel_seq == DISP_SEQ_UYVY) Esh_printf("\tpixels sequence:UYVY,");
                else if(pixel_seq == DISP_SEQ_YUYV) Esh_printf("\tpixels sequence:YUYV,");
                else if(pixel_seq == DISP_SEQ_VYUY) Esh_printf("\tpixels sequence:VYUY,");
                else if(pixel_seq == DISP_SEQ_YVYU) Esh_printf("\tpixels sequence:YVYU,");
            }
            else if(pixel_fmt == DISP_FORMAT_YUV444 && pixel_mode == DISP_MOD_INTERLEAVED)
            {
                if(pixel_seq == DISP_SEQ_AYUV) Esh_printf("\tpixels sequence:AYUV,");
                else if(pixel_seq == DISP_SEQ_VUYA) Esh_printf("\tpixels sequence:VUYA,");
            }
            else if(pixel_fmt == DISP_FORMAT_YUV420 && (pixel_mode == DISP_MOD_NON_MB_UV_COMBINED || pixel_mode == DISP_MOD_MB_UV_COMBINED))
            {
                if(pixel_seq == DISP_SEQ_UVUV) Esh_printf("\tpixels sequence:UVUV,");
                else if(pixel_seq == DISP_SEQ_VUVU) Esh_printf("\tpixels sequence:VUVU,");
            }
            else
            {
                Esh_printf("\tpixels sequence:%d,", pixel_seq);
            }
            Esh_printf("\tB R channel swap:%s\n", layer_para.fb.br_swap == 0 ? "RGB" : "BGR");

            if(pixel_mode == DISP_MOD_INTERLEAVED)
            {
                Esh_printf(".\t\tframe buffer address 0:%x\n", layer_para.fb.addr[0]);
            }
            else if(pixel_mode == DISP_MOD_NON_MB_UV_COMBINED || pixel_mode == DISP_MOD_MB_UV_COMBINED)
            {
                Esh_printf(".\t\tframe buffer address 0:%x,\taddress 1:%x\n", layer_para.fb.addr[0], layer_para.fb.addr[1]);
            }
            else
            {
                Esh_printf(".\t\tframe buffer address 0:%x,\taddress 1:%x,\tddress 2:%x\n", layer_para.fb.addr[0], layer_para.fb.addr[1], layer_para.fb.addr[2]);
            }

            Esh_printf(".\t\tsource window(x:%d,\ty:%d,\twidth:%d,\theight:%d)\n", layer_para.src_win.x, layer_para.src_win.y, layer_para.src_win.width, layer_para.src_win.height);
            Esh_printf(".\t\tscreen window(x:%d,\ty:%d,\twidth:%d,\theight:%d)\n", layer_para.scn_win.x, layer_para.scn_win.y, layer_para.scn_win.width, layer_para.scn_win.height);

            Esh_printf(".\t\tcolor key:%s\n", layer_para.ck_enable == 0 ? "disable" : "enable");
            Esh_printf(".\t\talpha:%s,\talpha value:%x\n", layer_para.alpha_en == 0 ? "disable" : "enable", layer_para.alpha_val);
            Esh_printf(".\t\tpipe:%d\n", layer_para.pipe);
        }
        Esh_printf(".Layers Information End.................................................................................................\n");
    }
    else if(Esh_strcmp(args[1], "ol") == 0 || Esh_strcmp(args[1], "OL") == 0 || Esh_strcmp(args[1], "cl") == 0 || Esh_strcmp(args[1], "CL") == 0)
    {
        const char  *err = NULL;
        __s32 layer_num = 0;

        layer_num = eLIBs_strtol(args[2], &err, 0 );
        if(layer_num >= 0 && layer_num <= 3)
        {
            pbuffer[0] = layer_num + 100;
            if(Esh_strcmp(args[1], "ol") == 0 || Esh_strcmp(args[1], "OL") == 0)
            {
                ret = eLIBs_fioctrl(dispfile, DISP_CMD_LAYER_OPEN, 0, (void *)pbuffer);
            }
            else
            {
                ret = eLIBs_fioctrl(dispfile, DISP_CMD_LAYER_CLOSE, 0, (void *)pbuffer);
            }
            if(ret != EPDK_OK)
            {
                Esh_printf("open/close layer fail!\n");
            }
        }
        else
        {
            Esh_printf("para error!\n");
        }
    }

    eLIBs_fclose(dispfile);
    return DOSUCCEED;
}

