
/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: app_movie_i.h
* By      	: Bayden.chen
* Func		:
* Version	: v1.0
* ============================================================================================================
* 2011-05-05 Bayden.chen  create this file
**************************************************************************************************************
*/

#ifndef __APP_MOVIE_I_H__
#define  __APP_MOVIE_I_H__

#include "beetles_app.h"
#include "common/common.h"
#include "spsc/spsc_man.h"
#include "volume/volume.h"
#include "bright/bright.h"
#include "play_sta/play_sta.h"
#include "sub_show/sub_show.h"
#include "sub_set/sub_set.h"
#include "prog/prog.h"


//定义movie的子场景的ID
#define 	MOVIE_SPSC_ID			            0x1
#define 	MOVIE_VOLUME_ID			            0x2
#define 	MOVIE_BRIGHT_ID			            0x4
#define 	MOVIE_PLAYSTA_ID		            0x8
#define 	MOVIE_SUB_SHOW_ID		            0x10
#define 	MOVIE_SUB_SET_ID		            0x20
#define 	MOVIE_PROG_ID       	    		0x40
#define 	MOVIE_BREAKPLAY_DIALOG_ID			0x80
#define 	MOVIE_PLAYERROR_DIALOG_ID			0x100
#define 	MOVIE_PLAYEND_DIALOG_ID	    		0x200



//所有子场景id
#define		MOVIE_SUB_SCENE_TYPE_ALL   (MOVIE_SPSC_ID|MOVIE_VOLUME_ID|MOVIE_BRIGHT_ID|MOVIE_PLAYSTA_ID|MOVIE_SUB_SHOW_ID|MOVIE_SUB_SET_ID\
    |MOVIE_PROG_ID|MOVIE_BREAKPLAY_DIALOG_ID|MOVIE_PLAYERROR_DIALOG_ID|MOVIE_PLAYEND_DIALOG_ID)

//除了spsc场景和sub_show场景以外的场景id
#define		MOVIE_SUB_SCENE_TYPE_OTHER (MOVIE_SUB_SCENE_TYPE_ALL&(~MOVIE_SPSC_ID)\
    &(~MOVIE_SUB_SHOW_ID))

#endif
