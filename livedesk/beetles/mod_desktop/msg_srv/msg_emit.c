/*
**************************************************************************************************************
*											         ePDK
*						            the Easy Portable/Player Develop Kits
*									           desktop system
*
*						        	 (c) Copyright 2007-2010, ANDY, China
*											 All Rights Reserved
*
* File    	: msg_emit.c
* By      	: Andy.zhang
* Func		: system msg emit
* Version	: v1.0
* ============================================================================================================
* 2009-7-24 10:56:38 andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "mod_desktop_i.h"
//#include "bsp_rk.h"


#define 	SYS_MSGQ_SIZE			128

/**************************************************************************************************************/
typedef struct _msrv_msg_t
{
    __u32 type;
    __u32 id;
    __u32 data;
    __u32 reserved;
} __msrv_msg_t;

typedef struct _memit_ctr_t
{
    __krnl_event_t	*p_array_sem;					// 缓冲区互斥
    __krnl_event_t	*p_hook_sem;
    __krnl_event_t  *psys_msg_queue;				// msg 消息队列
    int 			sys_msg_counter;
    __msrv_msg_t	sys_msg_array[SYS_MSGQ_SIZE];	// msg 消息队列缓冲
    __u8 			ksrv_th_id;

    cb_key_hook 	key_hook;
    cb_tp_hook  	tp_hook;
    cb_init_hook 	init_hook;

    __hdle  		h_tpGraber;
    __hdle  		h_keyGraber;
} __memit_ctr_t;

static __memit_ctr_t emit_ctr;
static __u8			 msg_srv_tid;
static __s32		 last_touch_action;

/**************************************************************************************************************/
static __msrv_msg_t  	*get_msg_buf(__memit_ctr_t *emit);
static __s32 			tp_msg_cb(void *msg);
static __s32 			key_msg_cb(void *msg);
static void 			ksrv_msg_thread(void *arg);
/**************************************************************************************************************/
/**
 *  初始化 touch panel 消息传输通道
 */
