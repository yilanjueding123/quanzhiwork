/*
*******************************************************************************************************************
*                                                         	Listmenu  Framework
*                                         			the display dirver support module
*
*                             				 Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*											               All Rights Reserved
*
*File Name： 	message.h
*
*Author： 		William Wang
*
*Version :   	1.1.0
*
*Date:  		2009-7-22
*
*Description : 	message definition and function implement
*
*Others : 		None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* William Wang     2009-7-22         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define GUI_MSG_USER   0x2000

typedef enum __GUI_MSGID
{
    GUI_MSG_CREATE = 0,
    GUI_MSG_DESTROY,
    GUI_MSG_PAINT,                                /* draw */
    GUI_MSG_CLOSE,
    GUI_MSG_QUIT,
    GUI_MSG_KEY,
    GUI_MSG_TOUCH,
    GUI_MSG_COMMAND,
    GUI_MSG_ENABLE,
    GUI_MSG_STSTEM,                                 /*广播消息，注册控件消息等，(GUI系统保留)*/
    GUI_MSG_NOTIFY_CHILD_DELETED,
    GUI_MSG_SET_FOCUS,
    GUI_MSG_SET_UNFOCUS,
    GUI_MSG_INVALID,
    GUI_MSG_TOUCHACTIVE,
    GUI_MSG_TOUCHINACTIVE,
    GUI_MSG_INITDIALOG,
    GUI_MSG_NCPAINT,
    GUI_MSG_ERASEBKGND,
    GUI_MSG_TIMER,
    //GUI_MSG_SINGLE,

    GUI_MSG_WIN_SLEEP = 100,
    GUI_MSG_WIN_WAKEUP,
    GUI_MSG_LYR_MOVE,

    GUI_MSG_KEYTONE,

    GUI_MSG_CONTROL = 0x1000,                       /*控件*/

    GUI_MSG_DESKTOP = 0x2000,



    GUI_MSG_USER_DEF = 0x3000,

    GUI_MSG_
} __gui_msgid_t;

typedef enum __GUI_MSG_KEYID
{
    GUI_MSG_KEY_UP = 0x0,
    GUI_MSG_KEY_DOWN,
    GUI_MSG_KEY_ENTER,
    GUI_MSG_KEY_RIGHT,
    GUI_MSG_KEY_LEFT,
    GUI_MSG_KEY_ESCAPE,
    GUI_MSG_KEY_VADD,
    GUI_MSG_KEY_VDEC,
    GUI_MSG_KEY_RISE,
    GUI_MSG_KEY_POWER,
    GUI_MSG_KEY_SOURCE,
    GUI_MSG_KEY_CLEAR,
    GUI_MSG_KEY_DISPLAY,
    GUI_MSG_KEY_NUM0,
    GUI_MSG_KEY_NUM1,
    GUI_MSG_KEY_NUM2,
    GUI_MSG_KEY_NUM3,
    GUI_MSG_KEY_NUM4,
    GUI_MSG_KEY_NUM5,
    GUI_MSG_KEY_NUM6,
    GUI_MSG_KEY_NUM7,
    GUI_MSG_KEY_NUM8,
    GUI_MSG_KEY_NUM9,
    GUI_MSG_KEY_REPEATE,
    GUI_MSG_KEY_BLOCK,
    GUI_MSG_KEY_PLAY_PAUSE,
    GUI_MSG_KEY_TITLE,
    GUI_MSG_KEY_MENU,
    GUI_MSG_KEY_SETUP,
    GUI_MSG_KEY_GOTO,
    GUI_MSG_KEY_SEL,
    GUI_MSG_KEY_SHIFT,
    GUI_MSG_KEY_DISC,
    GUI_MSG_KEY_ATT,
    GUI_MSG_KEY_POWEROFF,
    GUI_MSG_KEY_RST,
    GUI_MSG_KEY_CANCLE,
    GUI_MSG_KEY_ZOOM_UP,
    GUI_MSG_KEY_ZOOM_DOWN,
    GUI_MSG_KEY_HOLD,
    GUI_MSG_KEY_MODE,
    GUI_MASG_SINGLE0,
    GUI_MASG_SINGLE1,
    GUI_MASG_SINGLE2,
    GUI_MASG_SINGLE3,
    GUI_MASG_SINGLE4,
    GUI_MSG_KEY_SEETING,

	
    GUI_MSG_KEY_LONGUP = 0x60,
    GUI_MSG_KEY_LONGDOWN,
    GUI_MSG_KEY_LONGENTER,
    GUI_MSG_KEY_LONGRIGHT,
    GUI_MSG_KEY_LONGLEFT,
    GUI_MSG_KEY_LONGESCAPE,
    GUI_MSG_KEY_LONGVADD,
    GUI_MSG_KEY_LONGVDEC,
    GUI_MSG_KEY_LONGRISE,
    GUI_MSG_KEY_LONGPOWER,
    GUI_MSG_KEY_LONGSOURCE,
    GUI_MSG_KEY_LONGCLEAR,
    GUI_MSG_KEY_LONGDISPLAY,
    GUI_MSG_KEY_LONGNUM0,
    GUI_MSG_KEY_LONGNUM1,
    GUI_MSG_KEY_LONGNUM2,
    GUI_MSG_KEY_LONGNUM3,
    GUI_MSG_KEY_LONGNUM4,
    GUI_MSG_KEY_LONGNUM5,
    GUI_MSG_KEY_LONGNUM6,
    GUI_MSG_KEY_LONGNUM7,
    GUI_MSG_KEY_LONGNUM8,
    GUI_MSG_KEY_LONGNUM9,
    GUI_MSG_KEY_LONGREPEATE,
    GUI_MSG_KEY_LONGBLOCK,
    GUI_MSG_KEY_LONGPLAY_PAUSE,
    GUI_MSG_KEY_LONGTITLE,
    GUI_MSG_KEY_LONGMENU,
    GUI_MSG_KEY_LONGSETUP,
    GUI_MSG_KEY_LONGGOTO,
    GUI_MSG_KEY_LONGSEL,
    GUI_MSG_KEY_LONGSHIFT,
    GUI_MSG_KEY_LONGDISC,
    GUI_MSG_KEY_LONGATT,
    GUI_MSG_KEY_LONGPOWEROFF,
    GUI_MSG_KEY_LONGRST,
    GUI_MSG_KEY_LONGCANCLE,
    GUI_MSG_KEY_LONGZOOM_UP,
    GUI_MSG_KEY_LONGZOOM_DOWN,
    GUI_MSG_KEY_LONGHOLD,
    GUI_MSG_KEY_LONGSEETING,
} __gui_msg_keyid_t;

