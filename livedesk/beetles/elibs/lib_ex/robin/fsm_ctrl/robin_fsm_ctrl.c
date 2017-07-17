/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : robin_fsm_ctrl.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.25
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
* Gary.Wang      2009.09.25       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_fsm_ctrl_c
#define  __robin_fsm_ctrl_c


#include "..\\robin_i.h"



/***************************************************************************************************
*Name        : robin_set_cmd_play_file
*Prototype   : __s32  robin_set_cmd_play_file( const char *file, const __cedar_tag_inf_t *tag_info_p )
*Arguments   : file           input. the file to be played.
*              tag_info_p     input. the tag info of the file. If there is no tag info, input should
*                             be NULL.
*Return      : ==0       succeed
*              !=0       fail
*Description : set play a file cmd. This operation will send the "play file" command to FSM but can't
*              enable FSM to be in "play" status immediately, in other words, this is an asynchronous
*              operation.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_play_file( const char *file, const __cedar_tag_inf_t *tag_info_p )
{
    robin_msg_t  *msg_p;

    msg_p = get_robin_msg_buf( );

    msg_p->id = ROBIN_CMD_PLAY_FILE;
    g_strcpy( msg_p->file_info.file_path, file );
    if( tag_info_p != NULL )
    {
        g_memcpy( &(msg_p->file_info.tag_inf), tag_info_p, sizeof(__cedar_tag_inf_t) );
        msg_p->file_info.tag_inf_validflag = 1;
    }
    else
    {
        msg_p->file_info.tag_inf_validflag = 0;
    }

    g_QFlush( robin_cmdQ );
    g_QPost( robin_cmdQ, msg_p );

    return 0;
}

/***************************************************************************************************
*Name        : robin_play_file
*Prototype   : __s32  robin_play_file( const char *file, const __cedar_tag_inf_t *tag_info_p )
*Arguments   : file           input. the file to be played.
*              tag_info_p     input. the tag info of the file. If there is no tag info, input should
*                             be NULL.
*Return      : ==0       succeed
*              !=0       fail
*Description :
*              this is an synchronous
*              operation.
*Other       :
***************************************************************************************************/
__s32 robin_play_file( const char *file, const __cedar_tag_inf_t *tag_info_p )
{
    /* stop old */
    syn_op_stop( );

    /* play new */
    {
        syn_op_ret_e ret;

        /* play */
        ret = syn_op_play_file( file , tag_info_p);
        switch( ret )
        {
        case SYN_OP_RET_OK                 :
            goto ok;
        case SYN_OP_RET_TO_QUIT_ROBIN      :
            goto ok;
        case SYN_OP_RET_SEND_CMD_ERR       :
            goto error;
        case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
            goto error;
        case SYN_OP_RET_OVERTIME           :
        default :
            goto error;
        }

ok:
        return 0;

error:
        return -1;
    }
}