__s32 msg_srv_init_tp_channel(__memit_ctr_t *emit)
{
    /* 创建 desktop_msg_queue */
    emit->sys_msg_counter = 0;
    emit->psys_msg_queue  = esKRNL_QCreate(SYS_MSGQ_SIZE);
    if(!emit->psys_msg_queue)
    {
        __err(" create sys_msg_queue error \n");
        return EPDK_FAIL;
    }

    /* 创建 p_array_sem */
    emit->p_array_sem = esKRNL_SemCreate(1);
    if( !emit->p_array_sem )
    {
        __err(" create p_array_sem error \n");
        return EPDK_FAIL;
    }
    emit->p_hook_sem = esKRNL_SemCreate(1);
    if( !emit->p_hook_sem )
    {
        __err(" create p_hook_sem error \n");
        return EPDK_FAIL;
    }
    /* 装载 touchpanel 驱动 */

    /*  注册回调函数 tp_msg_cb */
    /*{
    	ES_FILE *p_tpfile;
    	p_tpfile = eLIBs_fopen("b:\\HID\\TP", "r+");
    	if( !p_tpfile )
    	{
    		__err(" open touchpanel driver error \n");
    		return EPDK_FAIL;
    	}

    	eLIBs_fioctrl(p_tpfile, DRV_TP_CMD_REG, 0, (void *)tp_msg_cb);
    	eLIBs_fclose(p_tpfile);
    }*/
    {
        /* grab logical keyboard, binding event callback to input_subsystem */
        emit_ctr.h_tpGraber = esINPUT_LdevGrab(INPUT_LTS_DEV_NAME, (__pCBK_t)tp_msg_cb, 0, 0);
        if (emit_ctr.h_tpGraber == NULL)
        {
            __msg("grab logical touchpannel failed\n");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

__s32 msg_srv_init_key_channel(__memit_ctr_t *emit)
{
    __s32  LdevID;
    __log("------------------key_init------------------------------\n");
    /* grab logical keyboard, binding event callback to input_subsystem */
    emit_ctr.h_keyGraber = esINPUT_LdevGrab(INPUT_LKEYBOARD_DEV_NAME, (__pCBK_t)key_msg_cb, 0, 0);
    if (emit_ctr.h_keyGraber == NULL)
    {
        __msg("grab logical keyboard failed\n");
        return EPDK_FAIL;
    }

    __msg("emit_ctr.h_keyGraber=%d\n", emit_ctr.h_keyGraber);

    //get logical key device id
    LdevID = esINPUT_GetLdevID(emit_ctr.h_keyGraber);
    if (LdevID == -1)
    {
        eLIBs_printf("get logical id failed\n");
        return EPDK_FAIL;
    }
    __msg("LdevID=%d\n", LdevID);

    //设置时间间隔单位为1ms，
    if (esINPUT_LdevCtl(LdevID, INPUT_SET_REP_PERIOD, 500, NULL) != EPDK_OK)
    {
        eLIBs_printf("logical device ioctl failed\n");
        return EPDK_FAIL;
    }

    __here__;
    return EPDK_OK;
}

__s32 msg_srv_deinit_key_channel(__memit_ctr_t *emit)
{
    esINPUT_LdevRelease(emit_ctr.h_keyGraber);

    return EPDK_OK;
}

/**
 *  释放 touch panel 消息传输通道
 */
__s32 msg_srv_deinit_tp_channel(__memit_ctr_t *emit)
{
    __u8 err;

    /* 删除 psys_msg_queue */
    esKRNL_QDel(emit->psys_msg_queue, 0, &err);
    emit->psys_msg_queue = NULL;

    /* 删除 p_array_sem */
    esKRNL_SemDel(emit->p_array_sem, 0, &err);
    esKRNL_SemDel(emit->p_hook_sem, 0, &err);
    emit->p_array_sem = NULL;
    emit->p_hook_sem = NULL;

    emit->sys_msg_counter = 0;

    /* 注销回调函数 */
    /*{
    	ES_FILE  *p_tpfile;
    	p_tpfile = eLIBs_fopen("b:\\HID\\TP", "r+");
    	if( !p_tpfile )
    	{
    		__err(" open touchpanel driver error \n");
    		return EPDK_FAIL;
    	}
    	eLIBs_fioctrl(p_tpfile, DRV_TP_CMD_UNREG, 0, (void *)tp_msg_cb);
    	eLIBs_fclose(p_tpfile);
    }*/

    esINPUT_LdevRelease(emit_ctr.h_tpGraber);

    return EPDK_OK;
}

/**
 * 初始化系统消息队列消息传输通道
 */
__s32 msg_srv_init_ksrv_channel(__memit_ctr_t *emit)
{
    /* create ksrv msg thread  */
#ifdef SIM_PC_WIN
    __SAVE_MOD_EVN__
#endif
    emit->ksrv_th_id = esKRNL_TCreate(ksrv_msg_thread, (void *)emit, 0x400, KRNL_priolevel3);
#ifdef SIM_PC_WIN
    __RESTORE_MOD_EVN__
#endif
    if( emit->ksrv_th_id == 0 )
    {
        __err(" ksrv_msg_thread create error \n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/**
 * 释放系统消息队列消息传输通道
 */
__s32 msg_srv_deinit_ksrv_channel(__memit_ctr_t *emit)
{
    while( esKRNL_TDelReq(emit->ksrv_th_id) != OS_TASK_NOT_EXIST )
    {
        esKRNL_TimeDly(1);
    }

    return EPDK_OK;
}

static __msrv_msg_t  *get_msg_buf(__memit_ctr_t *emit)
{
    //__msg("sys_msg_counter = %d \n", sys_msg_counter);
    emit->sys_msg_counter = (emit->sys_msg_counter + 1) & (SYS_MSGQ_SIZE - 1);
    return &(emit->sys_msg_array[emit->sys_msg_counter]);
}

/**
 * 通过回调函数的方式取触摸屏消息
 */
static __s32 tp_msg_cb(void *msg)
{
    __msrv_msg_t	*pmsg;
    __u8 error;

    __u32 i;
    __input_event_packet_t *pPacket;
    __input_event_t        *pEventFrame;
    __input_event_t        pEvent_type;
    __input_event_t        pEvent_x;
    __input_event_t        pEvent_y;
    __input_event_t        pEvent_speed_dir;
    __input_event_t        pEvent_speed_val;
    __log("tp_msg_down\n");
    if (msg == NULL)
    {
        __msg("invalid argument for call back\n");
        return EPDK_FAIL;
    }

    /* dump packet events */
    pPacket = (__input_event_packet_t *)msg;
    if (pPacket->event_cnt <= 0 || pPacket->event_cnt > INPUT_MAX_EVENT_NR)
    {
        __msg("invalid event count number\n");
        return EPDK_FAIL;
    }
    //__inf("\n------------------------------------------------\n");
    for (i = 0; i < pPacket->event_cnt; i++)
    {
        pEventFrame = &(pPacket->events[i]);
        /*__inf("Event %d: type = %d, code = %d, value = %d\n", i + 1,
                                                                 pEventFrame->type,
                                                                 pEventFrame->code,
                                                                 pEventFrame->value
                                                                 );*/
        if(pEventFrame->type == EV_ABS)
        {
            if(pEventFrame->code == ABS_MISC)
            {
                eLIBs_memcpy(&pEvent_type, pEventFrame, sizeof(__input_event_t));
            }
            else if(pEventFrame->code == ABS_X)
            {
                eLIBs_memcpy(&pEvent_x, pEventFrame, sizeof(__input_event_t));
            }
            else if(pEventFrame->code == ABS_Y)
            {
                eLIBs_memcpy(&pEvent_y, pEventFrame, sizeof(__input_event_t));
            }
            else if(pEventFrame->code == ABS_RUDDER)
            {
                eLIBs_memcpy(&pEvent_speed_dir, pEventFrame, sizeof(__input_event_t));
            }
            else if(pEventFrame->code == ABS_BRAKE)
            {
                eLIBs_memcpy(&pEvent_speed_val, pEventFrame, sizeof(__input_event_t));
            }
        }
        else if(pEventFrame->type == EV_SYN)
        {
            break;
        }
    }
    //__inf("\n------------------------------------------------\n");

    esKRNL_SemPend(emit_ctr.p_array_sem, 0, &error);
    pmsg = get_msg_buf(&emit_ctr);
    esKRNL_SemPost(emit_ctr.p_array_sem);

    pmsg->type 	= GUI_MSG_TOUCH;
    switch( pEvent_type.value)
    {
    case EV_TP_PRESS_START:
    {
        pmsg->id 	= GUI_MSG_TOUCH_DOWN;
        pmsg->data 	= (pEvent_y.value << 16) + pEvent_x.value;
        pmsg->reserved = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
        last_touch_action = GUI_MSG_TOUCH_DOWN;
        break;
    }
    case EV_TP_PRESS_MOVE:
        pmsg->id 	= GUI_MSG_TOUCH_MOVE;
        pmsg->data 	= (pEvent_y.value << 16) + pEvent_x.value;
        pmsg->reserved = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
        last_touch_action = GUI_MSG_TOUCH_MOVE;
        break;
    case EV_TP_PINCH_IN:
        pmsg->id 	= GUI_MSG_TOUCH_ZOOMIN;
        pmsg->data 	= (pEvent_y.value << 16) + pEvent_x.value;
        pmsg->reserved = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
        last_touch_action = GUI_MSG_TOUCH_ZOOMIN;
        break;
    case EV_TP_PINCH_OUT:
        pmsg->id 	= GUI_MSG_TOUCH_ZOOMOUT;
        pmsg->data 	= (pEvent_y.value << 16) + pEvent_x.value;
        pmsg->reserved = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
        last_touch_action = GUI_MSG_TOUCH_ZOOMOUT;
        break;
    case EV_TP_PRESS_HOLD:
        pmsg->id 	= last_touch_action;
        pmsg->data 	= (pEvent_y.value << 16) + pEvent_x.value;
        pmsg->reserved = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
        break;

    case EV_TP_ACTION_NULL:
    case EV_TP_ACTION_CLICK:
    case EV_TP_ACTION_DOWN:
    case EV_TP_ACTION_UP:
    case EV_TP_ACTION_LEFT:
    case EV_TP_ACTION_RIGHT:
    case EV_TP_ACTION_ANTICLOCKWISE:
    case EV_TP_ACTION_CLOCKWISE:
    case EV_TP_ACTION_LD2RU:
    case EV_TP_ACTION_RU2LD:
    case EV_TP_ACTION_LU2RD:
    case EV_TP_ACTION_RD2LU:
        pmsg->id 	= GUI_MSG_TOUCH_UP;
        pmsg->data 	= (pEvent_y.value << 16) + pEvent_x.value;
        pmsg->reserved = (pEvent_speed_dir.value << 16) + pEvent_speed_val.value;
        break;
    default:
        __msg("cannot identify message: %x\n", pEvent_type.value);
        return EPDK_FALSE;
    }
    __msg("------------------------------------------------\n");
    __msg("pmsg->type = %d, pmsg->id = %d\n", pmsg->type, pmsg->id);
    __msg("x = %d, y = %d\n", LOSWORD(pmsg->data), HISWORD(pmsg->data));
    __msg("------------------------------------------------\n");
    esKRNL_QPost(emit_ctr.psys_msg_queue, pmsg);

    return EPDK_TRUE;
}

static __s32 key_msg_cb(void *msg)
{
    __msrv_msg_t	*pmsg;
    __u8 error;

    __u32 i;
    __input_event_packet_t *pPacket;
    __input_event_t        *pEventFrame;
    __log("key_msg_down\n");
    if (msg == NULL)
    {
        __msg("invalid argument for call back\n");
        return EPDK_FAIL;
    }

    __inf("1------------------------------------------------\n");

    //dump packet events
    pPacket = (__input_event_packet_t *)msg;
    if (pPacket->event_cnt > INPUT_MAX_EVENT_NR)
    {
        __msg("invalid event count number\n");
        return EPDK_FAIL;
    }
    __inf("2------------------------------------------------\n");
    for (i = 0; i < pPacket->event_cnt; i++)
    {
        pEventFrame = &(pPacket->events[i]);
        __msg("Event %d: type = %d, code = %d, value = %d\n", i + 1,
              pEventFrame->type,
              pEventFrame->code,
              pEventFrame->value
             );
        if(pEventFrame->type == EV_KEY)
        {
            __msg("pEventFrame->type == EV_KEY\n");

            esKRNL_SemPend(emit_ctr.p_array_sem, 0, &error);
            pmsg = get_msg_buf(&emit_ctr);
            esKRNL_SemPost(emit_ctr.p_array_sem);


            switch(pEventFrame->code)
            {
            case KPAD_MODE:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_MENU;
                break;
            }
			case KPAD_SETTING:
			{
				pmsg->type	= GUI_MSG_KEY;
				pmsg->id	= GUI_MSG_KEY_SEETING;
				break;
			}

#if 0
            case KPAD_NUM1:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_NUM1;
                break;
            }
            case KPAD_NUM2:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_NUM2;
                break;
            }
            case KPAD_NUM3:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_NUM3;
                break;
            }
            case KPAD_NUM4:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_NUM4;
                break;
            }
            case KPAD_NUM5:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_NUM5;
                break;
            }
            case KPAD_NUM6:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_NUM6;
                break;
            }
            case KPAD_NUM7:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_UP;
                break;
            }
            case KPAD_NUM8:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_DOWN;
                break;
            }
            case KPAD_NUM9:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_NUM9;
                break;
            }
