/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUI_CursorCrossSI.c
* Purpose : Defines the cross cursor, small inverted
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include <stdlib.h>
#include "gui_core_kernel_private.h"

GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapCrossSI =
{
    11,                  /* XSize */
    11,                  /* YSize */
    3,                   /* BytesPerLine */
    2,                   /* BitsPerPixel */
    GUI_Pixels_CrossS,   /* Pointer to picture data (indices) */
    &GUI_CursorPalI      /* Pointer to palette */
};


GUI_CONST_STORAGE GUI_CURSOR GUI_CursorCrossSI =
{
    &GUI_BitmapCrossSI, 5, 5
};


/*************************** End of file ****************************/
