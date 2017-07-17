#include "drv_display_i.h"

static __u32    de_ext_hd;
extern __disp_drv_t    g_disp_drv;

__s32 disp_get_free_event(__u32 sel)
{
    __u32 i = 0;

    for(i = 0; i < MAX_EVENT_SEM; i++)
    {
        if(!g_disp_drv.event_used[sel][i])
        {
            g_disp_drv.event_used[sel][i] = 1;
            return i;
        }
    }
    return -1;
}

__s32 disp_cmd_before(__u32 sel)
{
    if(g_disp_drv.b_cache[sel] == 0)
    {
        __s32 event_id = 0;

        event_id = disp_get_free_event(sel);
        if(event_id >= 0)
        {
            g_disp_drv.event_sem[sel][event_id] = esKRNL_SemCreate(0);
            if(!g_disp_drv.event_sem[sel][event_id])
            {
                __wrn("create event_sem fail!\n");
                return EPDK_FAIL;
            }
            //__inf("de:0\n");
        }
        else
        {
            __wrn("disp_get_free_event() fail!\n");
        }
        return event_id;
    }
    return 0;

}

void disp_cmd_after(__u32 sel, __s32 event_id)
{
    if(g_disp_drv.b_cache[sel] == 0)
    {
        __u8 err;

        if(event_id >= 0 && g_disp_drv.event_sem[sel][event_id] != NULL)
        {
            esKRNL_SemPend(g_disp_drv.event_sem[sel][event_id], 0x50, &err);
            //__inf("de:2\n");

            if(err != OS_NO_ERR)
            {
                eLIBs_printf("pend event sem overtime!\n");
            }

            esKRNL_SemDel(g_disp_drv.event_sem[sel][event_id], 0, &err);
            g_disp_drv.event_sem[sel][event_id] = NULL;
            g_disp_drv.event_used[sel][event_id] = 0;
        }
        else
        {
            __wrn("no event sem in disp_cmd_after!\n");
        }
    }
}

__s32 disp_int_process(__u32 sel)
{
    __u32 i = 0;

    for(i = 0; i < MAX_EVENT_SEM; i++)
    {
        if(g_disp_drv.event_sem[sel][i] != NULL)
        {
            esKRNL_SemPost(g_disp_drv.event_sem[sel][i]);
            //__inf("de:1\n");
            g_disp_drv.event_used[sel][i] = 0;
        }
    }
    return EPDK_OK;
}

__hdle DEV_DISP_MOpen(void *open_arg, __u32 mode)
{
    return (__hdle)&de_ext_hd;
}

__s32 DEV_DISP_MClose(__hdle     hdev)
{
    return EPDK_OK;
}

__u32 DEV_DISP_MRead(void *pBuffer, __u32 size, __u32 count, __hdle hdev)
{
    return EPDK_OK;
}

__u32 DEV_DISP_MWrite(const void *pBuffer, __u32 size, __u32 count, __hdle hdev)
{
    return EPDK_OK;
}