#endif
            case KPAD_PREV:

            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_LEFT;
                break;
            }

            case KPAD_NEXT:

            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_RIGHT;
                break;
            }
            case KPAD_VOICEUP:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_VADD;
                break;
            }
            case KPAD_VOICEDOWN:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_VDEC;
                break;
            }
            case KPAD_SEL:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_ENTER;
                break;
            }
            case KPAD_BAND:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_ESCAPE;
                break;
            }
            case KPAD_POWER:
            {
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_POWER;
                break;
            }
            case KPAD_POWEROFF:
            {
                pmsg->type 	= DSK_MSG_POWER_OFF;
                break;
            }
            case KPAD_ENTER:
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_ENTER;
                break;
			case KPAD_RETURN:
                pmsg->type 	= GUI_MSG_KEY;
                pmsg->id 	= GUI_MSG_KEY_ESCAPE;
                break;
				
            case KPAD_SINGLE0:
                //pmsg->type 	= GUI_MSG_KEY;//GUI_MSG_SINGLE;
                //pmsg->id 	= GUI_MASG_SINGLE0;
                break;
            case KPAD_SINGLE1:
                //pmsg->type 	= GUI_MSG_KEY;//GUI_MSG_SINGLE;
                //pmsg->id 	= GUI_MASG_SINGLE1;
                break;
            case KPAD_SINGLE2:
                //pmsg->type 	= GUI_MSG_KEY;// GUI_MSG_SINGLE;
                //pmsg->id 	= GUI_MASG_SINGLE2;
                break;
            case KPAD_SINGLE3:
                //pmsg->type 	= GUI_MSG_KEY;//GUI_MSG_SINGLE;
                //pmsg->id 	= GUI_MASG_SINGLE3;
                break;
            case KPAD_SINGLE4:
                //pmsg->type 	= GUI_MSG_KEY;//GUI_MSG_SINGLE;
                //pmsg->id 	= GUI_MASG_SINGLE4;
                break;
            default:
                break;
            }

            pmsg->data 	= pEventFrame->value;

            __msg("GUI_MSG_KEY_ val=%d, pmsg->type=%d\n", pmsg->id, pmsg->type);

            esKRNL_QPost(emit_ctr.psys_msg_queue, pmsg);
        }
    }
    __inf("3------------------------------------------------\n");

    return EPDK_TRUE;
}