/***************************************************************************************************
*Name        : robin_set_cmd_play
*Prototype   : __s32  robin_set_cmd_play( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set play cmd. This operation will send the "play" command to FSM and wait the FSM
*              to be in "play" status.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_play( void )
{
    __s32  ret;
    __u8   err;



    __here__;
    if(EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }
    esKRNL_TimeDly(1);
    g_pend_mutex( robin_cedar_mutex, &err );
    __here__;
    robin_wait_no_file( );
    __here__;
    ret = syn_op_play( );
    esKRNL_TimeDly(1);
    switch( ret )
    {
    case SYN_OP_RET_OK                 :
        goto ok;
    case SYN_OP_RET_TO_QUIT_ROBIN      :
        goto ok;
    case SYN_OP_RET_SEND_CMD_ERR       :
        goto error;
    case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
        goto error;
    case SYN_OP_RET_OVERTIME           :
    default :
        goto error;
    }

ok:
    g_post_mutex( robin_cedar_mutex );
    return 0;

error:
    g_post_mutex( robin_cedar_mutex );
    return -1;
}



/***************************************************************************************************
*Name        : robin_set_cmd_stop
*Prototype   : __s32  robin_set_cmd_stop( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set stop cmd. This operation will send the "stop" command to FSM and wait the FSM
*              to be in "stop" status.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_stop( void )
{
    __s32  ret;
    __u8   err;

    g_pend_mutex( robin_cedar_mutex, &err );

    robin_wait_no_file( );

    ret = syn_op_stop( );
    switch( ret )
    {
    case SYN_OP_RET_OK                 :
        goto ok;
    case SYN_OP_RET_TO_QUIT_ROBIN      :
        goto ok;
    case SYN_OP_RET_SEND_CMD_ERR       :
        goto error;
    case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
        goto error;
    case SYN_OP_RET_OVERTIME           :
    default :
        goto error;
    }

ok:
    g_post_mutex( robin_cedar_mutex );
    return 0;

error:
    g_post_mutex( robin_cedar_mutex );
    return -1;
}



/***************************************************************************************************
*Name        : robin_set_cmd_pause
*Prototype   : __s32  robin_set_cmd_pause( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set pause cmd. This operation will send the "pause" command to FSM and wait the FSM
*              to be in "pause" status.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_pause( void )
{
    __s32  ret;
    __u8   err;

    g_pend_mutex( robin_cedar_mutex, &err );

    robin_wait_no_file( );

    ret = syn_op_pause( );
    switch( ret )
    {
    case SYN_OP_RET_OK                 :
        goto ok;
    case SYN_OP_RET_TO_QUIT_ROBIN      :
        goto ok;
    case SYN_OP_RET_SEND_CMD_ERR       :
        goto error;
    case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
        goto error;
    case SYN_OP_RET_OVERTIME           :
    default :
        goto error;
    }

ok:
    g_post_mutex( robin_cedar_mutex );
    return 0;

error:
    g_post_mutex( robin_cedar_mutex );
    return -1;
}



/***************************************************************************************************
*Name        : robin_set_cmd_ff
*Prototype   : __s32  robin_set_cmd_ff( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set ff cmd. This operation will send the "ff" command to FSM and wait the FSM
*              to be in "ff" status.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_ff( void )
{
    __s32  ret;
    __u8   err;

    g_pend_mutex( robin_cedar_mutex, &err );

    robin_wait_no_file( );

    ret = syn_op_ff( );
    switch( ret )
    {
    case SYN_OP_RET_OK                 :
        goto ok;
    case SYN_OP_RET_TO_QUIT_ROBIN      :
        goto ok;
    case SYN_OP_RET_SEND_CMD_ERR       :
        goto error;
    case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
        goto error;
    case SYN_OP_RET_OVERTIME           :
    default :
        goto error;
    }

ok:
    g_post_mutex( robin_cedar_mutex );
    return 0;

error:
    g_post_mutex( robin_cedar_mutex );
    return -1;
}



/***************************************************************************************************
*Name        : robin_set_cmd_rr
*Prototype   : __s32  robin_set_cmd_rr( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set rr cmd. This operation will send the "rr" command to FSM and wait the FSM
*              to be in "rr" status.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_rr( void )
{
    __s32  ret;
    __u8   err;

    g_pend_mutex( robin_cedar_mutex, &err );

    robin_wait_no_file( );

    ret = syn_op_rr( );
    switch( ret )
    {
    case SYN_OP_RET_OK                 :
        goto ok;
    case SYN_OP_RET_TO_QUIT_ROBIN      :
        goto ok;
    case SYN_OP_RET_SEND_CMD_ERR       :
        goto error;
    case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
        goto error;
    case SYN_OP_RET_OVERTIME           :
    default :
        goto error;
    }

ok:
    g_post_mutex( robin_cedar_mutex );
    return 0;

error:
    g_post_mutex( robin_cedar_mutex );
    return -1;
}



/***************************************************************************************************
*Name        : robin_set_cmd_jump
*Prototype   : __s32  robin_set_cmd_jump( __u32 time )
*Arguments   : time      time. unit : ms.
*Return      : ==0       succeed
*              !=0       fail
*Description : set jump cmd. This operation will send the "jump" command to FSM.
*Other       :
***************************************************************************************************/
__s32  robin_set_cmd_jump( __u32 time )
{
    __s32  ret;
    __u8   err;

    g_pend_mutex( robin_cedar_mutex, &err );

    robin_wait_no_file( );

    ret = syn_op_jump( time );
    switch( ret )
    {
    case SYN_OP_RET_OK                 :
        goto ok;
    case SYN_OP_RET_TO_QUIT_ROBIN      :
        goto ok;
    case SYN_OP_RET_SEND_CMD_ERR       :
        goto error;
    case SYN_OP_RET_CEDAR_FEEDBACK_ERR :
        goto error;
    case SYN_OP_RET_OVERTIME           :
    default :
        goto error;
    }

ok:
    g_post_mutex( robin_cedar_mutex );
    return 0;

error:
    g_post_mutex( robin_cedar_mutex );
    return -1;
}



