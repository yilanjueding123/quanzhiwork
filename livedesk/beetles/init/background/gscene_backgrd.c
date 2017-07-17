/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: gscene_backgrd.c
* By      	: Andy.zhang
* Func		: desktop toolbar control interface
* Version	: v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "mod_init_i.h"
#include ".\fb_lib\backlayer_lib.h"
#include ".\fb_lib\fb_file.h"

#define BG_MAX_CHAR_LEN		768

#define BG_DEFAULT_PATH     BEETLES_APP_ROOT"apps\\"

typedef struct tag_bgd_com
{
    ES_FILE 	*de_hdle;				// display driver
    FB 	 		fb;						// FB
    //__disp_fb_t	fb;
    __u8 		*buffer;				// FB ¶ÔÓ¦µÄ buffer
    __s32		len;					// buffer length;
    __s32		layer_hdle; 			// Í¼²ã¾ä±ú
    bgd_status_t status;				// Í¼²ã×´Ì¬
    SIZE		screen_size;			// Í¼²ã³ß´ç
    char		filename[BG_MAX_CHAR_LEN];	// ±³¾°Í¼Æ¬ÎÄ¼þÃû³Æ
} bgd_com_t;

static bgd_com_t *bgd_com = NULL;

/**********************************************************************************************************************/
static __s32   gscene_bgd_get_default_file(char *filename);
static __s32   gscene_bgd_get_setting_file(char *filename);
static void   gscene_bgd_set_status_show(void);
static void   gscene_bgd_set_status_hide(void);
static __s32  gscene_bgd_is_file_exist(char *file);
static __s32 gscene_bgd_set_setting_file(char *filename);
static __s32 gscene_bgd_update_filename(void);


/*»ñµÃÏÔÊ¾Ä¬ÈÏ±³¾°»¹ÊÇÓÃ»§ÉèÖÃµÄ±³¾°µÄ±ê¼Ç
 flag : EPDK_TRUE : Ä¬ÈÏ±³¾°Í¼Æ¬
 	  : EPDK_FALSE: ÉèÖÃ±³¾°Í¼Æ¬
 */
__bool gscene_bgd_get_flag(void)
{
    __bool  ret;
    reg_init_para_t *para;

    para = (reg_init_para_t *)dsk_reg_get_para_by_app(REG_APP_INIT);
    if(para)
    {
        ret = para->bgd_flag;
    }
    else
    {
        ret = EPDK_TRUE;
    }

    return ret;
}

/*ÉèÖÃÏÔÊ¾Ä¬ÈÏ±³¾°»¹ÊÇÓÃ»§ÉèÖÃµÄ±³¾°µÄ±ê¼Ç
 flag : EPDK_TRUE : Ä¬ÈÏ±³¾°Í¼Æ¬
 	  : EPDK_FALSE: ÉèÖÃ±³¾°Í¼Æ¬
 */
void gscene_bgd_set_flag(__bool flag)
{
    reg_init_para_t *para;

    para = (reg_init_para_t *)dsk_reg_get_para_by_app(REG_APP_INIT);
    if(para)
    {
        para->bgd_flag = flag;
        //dsk_reg_flush();
    }
    else
    {
        __msg("dsk_reg_get_default_para_by_app fail...\n");
    }
}


/**********************************************************************************************************************/
__s32 gscene_bgd_init(SIZE *p_size, bgd_status_t status, __fb_type_t ftype)
{
    bgd_com = (bgd_com_t *)esMEMS_Malloc(0, sizeof(bgd_com_t));
    if( !bgd_com )
    {
        __err(" background com handle malloc error \n");
        return -1;
    }

    eLIBs_memset(bgd_com, 0, sizeof(bgd_com_t));

    gscene_bgd_update_filename();

    bgd_com->de_hdle = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
    if (bgd_com->de_hdle == ((ES_FILE *)0) )
    {
        __err("can not open display driver\n");
        return -1;
    }

    bgd_com->screen_size.width  = p_size->width;
    bgd_com->screen_size.height = p_size->height;
    bgd_com->fb.fmt.type = ftype;

    if( bgd_com->fb.fmt.type == FB_TYPE_YUV )
    {
        bgd_com->len = 2 * (p_size->width) * (p_size->height) / 1024;
    }
    else
    {
        bgd_com->len = 4 * (p_size->width) * (p_size->height) / 1024;
    }

    if( status == BGD_STATUS_SHOW )
    {
        gscene_bgd_set_status_show();
    }
    else
        gscene_bgd_set_status_hide();

    return 0;
}

