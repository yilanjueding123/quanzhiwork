/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Flash Module
*
*                                    (c) Copyright 2010-2012, wfsun China
*                                             All Rights Reserved
*
* File    : mod_flash_def.h
* By      : wfsun
* Version : v1.0
* Date    : 2011-1-18
* Descript: flash module external interfaces.
* Update  : date                auther      ver     notes
*           2011-1-18 17:05:31  wfsun       1.0     create this file.
*********************************************************************************************************
*/

#ifndef __MOD_FLASH_DEF_H__
#define __MOD_FLASH_DEF_H__

typedef struct
{
    RECT	show_rect;		//显示的区域
    __u32	format;			//格式，如PIXEL_COLOR_ARGB8888
    __u32	pipe;			//管道，0或1
} flash_disp_para_t;

typedef enum
{
    FLASH_KEY_LEFT = 0x01,
    FLASH_KEY_RIGHT,
    FLASH_KEY_HOME,
    FLASH_KEY_END,
    FLASH_KEY_INSERT,
    FLASH_KEY_DELETE,
    FLASH_KEY_BACKSPACE,
    FLASH_KEY_ENTER,
    FLASH_KEY_UP,
    FLASH_KEY_DOWN,
    FLASH_KEY_TAB,
    FLASH_KEY_ESC,
    FLASH_KEY_SHITF,

    FLASH_KEY_0 = 0x30,
    FLASH_KEY_1,
    FLASH_KEY_2,
    FLASH_KEY_3,
    FLASH_KEY_4,
    FLASH_KEY_5,
    FLASH_KEY_6,
    FLASH_KEY_7,
    FLASH_KEY_8,
    FLASH_KEY_9,

    FLASH_KEY_A = 0x41,
    FLASH_KEY_B,
    FLASH_KEY_C,
    FLASH_KEY_D,
    FLASH_KEY_E,
    FLASH_KEY_F,
    FLASH_KEY_G,
    FLASH_KEY_H,
    FLASH_KEY_I,
    FLASH_KEY_J,
    FLASH_KEY_K,
    FLASH_KEY_L,
    FLASH_KEY_M,
    FLASH_KEY_N,
    FLASH_KEY_O,
    FLASH_KEY_P,
    FLASH_KEY_Q,
    FLASH_KEY_R,
    FLASH_KEY_S,
    FLASH_KEY_T,
    FLASH_KEY_U,
    FLASH_KEY_V,
    FLASH_KEY_W,
    FLASH_KEY_X,
    FLASH_KEY_Y,
    FLASH_KEY_Z,

    FLASH_KEY_a = 0x61,
    FLASH_KEY_b,
    FLASH_KEY_c,
    FLASH_KEY_d,
    FLASH_KEY_e,
    FLASH_KEY_f,
    FLASH_KEY_g,
    FLASH_KEY_h,
    FLASH_KEY_i,
    FLASH_KEY_j,
    FLASH_KEY_k,
    FLASH_KEY_l,
    FLASH_KEY_m,
    FLASH_KEY_n,
    FLASH_KEY_o,
    FLASH_KEY_p,
    FLASH_KEY_q,
    FLASH_KEY_r,
    FLASH_KEY_s,
    FLASH_KEY_t,
    FLASH_KEY_u,
    FLASH_KEY_v,
    FLASH_KEY_w,
    FLASH_KEY_x,
    FLASH_KEY_y,
    FLASH_KEY_z,
    FLASH_KEY_NULL = 0x200
} flash_key_msg_t;

typedef enum
{
    FLASH_DOWN = 0x01,
    FLASH_UP,
} flash_key_status_t;

typedef enum
{
    FLASH_MOUSE_DOWN = 0x300,
    FLASH_MOUSE_MOVE,
    FLASH_MOUSE_UP
} flash_mouse_msg_t;

