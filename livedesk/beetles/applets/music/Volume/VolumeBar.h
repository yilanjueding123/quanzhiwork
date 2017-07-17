/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
* File    	: 	music_player.h
*
* By      	: 	Libaiao
* Func	:
* Version	: 	v1.0
* ============================================================================================================
* 2010-5-25 15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __VOLUMEBAR_H__
#define __VOLUMEBAR_H__

#include "apps.h"
#define MUSIC_VOLBAR_WIN "volume bar win"

//VolumeBar UI 参数
typedef struct VolumeBarUI_s
{
    RECT Background;		//背景区域大小.(整个VolumeBar的区域大小)
    //VolumeBar会根据该参数创建图层
    __u32 BG_bmp_id;		//背景图片id
    //下面坐标参数都是相对于Background.(即从0,0开始)
    RECT LeftSpeaker;		//左边Icon坐标,
    __u32 LS_bmp_id;		//左边icon图片ID

    RECT BarBody;			//音量条body坐标信息
    __u32 BB_bmp_id;		//音量条body图片id,, 注意body宽度或高度应是point的整数倍.

    RECT Point;				//音量条进度点坐标信息
    __u32 PO_bmp_id;		//进度点图片ID

    RECT RightSpeaker;		//右边喇叭坐标信息
    __u32 RS_bmp_id;		//右边喇叭图片id

    RECT MuteSpeaker;		//静音图标
    __u32 MS_bmp_id;		//静音图标id号码

    RECT VolNumber;			//音量数值
    //for volume bar private, user doesn't have to initial.
} VolumeBarUI_t;

//VolumeBar控制变量参数
typedef struct VolumeBarCtrlVar_s
{
    __u32 ViewAlign;					//GUI_TA_HORIZONTAL = 水平显示音量条
    //GUI_TA_VERTICAL   = 垂直显示音量条
    __u32 vol_max;						//最大音量值.(若小于当前系统设置音量，则以该音量为准).
    __u32 AutoExitTime;					//自动退出时间，以Second为单位

    H_WIN hParent;						//父窗口句柄, should be manager win.
    H_LYR UserLayer;					//用户图层,当系统已经没有空闲图层通道时，需要传递该参数给VolumeBar(为了透明显示)
    __s32 Pipe;							//用户可以指定VolumeBar图层的通道.
    void *attr;							//用户参数指针
} VolumeBarCtrlVar_t;

//事件控制参数
typedef struct VolumeBarEvent_s
{
    __u32 VolAddKey;					//用户指定用那个键响应音量加动作. 比如 = GUI_MSG_KEY_RIGHT
    __u32 VolAddRepeatKey;				//响应重复键
    __u32 VolDecKey;					//用户指定用那个键响应音量减动作. 比如 = GUI_MSG_KEY_LEFT
    __u32 VolDecRepeatKey;				//响应重复键
    __u32 VolExitKey;					//退出音量控制键

    __s32 (*OnVolAdd)(void *Handle); 	//音量加时,用户的事件处理函数
    __s32 (*OnVolDec)(void *Handle);	//音量减时,用户的事件处理函?
    __s32 (*OnExit)(void *Handle);		//

} VolumeBarEvent_t;

typedef struct  VolumeBarPara_s
{
    VolumeBarUI_t  			UI;				//音量控制条  UI参数
    VolumeBarCtrlVar_t		CtrlVar;		//音量控制条  变量控制参数
    VolumeBarEvent_t        Event;			//音量控制条  事件处理参数
} VolumeBarPara_t;


typedef struct VolumeBar_s
{
    VolumeBarPara_t  Para;				//User传递进来的参数


    //以下为VoluemBar的私有参数

    H_WIN   H_Framewin;					//frame window
    H_LYR 	VB_Layer;					//VolumeBar图层，用户无需初始化

    __u32   TimerID;
    HTHEME 	BG_Handle;					//背景图片句柄
    void   *BG_bmp;					//背景图片地址

    HTHEME 	LS_Handle;					//LeftSpeaker图片句柄
    void   *LS_bmp;						//左边喇叭icon图片地址

    HTHEME 	BB_Handle;					//音量条body图片句柄
    void   *BB_bmp;						//音量条body图片buffer地址

    HTHEME 	PO_Handle;					//进度点句柄
    void   *PO_bmp; 					//进度点图片信息

    HTHEME 	RS_Handle;					//RightSpeaker icon 句柄
    void   *RS_bmp;						//右边喇叭图片地址

    HTHEME 	MS_Handle;					//mute speaker icon 句柄
    void   *MS_bmp;						//静音图标地址
} VolumeBar_t;

#define C_VOLUMEBAR_FRAMEWIN_ID			(APP_MUSIC_ID + 20)

#define	C_VB_AUTO_EXIT_TIME				100

extern VolumeBar_t *VolumeBarCreate(VolumeBarPara_t *Para);
extern __s32 VolumeBarDelete(VolumeBar_t *This);
extern __s32 VolumeBarSetFocus(VolumeBar_t *This);
extern __s32 VolumeBarShow(VolumeBar_t *This);

#endif