/**
 * 从系统消息队列取消息
 */
static void ksrv_msg_thread(void *arg)
{
    __memit_ctr_t *emit = (__memit_ctr_t *)arg;
    __u8  error;

    /* 清空按键消息队列 */
    while(1)
    {
        __u32           usrmsg;

        //usrmsg = esKSRV_GetLowMsg();
        usrmsg = esKSRV_GetMsg();
        if( usrmsg == 0 )
            break;
    }

    while(1)
    {
        __u32           usrmsg;
        __msrv_msg_t	*pmsg;
        __u32 			tmp;

        while(1)
        {
            if( esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ )
            {
                esKRNL_TDel(OS_PRIO_SELF);
            }

            usrmsg = esKSRV_GetMsg();				// 系统消息队列
            if(usrmsg)
                break;
            esKRNL_TimeDly(2);
        }

        esKRNL_SemPend(emit->p_array_sem, 0, &error);
        pmsg = get_msg_buf(emit);
        //eLIBs_memset(pmsg, 0, sizeof(__msrv_msg_t));
        esKRNL_SemPost(emit->p_array_sem);

        if( (usrmsg & 0xffff0000) == KMSG_USR_CLS_KEY) 	// key 按键消息
        {
            tmp = usrmsg & 0x0000ffff;
            __msg(" usrmsg = %d \n", tmp);
            switch (usrmsg)
            {

#if( EPDK_CASE == EPDK_LIVE_BOX )
            case KMSG_USR_KEY_POWER:				/* standby	*/
                pmsg->type = DSK_MSG_STANDBY;
                break;

            case KMSG_USR_KEY_REPEATE:				/* 切换到色差输出 */
                pmsg->type = DSK_MSG_SWITCH_YPBPR;
                break;

            case KMSG_USR_KEY_CLEAR:				/* 切换到cvbs输出 */
                pmsg->type = DSK_MSG_SWITCH_CVBS;
                break;

            case KMSG_USR_KEY_DISPLAY:				/* 切换到hdmi输出 */
                pmsg->type = DSK_MSG_SWITCH_HDMI;
                break;

            case KMSG_USR_KEY_SHIFT:
                pmsg->type = GUI_MSG_KEY;
                pmsg->id	= GUI_MSG_KEY_SHIFT;
                break;

            case KMSG_USR_KEY_SEL:
                pmsg->type = GUI_MSG_KEY;
                pmsg->id	= GUI_MSG_KEY_SEL;
                break;

            case KMSG_USR_KEY_NUM4:
                pmsg->type = DSK_MSG_SWITCH_AUDIO_IF;
                pmsg->data = AUDIO_DEV_IF_CODEC;
                break;

            case KMSG_USR_KEY_NUM5:
                pmsg->type = DSK_MSG_SWITCH_AUDIO_IF;
                pmsg->data = AUDIO_DEV_IF_IIS;
                break;

            case KMSG_USR_KEY_NUM6:
                pmsg->type = DSK_MSG_SWITCH_AUDIO_IF;
                pmsg->data = AUDIO_DEV_IF_SPDIF;
                break;

#endif

#if( EPDK_CASE == EPDK_LIVE_TOUCH )
            case KMSG_USR_KEY_NUM8:					/* hold 按键 */
                pmsg->type = DSK_MSG_HOLD;
                break;

            case KMSG_USR_KEY_NUM7:								// 禁音功能
                pmsg->type = DSK_MSG_BAN_VOLUME;
                pmsg->data = 0;
                break;

#endif

            case KMSG_USR_KEY_GOTO:
                pmsg->type = DSK_MSG_APP_EXIT;		/* 一键回主界面 */
                break;

            case KMSG_USR_KEY_POWEROFF:				// 关机消息
                pmsg->type = DSK_MSG_POWER_OFF;
                break;

            case KMSG_USR_KEY_NUM9:
                pmsg->type = GUI_MSG_KEY;
                pmsg->id	= GUI_MSG_KEY_ESCAPE;
                break;

            case KMSG_USR_KEY_VOICE_UP:
                pmsg->type = GUI_MSG_KEY;
                pmsg->id	= GUI_MSG_KEY_VADD;
                break;

            case KMSG_USR_KEY_VOICE_DOWN:
                pmsg->type    = GUI_MSG_KEY;
                pmsg->id	   = GUI_MSG_KEY_VDEC;
                break;

            case KMSG_USR_KEY_UP:
                pmsg->type    = GUI_MSG_KEY;
                pmsg->id	   = GUI_MSG_KEY_UP;
                break;

            case KMSG_USR_KEY_DOWN  :
                pmsg->type    = GUI_MSG_KEY;
                pmsg->id	   = GUI_MSG_KEY_DOWN;
                break;

            case KMSG_USR_KEY_LEFT  :
                pmsg->type    	= GUI_MSG_KEY;
                pmsg->id	    = GUI_MSG_KEY_LEFT;
                break;

            case KMSG_USR_KEY_RIGHT :
                pmsg->type    	= GUI_MSG_KEY;
                pmsg->id	    = GUI_MSG_KEY_RIGHT;
                break;

            case KMSG_USR_KEY_ENTER:
                pmsg->type    	= GUI_MSG_KEY;
                pmsg->id	    = GUI_MSG_KEY_ENTER;
                break;

            case KMSG_USR_KEY_RISE:
                pmsg->type    	= GUI_MSG_KEY;
                pmsg->id	    = GUI_MSG_KEY_RISE;
                break;

            default:
                continue;
            }
        }
        else	/* system 消息 */
        {
            if( (usrmsg & 0x0000ffff) == KMSG_USR_SYSTEM_FS_PLUGIN )
            {
                pmsg->type = DSK_MSG_FS_PART_PLUGIN;
				//eLIBs_printf("pmsg->type: DSK_MSG_FS_PART_PLUGIN\n");
                pmsg->data = (usrmsg & KMSG_USR_SYSTEM_FS_PARA_MASK) >> 16;
            }
            else if( (usrmsg & 0x0000ffff) == KMSG_USR_SYSTEM_FS_PLUGOUT )
            {
                pmsg->type = DSK_MSG_FS_PART_PLUGOUT;
				//eLIBs_printf("pmsg->type: DSK_MSG_FS_PART_PLUGOUT\n");
                pmsg->data = (usrmsg & KMSG_USR_SYSTEM_FS_PARA_MASK) >> 16;
            }
            else
            {
                __msg("*************usrmsg = %d***************\n", usrmsg);
                switch (usrmsg)
                {
                case KMSG_USR_SYSTEM_USBD_PLUGIN:		// usb device plug in
                    __msg("case KMSG_USR_SYSTEM_USBD_PLUGIN\n");
                    pmsg->type = DSK_MSG_USBD_PLUG_IN;
                    break;

                case KMSG_USR_SYSTEM_USBD_PLUGOUT:    	// usb device plug out
                    pmsg->type = DSK_MSG_USBD_PLUG_OUT;
                    __msg("case KMSG_USR_SYSTEM_USBD_PLUGOUT\n");
                    break;

                case KMSG_USR_SYSTEM_SDMMC_PLUGOUT:
                    pmsg->type = DSK_MSG_APP_EXIT;
                    break;

                case KMSG_USR_SYSTEM_USBH_PLUGOUT:		// usb host 设备拔出
                    pmsg->type = DSK_MSG_APP_EXIT;
                    break;

                case KMSG_USR_SYSTEM_TVDAC_PLUGIN:
                    __msg("**********KMSG_USR_SYSTEM_TVDAC_PLUGIN***********\n");
                    pmsg->type = DSK_MSG_TVDAC_PLUGIN;
                    break;

                case KMSG_USR_SYSTEM_TVDAC_PLUGOUT:
                    __msg("***********KMSG_USR_SYSTEM_TVDAC_PLUGOUT***************\n");
                    pmsg->type = DSK_MSG_TVDAC_PLUGOUT;
                    break;

                case KMSG_USR_SYSTEM_HDMI_PLUGIN:
                    __msg("**********KMSG_USR_SYSTEM_HDMI_PLUGIN***********\n");
                    pmsg->type = DSK_MSG_HDMI_PLUGIN;
                    break;

                case KMSG_USR_SYSTEM_HDMI_PLUGOUT:
                    __msg("***********KMSG_USR_SYSTEM_HDMI_PLUGOUT***************\n");
                    pmsg->type = DSK_MSG_HDMI_PLUGOUT;
                    break;

#if( EPDK_CASE == EPDK_LIVE_TOUCH )
                case KMSG_USR_SYSTEM_WAKEUP:
                    __here__
                    pmsg->type = DSK_MSG_STANDBY_WAKE_UP;
                    break;
#endif

                default:
                    continue;
                }
            }
        }

        esKRNL_QPost(emit->psys_msg_queue, pmsg);
    }
}