static void gscene_bgd_set_status_show(void)
{
    gscene_bgd_update_filename();

    if( !bgd_com->buffer )
    {
        bgd_com->buffer = esMEMS_Palloc( bgd_com->len + 1, 0 );
        get_fb_from_file(&(bgd_com->fb), bgd_com->buffer, &(bgd_com->screen_size), BACKLAYER_MOD_RATIO, (__u8 *)bgd_com->filename);
    }
    if( !bgd_com->layer_hdle )
    {
        bgd_com->layer_hdle = backlayer_create_layer(bgd_com->de_hdle);
        backlayer_set_bottom(bgd_com->de_hdle, bgd_com->layer_hdle);
    }

    //	backlayer_set_fb(bgd_com->de_hdle, bgd_com->layer_hdle, &(bgd_com->fb), BACKLAYER_MOD_RATIO);
    backlayer_set_fb(bgd_com->de_hdle, bgd_com->layer_hdle, &(bgd_com->fb), BACKLAYER_MOD_STRETCH);

    bgd_com->status = BGD_STATUS_SHOW;
}

static void gscene_bgd_set_status_hide(void)
{
    if(bgd_com->layer_hdle)
    {
        backlayer_delete_layer(bgd_com->de_hdle, bgd_com->layer_hdle);
        bgd_com->layer_hdle = 0;
    }

    if( bgd_com->buffer )
    {
        esMEMS_Pfree( bgd_com->buffer, bgd_com->len + 1 );
        bgd_com->buffer = 0;
    }

    bgd_com->status = BGD_STATUS_HIDE;
}

void gscene_bgd_set_state(bgd_status_t status)
{
    if( bgd_com->status == status )
    {
        return;
    }
    else
    {
        if( status == BGD_STATUS_SHOW )
            gscene_bgd_set_status_show();
        else
            gscene_bgd_set_status_hide();
    }

    gscene_bgd_set_bottom();
}

bgd_status_t gscene_bgd_get_state(void)
{
    return bgd_com->status;
}

void gscene_bgd_set_top(void)
{
    if( bgd_com->status == BGD_STATUS_SHOW )
    {
        backlayer_set_top(bgd_com->de_hdle, bgd_com->layer_hdle);
    }
}

void gscene_bgd_set_bottom(void)
{
    if( bgd_com->status == BGD_STATUS_SHOW )
    {
        backlayer_set_bottom(bgd_com->de_hdle, bgd_com->layer_hdle);
    }
}

