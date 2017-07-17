
#ifndef __APP_FM_I_H__
#define  __APP_FM_I_H__

#include "apps.h"
#include "beetles_app.h"

#include "app_fm.h"

#include "fmtop_frmwin.h"

#define APP_FM_MOD   			"APP FM MOD"

#define FM_TOP_FRMID		(APP_FM_ID+1)

#define NOTIFY_MSG(mid, src, des, d1, d2) \
do{\
	__gui_msg_t  exmsg; \
	exmsg.id = mid; \
	exmsg.h_srcwin = src; \
	exmsg.h_deswin = des; \
	exmsg.dwAddData1= (__u32)d1; \
	exmsg.dwAddData2 = (__u32)d2; \
	GUI_SendNotifyMessage(&exmsg); \
	}while(0)

#define SEND_MSG(mid, src, des, d1, d2) \
do{\
	__gui_msg_t  exmsg; \
	exmsg.id = mid; \
	exmsg.h_srcwin = src; \
	exmsg.h_deswin = des; \
	exmsg.dwAddData1= (__u32)d1; \
	exmsg.dwAddData2 = (__u32)d2; \
	GUI_SendMessage(&exmsg); \
	}while(0)

#define POST_MSG(mid, src, des, d1, d2) \
do{\
	__gui_msg_t  exmsg; \
	exmsg.id = mid; \
	exmsg.h_srcwin = src; \
	exmsg.h_deswin = des; \
	exmsg.dwAddData1= (__u32)d1; \
	exmsg.dwAddData2 = (__u32)d2; \
	GUI_PostMessage(&exmsg); \
	}while(0)

typedef enum
{
    COMMAND_SWITCH_MMENU = 0,
    COMMAND_SWITCH_RECORD,
    COMMAND_AUTOSRH_START,
    COMMAND_MANUALSRH_START,

} __fmplay_cmd_t;

typedef enum
{
    CMD_AUTOSRH_FINDCH = GUI_MSG_USER_DEF,
    CMD_AUTOSRH_FINDCHFAIL,
    CMD_AUTOSRH_OVER,

} __fmplay_msg_t;

#endif

