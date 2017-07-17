/*
************************************************************************************************************************
*                                                   language resource
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*											       All Rights Reserved
*
* File Name   : dsk_theme.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.29
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.10.29        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_THEME_C__
#define  __DSK_THEME_C__


#include "..\\functions_i.h"


static HRES  hres = NULL;
static __u32 style = 0;


typedef struct _theme_t
{
    __u32  id;
    __u32  size;//原始的大小
    __u32  size_com;//存储的大小
    void  *buf;
} theme_t, *HTHEME_i;


static void  _release_theme_res( HTHEME_i htheme );


/*
****************************************************************************************************
*Name        : dsk_theme_init
*Prototype   : __s32 dsk_theme_init( const char *file )
*Arguments   : file    input. the theme resource file.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : init theme resource module.
*Other       :
****************************************************************************************************
*/
__s32 dsk_theme_init( const char *file )
{
    if( hres == NULL )
    {
        hres = OpenRes( (char *)file, 0);
        if( hres == NULL )
        {
            __err("Error in opening resource file \"%s\".\n", file);
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}


/*
****************************************************************************************************
*Name        : dsk_theme_exit
*Prototype   : void dsk_theme_exit( void )
*Arguments   : void
*Return      : void
*Description : exit theme resource module.
*Other       :
****************************************************************************************************
*/
void  dsk_theme_exit( void )
{
    if( hres != NULL )
    {
        CloseRes( hres );
        hres = NULL;
    }

}

/*
****************************************************************************************************
*Name        : dsk_theme_set_style
*Prototype   : __s32 dsk_theme_set_style( __u32 style_id )
*Arguments   : style_id    input. theme style id.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set theme style.
*Other       :
****************************************************************************************************
*/
__s32 dsk_theme_set_style( __u32 style_id )
{
    style = style_id;

    return EPDK_OK;
}


/*
****************************************************************************************************
*Name        : dsk_theme_get_style
*Prototype   : __s32 dsk_theme_get_style( void )
*Arguments   : void
*Return      : the current theme style id.
*Description : get the current theme style.
*Other       :
****************************************************************************************************
*/
__s32 dsk_theme_get_style( void )
{
    return style;
}


/*
****************************************************************************************************
*Name        : dsk_theme_open
*Prototype   : HTHEME dsk_theme_open( __u32 theme_id )
*Arguments   : theme_id   input. the ID of a theme
*Return      : the handle of a theme. NULL means error.
*Description : open a handle of a theme.
*Other       :
****************************************************************************************************
*/
HTHEME dsk_theme_open( __u32 theme_id )
{
    __s32	  ret;
    HTHEME_i  htheme = NULL;
    void	  *pbuf;

    if( hres == NULL )
    {
        return NULL;
    }

    htheme = (HTHEME_i) esMEMS_Malloc( 0, sizeof(theme_t) );
    if( htheme == NULL )
    {
        __err("Error in allocating memory.\n");
        return NULL;
    }
    eLIBs_memset( htheme, 0, sizeof(theme_t) );

    htheme->id = theme_id;
    htheme->size_com = GetResSize( hres, style, theme_id );
    pbuf = esMEMS_Balloc(htheme->size_com);
    if (NULL == pbuf)
    {
        __msg("*************mem not enough***********\n");
        goto error;
    }

    ret = GetRes(hres, 0, theme_id, pbuf, htheme->size_com);

    __msg("hres=%x,theme_id=%d,pbuf=%x,htheme->size_com=%d\n",
          hres, theme_id, pbuf, htheme->size_com);
    if (-1 == ret)
    {
        __msg("***********GetRes fail***********");
        goto error;
    }

    if(EPDK_TRUE == AZ100_IsCompress(pbuf, htheme->size_com))
    {
        htheme->size = AZ100_GetUncompressSize(pbuf, htheme->size_com);
    }
    else//未压缩
    {
        htheme->size = htheme->size_com;
    }

    if( htheme->size == 0 )
    {
        __wrn("Fail in getting the size of res %u.\n", htheme->id );
        goto error;
    }

    if (pbuf)
    {
        esMEMS_Bfree(pbuf, htheme->size_com);
        pbuf = NULL;
    }

    return (HTHEME) htheme;

error:
    _release_theme_res( htheme );
    if (pbuf)
    {
        esMEMS_Bfree(pbuf, htheme->size_com);
        pbuf = NULL;
    }
    return NULL;
}



/*
****************************************************************************************************
*Name        : dsk_theme_close
*Prototype   : void  dsk_theme_close( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : void
*Description : close a handle of a theme.
*Other       :
****************************************************************************************************
*/
void  dsk_theme_close( HTHEME handle )
{
    if( handle != NULL )
    {
        _release_theme_res( (HTHEME_i) handle );
    }
}



/*
****************************************************************************************************
*Name        : dsk_theme_hdl2size
*Prototype   : __s32 dsk_theme_hdl2size( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : the size of a theme. return value <= 0, means error.
*Description : get the size of a theme.
*Other       :
****************************************************************************************************
*/
__s32 dsk_theme_hdl2size( HTHEME handle )
{
    HTHEME_i  htheme = (HTHEME_i) handle;

    if( handle == NULL )
    {
        __msg("handle is null, handle to size failed!");
        return -1;
    }

    return htheme->size;
}



/*
****************************************************************************************************
*Name        : dsk_theme_hdl2buf
*Prototype   : void *dsk_theme_hdl2buf( HTHEME handle )
*Arguments   : a handle of a theme
*Return      : the pointer to the buffer of a theme. NULL means error.
*Description : get the pointer to the buffer of a theme.
*Other       :
****************************************************************************************************
*/
void *dsk_theme_hdl2buf( HTHEME handle )
{
    __s32	  size;
    __s32	  ret;
    void	  *pbuf;
    HTHEME_i  htheme = (HTHEME_i) handle;

    if( handle == NULL )
    {
        __msg("handle is null, handle to buf failed!");
        return NULL;
    }
    if(htheme->buf != NULL)
    {
        return htheme->buf;
    }
    htheme->buf = (void *)esMEMS_Balloc( htheme->size );
    if( htheme->buf == NULL )
    {
        __err("Error in balloc size %x.\n", htheme->size );
        return NULL;
    }

    pbuf = esMEMS_Balloc( htheme->size_com );
    if (NULL == pbuf)
    {
        __msg("*************mem not enough***********\n");
        esMEMS_Bfree(htheme->buf, htheme->size);
        htheme->buf = NULL;
        return NULL;
    }

    ret = GetRes( hres, style, htheme->id, pbuf, htheme->size_com );
    if (-1 == ret)
    {
        __msg("***********GetRes fail***********\n");
        esMEMS_Bfree(htheme->buf, htheme->size);
        esMEMS_Bfree(pbuf, htheme->size_com);
        htheme->buf = NULL;
        pbuf = NULL;
        return NULL;
    }

    if(EPDK_TRUE == AZ100_IsCompress(pbuf, htheme->size_com))
    {
        __s32 ret;

        ret = AZ100_Uncompress(pbuf, htheme->size_com, htheme->buf, htheme->size);
        if(EPDK_FAIL == ret)
        {
            __msg("***********AZ100_Uncompress fail***********\n");
            esMEMS_Bfree(htheme->buf, htheme->size);
            esMEMS_Bfree(pbuf, htheme->size_com);
            htheme->buf = NULL;
            pbuf = NULL;
            return NULL;
        }
    }
    else
    {
        eLIBs_memcpy(htheme->buf, pbuf, htheme->size_com);
    }

    esMEMS_Bfree(pbuf, htheme->size_com);
    pbuf = NULL;
    return htheme->buf;
}



static void  _release_theme_res( HTHEME_i htheme )
{
    if( htheme != NULL )
    {
        if( htheme->buf != NULL )
        {
            esMEMS_Bfree( htheme->buf , htheme->size);
            htheme->buf = NULL;
        }
        esMEMS_Mfree( 0, htheme );
    }
}



#endif     //  ifndef __DSK_THEME_C__

/* end of dsk_theme.c */