__s32 gscene_bgd_save_fb(FB *fb, char *path)
{
    __s32 ret;

    ret = create_fb_file( fb, (__u8 *)path);
    if(EPDK_FAIL == ret)
    {
        __msg("create_fb_file fail...\n");
        return EPDK_FAIL;
    }

    ret = gscene_bgd_set_setting_file(path);
    if(EPDK_FAIL == ret)
    {
        __msg("gscene_bgd_set_setting_file fail...\n");
        return EPDK_FAIL;
    }
    eLIBs_strcpy(bgd_com->filename, path);
    gscene_bgd_set_flag(EPDK_FALSE);

    {
        reg_init_para_t *para;

        para = (reg_init_para_t *)dsk_reg_get_para_by_app(REG_APP_INIT);
        if(para)
        {
            para->bgd_flag_in_main = 0;
            //dsk_reg_flush();
        }
        else
        {
            __msg("dsk_reg_get_default_para_by_app fail...\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

void gscene_bgd_set_fb_type(__fb_type_t ftype)
{
    if( bgd_com->fb.fmt.type == ftype )
    {
        return;
    }

    bgd_com->fb.fmt.type = ftype;

    if( bgd_com->status == BGD_STATUS_HIDE)
    {
        if( bgd_com->fb.fmt.type == FB_TYPE_YUV )
        {
            bgd_com->len = 2 * (bgd_com->screen_size.width) * (bgd_com->screen_size.height) / 1024;
        }
        else
        {
            bgd_com->len = 4 * (bgd_com->screen_size.width) * (bgd_com->screen_size.height) / 1024;
        }

        return;
    }
    else
    {
        gscene_bgd_set_status_hide();
        if( bgd_com->fb.fmt.type == FB_TYPE_YUV )
        {
            bgd_com->len = 2 * (bgd_com->screen_size.width) * (bgd_com->screen_size.height) / 1024;
        }
        else
        {
            bgd_com->len = 4 * (bgd_com->screen_size.width) * (bgd_com->screen_size.height) / 1024;
        }
        gscene_bgd_set_status_show();
        return;
    }
}

void gscene_bgd_refresh(void)
{

    gscene_bgd_set_status_hide();

    gscene_bgd_set_status_show();
    /*
    void* ptmp_buf;

    if(BGD_STATUS_SHOW != bgd_com->status)
    {
        __msg("background not in show status...\n");
        return ;
    }

    //Èç¹ûÃ»ÉêÇëÄÚ´æ£¬ÔòÉêÇëÄÚ´æ
    if( !bgd_com->buffer )
    {
    	bgd_com->buffer = esMEMS_Palloc( bgd_com->len + 1, 0 );
        if(!bgd_com->buffer)
        {
            __msg("mem not enough...\n");
            return;
        }
    }

    //Èç¹ûÃ»ÉêÇëÏÔÊ¾Í¼²ã£¬ÔòÉêÇëÏÔÊ¾Í¼²ã
    if( !bgd_com->layer_hdle )
    {
    	bgd_com->layer_hdle = backlayer_create_layer(bgd_com->de_hdle);
        if(!bgd_com->layer_hdle)
        {
            __msg("backlayer_create_layer fail...\n");
            return ;
        }
    }
    backlayer_set_bottom(bgd_com->de_hdle, bgd_com->layer_hdle);

    //ÉêÇëbuffer£¬Ë«buffer±ÜÃâÉÁÆÁºÍºÚÆÁ
    {
        ptmp_buf = esMEMS_Palloc( bgd_com->len + 1, 0 );
        if(!ptmp_buf)
        {
            __msg("mem not enough...\n");
            return;
        }
    }

    {
        __s32 ret;
        FB fb;

        eLIBs_memcpy(&fb, &(bgd_com->fb), sizeof(FB));
        ret = get_fb_from_file(&fb, ptmp_buf, &(bgd_com->screen_size), BACKLAYER_MOD_RATIO, (__u8 *)bgd_com->filename);
        if(EPDK_FAIL == ret)
        {
            __msg("get_fb_from_file...\n");
            if(ptmp_buf)
            {
                esMEMS_Pfree(ptmp_buf, bgd_com->len + 1);
                ptmp_buf = NULL;
            }
            return ;
        }
        backlayer_set_fb(bgd_com->de_hdle, bgd_com->layer_hdle, &fb, BACKLAYER_MOD_RATIO);
        eLIBs_memcpy(&(bgd_com->fb), &fb, sizeof(FB));
        if(bgd_com->buffer)
        {
            esMEMS_Pfree(bgd_com->buffer, bgd_com->len + 1);
        }
        bgd_com->buffer = ptmp_buf;
    }
    */
}

void gscene_bgd_restore(void)
{
    __s32 ret;
    char file[BG_MAX_CHAR_LEN] = {0};

    ret = gscene_bgd_get_default_file(file);
    if(EPDK_FAIL == ret)
    {
        __msg("gscene_bgd_get_default_file fail...\n");
        return ;
    }

    eLIBs_strcpy(bgd_com->filename, file);
    gscene_bgd_set_flag(EPDK_TRUE);
}

__s32 gscene_bgd_deinit(void)
{
    if(bgd_com->layer_hdle)
    {
        backlayer_delete_layer(bgd_com->de_hdle, bgd_com->layer_hdle);
        bgd_com->layer_hdle = 0;
    }

    if( bgd_com->buffer )
    {
        esMEMS_Pfree( bgd_com->buffer, bgd_com->len + 1 );
        bgd_com->buffer = 0;
    }

    eLIBs_fclose(bgd_com->de_hdle);
    esMEMS_Mfree(0, bgd_com);

    bgd_com = NULL;

    return 0;
}

__s32 gscene_bgd_get_default_bg_index(void)
{
    reg_init_para_t *para;

    para = (reg_init_para_t *)dsk_reg_get_para_by_app(REG_APP_INIT);
    if(para)
    {
        return para->default_bgd_index;
    }
    else
    {
        __msg("dsk_reg_get_default_para_by_app fail...\n");
        return 0;
    }
}

__s32 gscene_bgd_set_default_bg_index(__u32 index)
{
    __here__;
    if(EPDK_TRUE != gscene_bgd_get_flag())//²»Ê¹ÓÃÄ¬ÈÏ±³¾°
    {
        __msg("not using default background...\n");
        return EPDK_FAIL;
    }
    __here__;
    if(index >= BG_MAX_NUM)
    {
        __msg("index too large, index=%d\n", index);
        return EPDK_FAIL;
    }
    __here__;
    if(gscene_bgd_get_default_bg_index() == index)
    {
        __msg("default_bg_index == index, not need change bg\n");
        return EPDK_OK;
    }
    __here__;

    {
        reg_init_para_t *para;

        para = (reg_init_para_t *)dsk_reg_get_para_by_app(REG_APP_INIT);
        if(para)
        {
            para->default_bgd_index = index;
            para->bgd_flag_in_main = 1;
            para->bgd_flag = 1;
            //dsk_reg_flush();
        }
        else
        {
            __msg("dsk_reg_get_default_para_by_app fail...\n");
            return EPDK_FAIL;
        }
    }

    {
        __s32 ret;

        ret = gscene_bgd_get_default_file(bgd_com->filename);
        if(EPDK_FAIL == ret)
        {
            __msg("gscene_bgd_get_default_file fail...\n");
            return EPDK_FAIL;
        }
    }

    __msg("change bg index=%d\n", index);

    gscene_bgd_refresh();

    return EPDK_OK;
}

static __s32 gscene_bgd_get_default_file(char *filename)
{
    __s32 index;
    char path[BG_MAX_CHAR_LEN] = {0};

    index = gscene_bgd_get_default_bg_index();

    eLIBs_sprintf(path, "%sbg_default%d.bgd", BG_DEFAULT_PATH, index);
    if(EPDK_TRUE == gscene_bgd_is_file_exist(path))
    {
        eLIBs_strcpy(filename, path);
        return EPDK_OK;
    }
    else
    {
        __msg("warning, bg file does not exist, index=%d, load index 0\n", index);
        eLIBs_sprintf(path, "%sbg_default%d.bgd", BG_DEFAULT_PATH, 0);
        if(EPDK_TRUE == gscene_bgd_is_file_exist(path))
        {
            eLIBs_strcpy(filename, path);
            return EPDK_OK;
        }
        else
        {
            __err("file not exist:%s\n", path);
            return EPDK_FAIL;
        }
    }
}

static __s32 gscene_bgd_set_setting_file(char *filename)
{
    reg_init_para_t *para;

    para = (reg_init_para_t *)dsk_reg_get_para_by_app(REG_APP_INIT);
    if(para)
    {
        eLIBs_strcpy(para->setting_bgd_path, filename);
        //dsk_reg_flush();
    }
    else
    {
        __msg("dsk_reg_get_default_para_by_app fail...\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


static __s32 gscene_bgd_get_setting_file(char *filename)
{
    __s32 index;
    char path[BG_MAX_CHAR_LEN] = {0};
    reg_init_para_t *para;

    para = (reg_init_para_t *)dsk_reg_get_para_by_app(REG_APP_INIT);
    if(para)
    {
        eLIBs_sprintf(path, "%s", para->setting_bgd_path);
    }
    else
    {
        __msg("dsk_reg_get_default_para_by_app fail...\n");
        return EPDK_FAIL;
    }

    if(EPDK_TRUE == gscene_bgd_is_file_exist(path))
    {
        eLIBs_strcpy(filename, path);
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}

static __s32  gscene_bgd_is_file_exist(char *file)
{
    ES_FILE *fp;

    fp = eLIBs_fopen(file, "rb");
    if(fp)
    {
        __msg("file is exist:%s\n", file);
        eLIBs_fclose(fp);
        return EPDK_TRUE;
    }
    else
    {
        __msg("file not exist:%s\n", file);
        return EPDK_FALSE;
    }
}

static __s32 gscene_bgd_update_filename(void)
{
    __bool flag;

    flag = gscene_bgd_get_flag();
    if( flag == EPDK_TRUE )
    {
        __s32 ret;

        ret = gscene_bgd_get_default_file(bgd_com->filename);
        if(EPDK_OK != ret)
        {
            __err("gscene_bgd_get_default_file fail...\n");
            return EPDK_FAIL;
        }
    }
    else
    {
        __s32 ret;

        ret = gscene_bgd_get_setting_file(bgd_com->filename);
        if(EPDK_OK != ret)
        {
            __msg("get setting file fail,  load default file...\n");

            ret = gscene_bgd_get_default_file(bgd_com->filename);
            if(EPDK_OK != ret)
            {
                __err("gscene_bgd_get_default_file fail...\n");
                return EPDK_FAIL;
            }
            gscene_bgd_set_flag(EPDK_TRUE);
        }
    }
    return EPDK_OK;
}