/**
 *
 */
__s32 msg_srv_get_message(__memit_ctr_t *emit, __msrv_msg_t *p_msg)
{
    __u8 error;
    __msrv_msg_t *tmp;

    tmp = (__msrv_msg_t *)esKRNL_QAccept( emit->psys_msg_queue, &error);

    if( tmp != NULL)
    {
        memcpy( p_msg, tmp, sizeof(__msrv_msg_t) );
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }

}

/**
 * 发送消息到桌面系统消息队列
 */
__s32 msg_srv_dispatch_message(__memit_ctr_t *emit, __msrv_msg_t *p_msg)
{
    H_WIN			h_win;
    __gui_msg_t     msg;
    int 			x, y;
    __u8			err;

    if( p_msg->type == GUI_MSG_TOUCH)
    {
        msg.id 			= p_msg->type;
        msg.h_srcwin 	= NULL;
        msg.dwAddData1	= p_msg->id;
        msg.dwAddData2	= p_msg->data;
        msg.dwReserved	= p_msg->reserved;
        msg.p_arg		= NULL;

        x = LOSWORD(msg.dwAddData2);
        y = HISWORD(msg.dwAddData2);

        h_win = GUI_GetTouchFocusWin(x, y);	/* 位于窗口范围内, 则发送到窗口 */

        if( h_win )
        {
            __msg("%d:h_win = %x\n", __LINE__, h_win);
            msg.h_srcwin = NULL;
            msg.h_deswin = h_win;
        }
        else								/* 不在任何窗口区域, 则发送到焦点窗口 */
        {
            h_win = GUI_GetActiveManWin();
            __msg("%d:h_win = %x\n", __LINE__, h_win);
            if( h_win )
            {
                switch( msg.dwAddData1 )
                {
                case GUI_MSG_TOUCH_DOWN:
                    msg.dwAddData1 = GUI_MSG_TOUCH_OVERDOWN;
                    break;
                case GUI_MSG_TOUCH_UP:
                    msg.dwAddData1 = GUI_MSG_TOUCH_OVERUP;
                    break;
                case GUI_MSG_TOUCH_MOVE:
                    msg.dwAddData1 = GUI_MSG_TOUCH_OVERMOVE;
                    break;
                case GUI_MSG_TOUCH_ZOOMIN:
                    msg.dwAddData1 = GUI_MSG_TOUCH_OVERZOOMIN;
                    break;
                case GUI_MSG_TOUCH_ZOOMOUT:
                    msg.dwAddData1 = GUI_MSG_TOUCH_OVERZOOMOUT;
                    break;
                default:
                    break;
                }

                msg.h_srcwin = NULL;
                msg.h_deswin = h_win;
            }
            else
            {
                msg.h_deswin = NULL;
            }
        }

        esKRNL_SemPend(emit->p_hook_sem, 0, &err);
        if( emit->tp_hook )
        {
            (emit->tp_hook)(&msg);
        }
        esKRNL_SemPost(emit->p_hook_sem);
    }
    else if(( p_msg->type == GUI_MSG_KEY)/*||(p_msg->type ==GUI_MSG_SINGLE)*/ )
    {
        msg.id 			= p_msg->type;
        msg.h_srcwin 	= NULL;
        msg.h_deswin	= GUI_GetActiveManWin();
        msg.dwAddData1	= p_msg->id;
        msg.dwAddData2	= p_msg->data;
        msg.p_arg		= NULL;

        esKRNL_SemPend(emit->p_hook_sem, 0, &err);
        if( emit->key_hook )
        {
            (emit->key_hook)(&msg);
        }
        esKRNL_SemPost(emit->p_hook_sem);
    }
    else if ( (p_msg->type >= DSK_MSG_START) && ( p_msg->type <= DSK_MSG_LAST))
    {
        msg.id 			= p_msg->type;
        msg.h_srcwin 	= NULL;
        msg.h_deswin 	= GUI_GetActiveManWin();
        msg.dwAddData1	= 0;
        msg.dwAddData2	= p_msg->data;
        msg.p_arg		= NULL;

        esKRNL_SemPend(emit->p_hook_sem, 0, &err);
        if( emit->init_hook )
        {
            (emit->init_hook)(&msg);
        }
        esKRNL_SemPost(emit->p_hook_sem);
    }
    else
    {
        __wrn(" msg.id = %d is error \n", p_msg->type);
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

#if 0
/**
 * msg srv thread
 */
__s32 adc_key_init()
{
    RK_Init();
    RK_SetFifoDepth(RK_FIFO_DEPTH_08);
    RK_SetSampleRate(RK_SAMPLE_RATE_16K);
    RK_EnableInt(RK_DATA_IRQ);
    RK_SelectChannel(RK_CHANNEL_X2);
    RK_ClearFifo();
}
__u8 adc_key_init_ok = 0;
__u8 adc_key_down = 0;
__s32 data_filter(__s32 *input, __s32  *output , int arr_len)
{
    __s32 tmp, i, j, half, rec, k = 0;
    __s32 *data;
    data = input;
    i = arr_len;
    for(i = 0; i < arr_len - 1; i++)
        for(j = 0; j < arr_len - i - 1; j++)
        {
            if(data[j] > data[j + 1])
            {
                tmp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = tmp;
            }
        }
    half = arr_len / 2;
    for(i = 0; i < arr_len; i++)
    {
        if(data[half] > data[i])
            rec = data[half] - data[i];
        else
            rec = data[i] - data[half];
        if(rec < 30)
        {
            output[k] = data[i];
            k++;
        }
    }
    return k;
}
static __s32 get_adc_valume(void)
{
#define AD_DATA_NUM 8
    __u32 ret;
    __s32 data[AD_DATA_NUM];
    __s32 data_out[AD_DATA_NUM];
    __s32 average;
    __s32 i;
    __s32 cnt;
    __s32 valid_num;
    cnt = 0;
    while(1)
    {
        ret = RK_QueryIntPending();
        if(ret & RK_DATA_PENDING)
        {
            RK_GetData(data, sizeof(data) / sizeof(data[0]));
            RK_ClearIntPending(ret);
            RK_ClearFifo();
            break;
        }
        if(cnt++ > 2)
        {
            RK_ClearIntPending(ret);
            RK_ClearFifo();
            break;
        }
        esKRNL_TimeDly(1);
    }
    if(!(ret & RK_DATA_PENDING))
    {
        return 0x7ff;//直接返回最大值，默认为没有。
    }
#if 1//C500采样精度比C100多1bit，所以除以2
    {
        for(i = 0 ; i < sizeof(data) / sizeof(data[0]) ; i++)
        {
            data[i] /= 2;
        }
    }
#endif
    {
        valid_num = 0;
        for(i = 0 ; i < sizeof(data) / sizeof(data[0]) ; i++)
        {
            if(data[i] < 0x7ff)
            {
                data[valid_num++]  = data[i];
            }
        }
    }
    if(valid_num <= 0)
    {
        return 0x7ff;
    }
    valid_num = data_filter(data, data_out, valid_num);
    average = 0;
    for(i = 0 ; i < valid_num ; i++)
    {
        average += data_out[i];
    }
    average /= valid_num;
    return average;
}
static __u32 adc_key_scan_X2()
{
    __s32 adc_valume;
    __u32 key = 0;
    adc_valume = get_adc_valume();
    if(adc_valume != 2047)
        __log("--adkey_value=%d\n");
    if(adc_valume < 1820)
    {
        esKRNL_TimeDly(2);
        adc_valume = get_adc_valume();
        if(adc_valume < 1820)
        {

            if(adc_valume >= 100 && adc_valume < 270) //184
            {
                key = KPAD_DOWN;
            }
            else if(adc_valume >= 280 && adc_valume < 450) //367
            {
                key = KPAD_MENU;
            }
            else if(adc_valume >= 460 && adc_valume < 640) //549
            {
                key = KPAD_ENTER;
            }
            else if(adc_valume >= 650 && adc_valume < 850) //738
            {
                key = KPAD_UP;
            }

            if(key)
            {
                adc_key_down = 1;
            }
        }
        //eLIBs_printf(">>>&&&&&&&&&&&&&& adc_valume_x2 = %d &&&&&&&&&&&&&\n",adc_valume);
    }
    return key;
}
#endif
void msg_srv_thread(void *arg)
{
    __msrv_msg_t msg;
    __u32 old_key, new_key, key = 0;
    __s32 delay;
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    __inf("&            message server started !!!   	           &\n");
    __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");

#if 1

    /* 初始化消息通道 */
    memset(&emit_ctr, 0, sizeof(__memit_ctr_t));

    msg_srv_init_tp_channel(&emit_ctr);
    //__wait__;
    msg_srv_init_key_channel(&emit_ctr);
    msg_srv_init_ksrv_channel(&emit_ctr);

    /* 消息分发 */
    while(1)
    {
        if( esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ )
        {
            /* 释放消息通道 */
            msg_srv_deinit_key_channel(&emit_ctr);
            msg_srv_deinit_tp_channel(&emit_ctr);
            msg_srv_deinit_ksrv_channel(&emit_ctr);


            __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
            __inf("&            message server exited !!!                  &\n");
            __inf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");

            esKRNL_TDel(OS_PRIO_SELF);
            return;
        }

        memset( &msg, 0, sizeof(__msrv_msg_t) );

        if( EPDK_OK ==  msg_srv_get_message(&emit_ctr, &msg) )
        {
            msg_srv_dispatch_message(&emit_ctr, &msg);
        }
        else
        {
            esKRNL_TimeDly(1);
        }
    }
#else
    msg_srv_init_key_channel(&emit_ctr);
    if(adc_key_init_ok == 0)
    {

        adc_key_init();
        adc_key_init_ok = 1;
    }


    RK_SelectChannel(RK_CHANNEL_X2);
    RK_ClearFifo();
    while (1)
    {
        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            msg_srv_deinit_key_channel(&emit_ctr);
            esKRNL_TDel(OS_PRIO_SELF);
            break;
        }
        esKRNL_TimeDly(1);
        if(adc_key_down == 1)
        {
            esKRNL_TimeDly(5);
            adc_key_down = 0;

        }
        RK_SelectChannel(RK_CHANNEL_X2);
        RK_ClearFifo();
        old_key = adc_key_scan_X2();
    }

#endif
}


__s32 msg_emit_init(void)
{
    /* 创建消息收集线程 */
#ifdef SIM_PC_WIN
    __SAVE_MOD_EVN__
#endif
    msg_srv_tid = esKRNL_TCreate( msg_srv_thread, NULL, 0x800, KRNL_priolevel3 );
#ifdef SIM_PC_WIN
    __RESTORE_MOD_EVN__
#endif
    if( msg_srv_tid != 0 )
        return EPDK_OK;
    else
        return EPDK_FAIL;

}

__s32 msg_emit_deinit(void)
{
    /* 删除消息采集线程 */
    while( esKRNL_TDelReq(msg_srv_tid) != OS_TASK_NOT_EXIST )
    {
        esKRNL_TimeDly(1);
    }

    msg_srv_tid = 0;

    return EPDK_OK;
}

__s32 msg_emit_register_hook(__memit_hook_t *hook)
{
    __u8 err;

    esKRNL_SemPend(emit_ctr.p_hook_sem, 0, &err);

    emit_ctr.key_hook = hook->key_hook;
    emit_ctr.tp_hook  = hook->tp_hook;
    emit_ctr.init_hook  = hook->init_hook;

    esKRNL_SemPost(emit_ctr.p_hook_sem);

    return EPDK_OK;
}