/***************************************************************************************************
*Name        : robin_get_fsm_status
*Prototype   : __cedar_status_t  robin_get_fsm_status( void )
*Arguments   : void
*Return      : the current status of the fsm, such as "play", "pause" and so on. The status are
*              defined in mod_cedar.h, see __cedar_status_t.
*Description : get the current status of the fsm.
*Other       :
***************************************************************************************************/
__cedar_status_t  robin_get_fsm_status( void )
{
    __u8   err;

    __here__;
    g_pend_mutex( robin_cedar_mutex, &err );

    __here__;
    robin_wait_no_file( );
    __here__;
    g_post_mutex( robin_cedar_mutex );

    __msg("*******before CEDAR_CMD_GET_STATUS*********\n");
    return (__cedar_status_t)esMODS_MIoctrl( robin_hced, CEDAR_CMD_GET_STATUS, 0, NULL );
}



/***************************************************************************************************
*Name        : robin_clear_feedback_msgQ
*Prototype   : void   robin_clear_feedback_msgQ( void )
*Arguments   : void
*Return      : void
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can clear the queue.
*Other       :
***************************************************************************************************/
void   robin_clear_feedback_msgQ( void )
{
    if( robin_feedbackQ != NULL )
        g_QFlush( robin_feedbackQ );
}



/***************************************************************************************************
*Name        : robin_get_feedback_msg
*Prototype   : feedback_msg_t  robin_get_feedback_msg( void )
*Arguments   : void
*Return      : error message. 0 means no message at present, while other values are the error
*              messages. The error messages are defined in mod_cedar.h, see __cedar_feedback_msg_t.
*Description : There is a message queue created by FSM, that offers instant error message. This
*              operation can get the instant error message .
*Other       :
***************************************************************************************************/
feedback_msg_t  robin_get_feedback_msg( void )
{
    feedback_msg_t  ret;
    __u8            err;

    __here__;
    ret = (feedback_msg_t)g_QAccept( robin_feedbackQ, &err );
    __here__;
    if( ret == NULL )
        return (feedback_msg_t)0;
    else
        return ret;
}



/***************************************************************************************************
*Name        : robin_get_fsm_cur_file
*Prototype   : __s32  robin_get_fsm_cur_file( char *file, __u32 size )
*Arguments   : file      output. a buffer to store the fsm's current file.
*              size      input. The size of the buffer.
*Return      : ==0       succeed.
*              !=0       no file.
*Description : get the fsm's current file.
*Other       :
***************************************************************************************************/
__s32  robin_get_fsm_cur_file( char *file, __u32 size )
{
    return robin_monitor_get_cur_file( file, size );
}



/***************************************************************************************************
*Name        : robin_fsm_is_busy
*Prototype   : __bool robin_fsm_is_busy( void );
*Arguments   : void
*Return      : EPDK_TRUE   the fsm is busy.
*              EPDK_FALSE  the fsm is not busy.
*Description : check whether the fsm is busy or not.
*Other       :
***************************************************************************************************/
__bool robin_fsm_is_busy( void )
{
    if( get_msg_nr_of_q( robin_cmdQ ) == 0 )
        return EPDK_TRUE;
    else
        return EPDK_FALSE;
}



/***************************************************************************************************
*Name        : robin_enable_raw_data
*Prototype   : __s32 robin_enable_raw_data( __bool flag )
*Arguments   : flag      input. enable or disable flag.
*Return      : ==0       succeed
*              !=0       fail
*Description : enable or disable raw data output.
*Other       :
***************************************************************************************************/
__s32 robin_enable_raw_data( __bool flag )
{
    __u8   err;
    __s32  ret;

    g_pend_mutex( robin_cedar_mutex, &err );

    ret = esMODS_MIoctrl( robin_hced, CEDAR_CMD_AUDIO_RAW_DATA_ENABLE, flag, NULL );

    g_post_mutex( robin_cedar_mutex );

    return ret;
}



#endif     //  ifndef __robin_fsm_ctrl_c

/* end of robin_fsm_ctrl.c */