__s32 DEV_DISP_MIoctrl(__hdle hDev, __u32 cmd, __s32 aux, void *pbuffer)
{
    __u32 para0 = 0, para1 = 0, para2 = 0;
    __s32 ret = 0;
    __s32 event_id = 0;


    if(pbuffer != NULL)
    {
        para0 = *((__u32 *)pbuffer);
        para1 = *((__u32 *)((__u32)pbuffer + 4));
        para2 = *((__u32 *)((__u32)pbuffer + 8));
    }

    if(cmd !=  DISP_CMD_GET_OUTPUT_TYPE && cmd != DISP_CMD_SCN_GET_WIDTH && cmd != DISP_CMD_SCN_GET_HEIGHT && cmd != DISP_CMD_LCD_GET_BRIGHTNESS)
    {
        // __inf("de:%x\n",cmd);
    }

    if(g_disp_drv.reg_operate_vaild != 1 && cmd != DISP_CMD_CLK_ON)	/* g_disp_drv.reg_operate_vaild=1 when call DRV_DISP_MInit*/
    {
        __wrn("Registers CAN'T READ OR WRITE\n", cmd);
        return EPDK_FAIL;
    }

    switch(cmd)
    {
    //----disp global----
    case DISP_CMD_SET_BKCOLOR:
        ret = BSP_disp_set_bk_color(aux, (__disp_color_t *)para0);
        break;

    case DISP_CMD_SET_COLORKEY:
        ret = BSP_disp_set_color_key(aux, (__disp_colorkey_t *)para0);
        break;

    case DISP_CMD_SET_PALETTE_TBL:
        ret = BSP_disp_set_palette_table(aux, (__u32 *)para0, para1, para2);
        break;

    case DISP_CMD_GET_PALETTE_TBL:
        ret = BSP_disp_get_palette_table(aux, (__u32 *)para0, para1, para2);
        break;

    case DISP_CMD_START_CMD_CACHE:
        ret = BSP_disp_cmd_cache(aux);
        g_disp_drv.b_cache[aux] = 1;
        break;

    case DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE:
        g_disp_drv.b_cache[aux] = 0;
        event_id = disp_cmd_before(aux);
        ret = BSP_disp_cmd_submit(aux);
        disp_cmd_after(aux, event_id);
        break;

    case DISP_CMD_GET_OUTPUT_TYPE:
        ret =  BSP_disp_get_output_type(aux);
        break;

    case DISP_CMD_SCN_GET_WIDTH:
        ret = BSP_disp_get_screen_width(aux);
        break;

    case DISP_CMD_SCN_GET_HEIGHT:
        ret = BSP_disp_get_screen_height(aux);
        break;
    case DISP_CMD_SET_BRIGHT:
        ret = BSP_disp_set_bright(aux, para0);
        break;

    case DISP_CMD_GET_BRIGHT:
        ret = BSP_disp_get_bright(aux);
        break;

    case DISP_CMD_SET_CONTRAST:
        ret = BSP_disp_set_contrast(aux, para0);
        break;

    case DISP_CMD_GET_CONTRAST:
        ret = BSP_disp_get_contrast(aux);
        break;

    case DISP_CMD_SET_SATURATION:
        ret = BSP_disp_set_saturation(aux, para0);
        break;

    case DISP_CMD_GET_SATURATION:
        ret = BSP_disp_get_saturation(aux);
        break;

    case DISP_CMD_SET_HUE:
        ret = BSP_disp_set_hue(aux, para0);
        break;

    case DISP_CMD_GET_HUE:
        ret = BSP_disp_get_hue(aux);
        break;
    case DISP_CMD_ENHANCE_ON:
        ret = BSP_disp_enhance_enable(aux, 1);
        break;

    case DISP_CMD_ENHANCE_OFF:
        ret = BSP_disp_enhance_enable(aux, 0);
        break;

    case DISP_CMD_GET_ENHANCE_EN:
        ret = BSP_disp_get_enhance_enable(aux);
        break;
    case DISP_CMD_CLK_ON:
        ret = BSP_disp_clk_on();
        g_disp_drv.reg_operate_vaild = 1;
        break;

    case DISP_CMD_CLK_OFF:
        g_disp_drv.reg_operate_vaild = 0;
        ret = BSP_disp_clk_off();
        break;

    case DISP_CMD_SET_DE_FLICKER:
        event_id = disp_cmd_before(aux);
        ret = BSP_disp_set_de_flicker(aux, para0);
        disp_cmd_after(aux, event_id);
        break;

    //----layer----
    case DISP_CMD_LAYER_REQUEST:
        ret = BSP_disp_layer_request(aux, (__disp_layer_work_mode_t)para0);
        break;

    case DISP_CMD_LAYER_RELEASE:
        ret = BSP_disp_layer_release(aux, para0);
        break;

    case DISP_CMD_LAYER_OPEN:
        ret = BSP_disp_layer_open(aux, para0);
        break;

    case DISP_CMD_LAYER_CLOSE:
        event_id = disp_cmd_before(aux);
        ret = BSP_disp_layer_close(aux, para0);
        disp_cmd_after(aux, event_id);
        break;

    case DISP_CMD_LAYER_SET_FB:
        event_id = disp_cmd_before(aux);
        ret = BSP_disp_layer_set_framebuffer(aux, para0, (__disp_fb_t *)para1);
        disp_cmd_after(aux, event_id);
        break;

    case DISP_CMD_LAYER_GET_FB:
        ret = BSP_disp_layer_get_framebuffer(aux, para0, (__disp_fb_t *)para1);
        break;

    case DISP_CMD_LAYER_SET_SRC_WINDOW:
        event_id = disp_cmd_before(aux);
        ret = BSP_disp_layer_set_src_window(aux, para0, (__disp_rect_t *)para1);
        disp_cmd_after(aux, event_id);
        break;

    case DISP_CMD_LAYER_GET_SRC_WINDOW:
        ret = BSP_disp_layer_get_src_window(aux, para0, (__disp_rect_t *)para1);
        break;

    case DISP_CMD_LAYER_SET_SCN_WINDOW:
        event_id = disp_cmd_before(aux);
        ret = BSP_disp_layer_set_screen_window(aux, para0, (__disp_rect_t *)para1);
        disp_cmd_after(aux, event_id);
        break;

    case DISP_CMD_LAYER_GET_SCN_WINDOW:
        ret = BSP_disp_layer_get_screen_window(aux, para0, (__disp_rect_t *)para1);
        break;

    case DISP_CMD_LAYER_SET_PARA:
        event_id = disp_cmd_before(aux);
        ret = BSP_disp_layer_set_para(aux, para0, (__disp_layer_info_t *)para1);
        disp_cmd_after(aux, event_id);
        break;

    case DISP_CMD_LAYER_GET_PARA:
        ret = BSP_disp_layer_get_para(aux, para0, (__disp_layer_info_t *)para1);
        break;

    case DISP_CMD_LAYER_TOP:
        ret = BSP_disp_layer_set_top(aux, para0);
        break;

    case DISP_CMD_LAYER_BOTTOM:
        ret = BSP_disp_layer_set_bottom(aux, para0);
        break;

    case DISP_CMD_LAYER_ALPHA_ON:
        ret = BSP_disp_layer_alpha_enable(aux, para0, 1);
        break;

    case DISP_CMD_LAYER_ALPHA_OFF:
        ret = BSP_disp_layer_alpha_enable(aux, para0, 0);
        break;

    case DISP_CMD_LAYER_SET_ALPHA_VALUE:
        event_id = disp_cmd_before(aux);
        ret = BSP_disp_layer_set_alpha_value(aux, para0, para1);
        disp_cmd_after(aux, event_id);
        break;

    case DISP_CMD_LAYER_CK_ON:
        ret = BSP_disp_layer_colorkey_enable(aux, para0, 1);
        break;

    case DISP_CMD_LAYER_CK_OFF:
        ret = BSP_disp_layer_colorkey_enable(aux, para0, 0);
        break;

    case DISP_CMD_LAYER_SET_PIPE:
        ret = BSP_disp_layer_set_pipe(aux, para0, para1);
        break;

    case DISP_CMD_LAYER_GET_ALPHA_VALUE:
        ret = BSP_disp_layer_get_alpha_value(aux, para0);
        break;

    case DISP_CMD_LAYER_GET_ALPHA_EN:
        ret = BSP_disp_layer_get_alpha_enable(aux, para0);
        break;

    case DISP_CMD_LAYER_GET_CK_EN:
        ret = BSP_disp_layer_get_colorkey_enable(aux, para0);
        break;

    case DISP_CMD_LAYER_GET_PRIO:
        ret = BSP_disp_layer_get_piro(aux, para0);
        break;

    case DISP_CMD_LAYER_GET_PIPE:
        ret = BSP_disp_layer_get_pipe(aux, para0);
        break;

    case DISP_CMD_LAYER_SET_SMOOTH:
        ret = BSP_disp_layer_set_smooth(aux, para0, (__disp_video_smooth_t) para1);
        break;

    case DISP_CMD_LAYER_GET_SMOOTH:
        ret = BSP_disp_layer_get_smooth(aux, para0);
        break;

    case DISP_CMD_LAYER_SET_BRIGHT:
        ret = BSP_disp_layer_set_bright(aux, para0, para1);
        break;

    case DISP_CMD_LAYER_GET_BRIGHT:
        ret = BSP_disp_layer_get_bright(aux, para0);
        break;

    case DISP_CMD_LAYER_SET_CONTRAST:
        ret = BSP_disp_layer_set_contrast(aux, para0, para1);
        break;

    case DISP_CMD_LAYER_GET_CONTRAST:
        ret = BSP_disp_layer_get_contrast(aux, para0);
        break;

    case DISP_CMD_LAYER_SET_SATURATION:
        ret = BSP_disp_layer_set_saturation(aux, para0, para1);
        break;

    case DISP_CMD_LAYER_GET_SATURATION:
        ret = BSP_disp_layer_get_saturation(aux, para0);
        break;

    case DISP_CMD_LAYER_SET_HUE:
        ret = BSP_disp_layer_set_hue(aux, para0, para1);
        break;

    case DISP_CMD_LAYER_GET_HUE:
        ret = BSP_disp_layer_get_hue(aux, para0);
        break;

    case DISP_CMD_LAYER_ENHANCE_ON:
        ret = BSP_disp_layer_enhance_enable(aux, para0, 1);
        break;

    case DISP_CMD_LAYER_ENHANCE_OFF:
        ret = BSP_disp_layer_enhance_enable(aux, para0, 0);
        break;

    case DISP_CMD_LAYER_GET_ENHANCE_EN:
        ret = BSP_disp_layer_get_enhance_enable(aux, para0);
        break;


    //----scaler----
    case DISP_CMD_SCALER_REQUEST:
        if(BSP_disp_get_de_flicker(aux) == TRUE)
        {
            event_id = disp_cmd_before(aux);
            ret = BSP_disp_scaler_request();
            disp_cmd_after(aux, event_id);
        }
        else
        {
            ret = BSP_disp_scaler_request();
        }
        break;

    case DISP_CMD_SCALER_RELEASE:
        if(BSP_disp_get_de_flicker(aux) == TRUE)
        {
            event_id = disp_cmd_before(aux);
            ret = BSP_disp_scaler_release(para0);
            disp_cmd_after(aux, event_id);
        }
        else
        {
            ret = BSP_disp_scaler_release(para0);
        }
        break;

    case DISP_CMD_SCALER_EXECUTE:
        ret = BSP_disp_scaler_start(para0, (__disp_scaler_para_t *)para1);
        break;

    //----hwc----
    case DISP_CMD_HWC_OPEN:
        ret =  BSP_disp_hwc_enable(aux, 1);
        break;

    case DISP_CMD_HWC_CLOSE:
        ret =  BSP_disp_hwc_enable(aux, 0);
        break;

    case DISP_CMD_HWC_SET_POS:
        ret = BSP_disp_hwc_set_pos(aux, (__disp_pos_t *)para0);
        break;

    case DISP_CMD_HWC_GET_POS:
        ret = BSP_disp_hwc_get_pos(aux, (__disp_pos_t *)para0);
        break;

    case DISP_CMD_HWC_SET_FB:
        ret = BSP_disp_hwc_set_framebuffer(aux, (__disp_hwc_pattern_t *)para0);
        break;

    case DISP_CMD_HWC_SET_PALETTE_TABLE:
        ret = BSP_disp_hwc_set_palette(aux, (void *)para0, para1, para2);
        break;


    //----video----
    case DISP_CMD_VIDEO_START:
        ret = BSP_disp_video_start(aux, para0);
        break;

    case DISP_CMD_VIDEO_STOP:
        ret = BSP_disp_video_stop(aux, para0);
        break;

    case DISP_CMD_VIDEO_SET_FB:
        ret = BSP_disp_video_set_fb(aux, para0, (__disp_video_fb_t *)para1);
        break;

    case DISP_CMD_VIDEO_GET_FRAME_ID:
        ret = BSP_disp_video_get_frame_id(aux, para0);
        break;

    case DISP_CMD_VIDEO_GET_DIT_INFO:
        ret = BSP_disp_video_get_dit_info(aux, para0, (__disp_dit_info_t *)para1);
        break;

    //----lcd----
    case DISP_CMD_LCD_ON:
        DISP_switch2lcd_panel_funs_0(aux);
        ret = DRV_lcd_open(aux);
        break;

    case DISP_CMD_LCD_OFF:
        ret = DRV_lcd_close(aux);
        break;

    case DISP_CMD_LCD_SET_BRIGHTNESS:
        ret = BSP_disp_lcd_set_bright(aux, (__disp_lcd_bright_t)para0);
        break;

    case DISP_CMD_LCD_GET_BRIGHTNESS:
        ret = BSP_disp_lcd_get_bright(aux);
        break;

    case DISP_CMD_LCD_CPUIF_XY_SWITCH:
        ret = BSP_disp_lcd_xy_switch(aux, para0);
        break;

    case DISP_CMD_LCD_SET_SRC:
        ret = BSP_disp_lcd_set_src(aux, (__disp_lcdc_src_t)para0);
        break;

    //----tv----
    case DISP_CMD_TV_ON:
        ret = BSP_disp_tv_open(aux);
        break;

    case DISP_CMD_TV_OFF:
        ret = BSP_disp_tv_close(aux);
        break;

    case DISP_CMD_TV_SET_MODE:
        ret = BSP_disp_tv_set_mode(aux, (__disp_tv_mode_t)para0);
        break;

    case DISP_CMD_TV_GET_MODE:
        ret = BSP_disp_tv_get_mode(aux);
        break;

    case DISP_CMD_TV_AUTOCHECK_ON:
        ret = BSP_disp_tv_auto_check_enable(aux);
        break;

    case DISP_CMD_TV_AUTOCHECK_OFF:
        ret = BSP_disp_tv_auto_check_disable(aux);
        break;

    case DISP_CMD_TV_GET_INTERFACE:
        ret = BSP_disp_tv_get_interface(aux);
        break;

    case DISP_CMD_TV_SET_SRC:
        ret = BSP_disp_tv_set_src(aux, (__disp_lcdc_src_t)para0);
        break;

    case DISP_CMD_TV_GET_DAC_STATUS:
        ret =  BSP_disp_tv_get_dac_status(aux, para0);
        break;

    case DISP_CMD_TV_SET_DAC_SOURCE:
        ret =  BSP_disp_tv_set_dac_source(aux, para0, (__disp_tv_dac_source)para1);
        break;

    case DISP_CMD_TV_GET_DAC_SOURCE:
        ret =  BSP_disp_tv_get_dac_source(aux, para0);
        break;

    //----hdmi----
    case DISP_CMD_HDMI_ON:
        DISP_switch2hdmi_panel_funs_1(aux);

        ret = DRV_hdmi_open(aux, BSP_disp_hdmi_get_mode(aux));
        ret = BSP_disp_hdmi_open(aux);
        break;

    case DISP_CMD_HDMI_OFF:
        BSP_disp_hdmi_close(aux);
        ret = DRV_hdmi_close(aux);

        break;
    case DISP_CMD_HDMI_SET_MODE:
        ret = BSP_disp_hdmi_set_mode(aux, (__disp_tv_mode_t)para0);
        break;

    case DISP_CMD_HDMI_GET_MODE:
        ret = BSP_disp_hdmi_get_mode(aux);
        break;

    case DISP_CMD_HDMI_GET_HPD_STATUS:
        ret = BSP_disp_hdmi_get_hpd_status(aux);
        break;

    case DISP_CMD_HDMI_SUPPORT_MODE:
        ret = BSP_disp_hdmi_check_support_mode(aux, para0);
        break;

    case DISP_CMD_HDMI_SET_SRC:
        ret = BSP_disp_hdmi_set_src(aux, (__disp_lcdc_src_t)para0);
        break;


    default:
        __wrn("not supported display cmd:%x\n", cmd);
        ret = EPDK_FAIL;
    }
    return ret;
}

__dev_devop_t dispdev_ops =
{
    DEV_DISP_MOpen,
    DEV_DISP_MClose,
    DEV_DISP_MRead,
    DEV_DISP_MWrite,
    DEV_DISP_MIoctrl
};