typedef enum __GUI_MSG_TOUCHID
{
    GUI_MSG_TOUCH_DOWN = 0x80,
    GUI_MSG_TOUCH_UP,
    GUI_MSG_TOUCH_MOVE,
    GUI_MSG_TOUCH_ZOOMIN,
    GUI_MSG_TOUCH_ZOOMOUT,
    GUI_MSG_TOUCH_NCDOWN,
    GUI_MSG_TOUCH_NCUP,
    GUI_MSG_TOUCH_NCMOVE,
    GUI_MSG_TOUCH_OVERDOWN,
    GUI_MSG_TOUCH_OVERUP,
    GUI_MSG_TOUCH_OVERMOVE,
    GUI_MSG_TOUCH_OVERZOOMIN,
    GUI_MSG_TOUCH_OVERZOOMOUT,
    GUI_MSG_TOUCH_LONGDOWN,
    GUI_MSG_TOUCH_OVERLONGDOWN
} __gui_msg_touchid_t;

typedef struct __GUI_KEY_INFO
{
    __u32  key;                                     /* key value                                                    */
    __u32  presscnt;
} __gui_keyinfo_t;


typedef struct __GUI_MSG
{
    __u32       id;
    H_WIN       h_srcwin;
    H_WIN       h_deswin;
    __u32		dwAddData1;
    __u32		dwAddData2;
    __u32       dwReserved;
    void       *p_arg;
} __gui_msg_t;

typedef __gui_msg_t  *pmsg;

/*子窗口用来向父窗口的通知消息结构*/
typedef struct __GUI_NOTIFY_MSG
{
    H_WIN        hWnd;                       /*子窗口句柄*/
    __u32        id;                         /*子窗口在父窗口中的唯一标识*/
    __u32        msgcode;                    /*通知消息的通知码*/
    __u32        dwAddData;                  /*通知消息的附加值*/
    __u32        dwReserved;                 /*通知消息的保留附加值*/
} __gui_notify_msg_t;

extern __bool   GUI_HavePendingMessage  (H_WIN hWnd);                           /**/
extern __s32    GUI_BroadcastMessage    (__gui_msg_t *pmsg);
extern __s32    GUI_PostSyncMessage     (__gui_msg_t *pmsg);
extern __s32    GUI_SendMessage         (__gui_msg_t *msg);
extern __s32    GUI_SendMessageTimeout  (__gui_msg_t *msg, __u32 timeout);
extern __s32    GUI_PostMessage         (__gui_msg_t *msg);
extern __s32    GUI_SendNotifyMessage   (__gui_msg_t *msg);
extern __s32    GUI_GetMessage          (__gui_msg_t *msg, H_WIN  hManWin);
extern __s32    GUI_GetMessageEx        (__gui_msg_t *msg, H_WIN  hManWin);
extern __s32    GUI_MsgSetRepeatTimes   (H_WIN  hManWin, __u32 count);
extern __bool   GUI_QueryMessage        (H_WIN  hManWin);
extern __s32    GUI_PeekMessage         (__gui_msg_t *msg, H_WIN  hManWin, __u32 len);
extern __s32    GUI_DispatchMessage     (__gui_msg_t *msg);
extern __s32    GUI_PostQuitMessage     (H_WIN  hWnd);
extern __s32    GUI_SendAsyncMessage    (__gui_msg_t *msg);
extern __s32    GUI_ThrowAwayMessages   (H_WIN hWnd);
extern __s32    GUI_SetSyncMsgRetVal    (__gui_msg_t *msg, __s32 ret);
extern __s32    GUI_PostSyncSem         (__gui_msg_t *msg);
#endif