/*!
  * \brief
  *		flash与应用交互的命令
*/
typedef enum
{
    FLASH_DO_NOTHING,
    FLASH_SET_FILE,				//设置待播放的swf文件，param1实参为char *file，param2=0;
    FLASH_SET_DISPLAY_PARA,		//设置显示参数，param1实参为flash_disp_para_t,用于限定flash的显示区域等;param2=0;
    FLASH_PLAY,					//播放当前的flash文件，param1=0;param2=0;
    FLASH_STOP,					//停止当前flash的播放,param1=0;param2=0;
    FLASH_KEY,					//按键消息，param1实参为flash_key_msg_t中的一个;param2实参为flash_key_status_t中的一个;
    FLASH_MOUSE,				//鼠标消息，param1实参为flash_mouse_msg_t中的一个;param2实参为__pos_t,即鼠标坐标信息;
    FLASH_INPUT_STRING,			//字符串输入，param1实参为char *string，param2=0;
    FLASH_SET_INPUT_CALLBACK,	//设回调，用于flash解码过程中需要输入时返回的通知消息
    FLASH_SET_ATTR,				//设定私有数据，param1为指针
    FLASH_GET_ATTR,				//获取私有数据
} flash_cmd_t;

typedef enum
{
    FLASH_IME_OPEN = 0x4001,
    FLASH_MSG_QUIT = 0x4002,
    FLASH_MSG_MEMORY_NOT_ENOUGH = 0x4003
} flash_msg;

typedef struct
{
    void		*flashhandle;				/*flash句柄*/
    flash_cmd_t	cmd;
    __s32		cmd_param1;
    __s32		cmd_param2;
    __s32		cmd_param3;
} __flash_msg_t;

typedef	struct	tag_flash_data
{
    H_WIN		h_win;						/*主管理窗口句柄*/
    flash_disp_para_t h_win_para;

    H_WIN		sime_win;					/*sime 		场景管理窗口*/
    H_WIN		sback_win;					/*sback		背景*/

    __u32		last_key;					/*记录上一次按键值*/
    __bool		select;
    __pos_t		selectpos;

    char		app_title[32];
    void		*flashhandle;				/*flash句柄*/
} flash_data_t;

typedef struct
{
    void		*flashhandle;				/*flash句柄*/
    flash_msg 	msg;
} __flash_callback_msg_t;

/*
**********************************************************************************************************************
*                                               flash_open
*
* author:           wfsun
*
* date:             2010-12-07
*
* Description:      初始化flash解码，flash解码库内部对需要的资源进行初始化。
*
* parameters:
*
* return:           if success return handle
*                   if fail return NULL
* modify history:
**********************************************************************************************************************
*/
//__hdle flash_open(void);

/*
**********************************************************************************************************************
*                                               flash_control
*
* author:           wfsun
*
* date:             2010-12-07
*
* Description:      flash与应用交互的命令，各个命令的意义及参数参见flash_cmd_t
*
* parameters:
*
* return:           if success return 0
*                   if fail return -1
* modify history:
**********************************************************************************************************************
*/
//__s32 flash_control(__hdle handle, flash_cmd_t cmd, __s32 param1, __s32 param2);

/*
**********************************************************************************************************************
*                                               flash_close
*
* author:           wfsun
*
* date:             2010-12-07
*
* Description:      关闭flash解码，释放flash解码资源
*
* parameters:
*
* return:           if success return 0
*                   if fail return -1
* modify history:
**********************************************************************************************************************
*/
//__s32 (*flash_close)(__hdle handle);


typedef struct FLASH_API_ENTRY
{
    __hdle (*ptr_flash_open)(void);
    __s32  (*ptr_flash_control)(__hdle handle, flash_cmd_t cmd, __s32 param1, __s32 param2);
    __s32  (*ptr_flash_close)(__hdle handle);
} flash_api_entry_t;

//not used by flash module itsef

//this address is depended on the link address of mod_flash
#define FLASH_MOD_ENTRY_TBL   (0xe8600000)

#define FLASH_MOD_FUNC_ENTRY  ((flash_api_entry_t *)FLASH_MOD_ENTRY_TBL)

#endif  /* __MOD_FLASH_H__ */
