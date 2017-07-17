/*
*********************************************************************************************************
*											        ePOS
*						           the Easy Portable/Player Operation System
*									           MODS sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : MODS.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _MODS_H_
#define _MODS_H_

#include "typedef.h"
#include "cfgs.h"


#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif
//**********************************************************************************************************
//* define level 0( system level)
//************************************************
// 常数定义
#define MODS_maxseg 16
#define MODS_midmax 64



//************************************************
// 数据类型定义
typedef struct __MP         //mp的具体结构由模块自己自行定义，大小不定，但强制第一个byte存放mid
{
    //mid将用于调用具体的module函数时fsfc用
    __u8            mid;
    __u8            userdef;
} __mp;

typedef struct __MIF
{
    __s32          (*MInit)(void);              //在install/reset模块时使用,若模块需要后台任务,可以在此启动
    __s32          (*MExit)(void);              //在uninstall模块时使用,若模块有后台任务,可以在此处关闭
    __mp          *(*MOpen)(__u32 id, __u32 mode);   //打开一个模块句柄
    __s32          (*MClose)(__mp *mp);         //关闭一个模块句柄
    __u32          (*MRead)(void *pdata, __u32 size, __u32 n, __mp *mp);                  //模块读
    __u32          (*MWrite)(const void *pdata, __u32 size, __u32 n, __mp *mp);           //模块写
    __s32          (*MIoctrl)(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);             //模块控制
} __mif_t;
typedef struct __MODS_MGSEC
{
    char            magic[8];           // "ePDK.mod"
    __u32           version;
    __u8            type;               // 0xff: user module(midware)

    __u32           heapaddr;           //heap在32M空间内的位置，通常在最后1M(31*1024*1024)
    //若地址等于0，那么表示进程不需要局部堆，将共用系统堆
    __u32           heapsize;	        //heap的初始大小
    __mif_t         mif;                //pcb里第一个任务的首地址
} __mods_mgsec_t;


typedef struct __MODS_MCB        		//进程控制块
{
    __krnl_xcb_t    xcb;
    __u8            type;               //系统模块或用户模块
    __mif_t         mif;                //模块入口，模块的标准函数入口地址存放在此处
    __u32			vmbitmap;			//模块虚拟内存空间占用情况，用于卸载模块时的虚拟内存时间释放
} __mods_mcb_t;

//************************************************
// 变量定义
/* GLOBAL VARIABLES */

//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esMODS_MInstall              ;
    __pSWI_t esMODS_MUninstall            ;
    __pSWI_t esMODS_MOpen                 ;
    __pSWI_t esMODS_MClose                ;
    __pSWI_t esMODS_MRead                 ;
    __pSWI_t esMODS_MWrite                ;
    __pSWI_t esMODS_MIoctrl               ;

} SWIHandler_MODS_t;
#ifndef SIM_PC_WIN
#define SYSCALL_MODS(x,y) __swi(SWINO(SWINO_MODS, SWIHandler_MODS_t, y))x y
#else
#define SYSCALL_MODS(x,y) x y
#endif
SYSCALL_MODS(__u32, esMODS_MInstall  )(const char *mfile, __u8 mode);
SYSCALL_MODS(__s32, esMODS_MUninstall)(__u8 mid);
SYSCALL_MODS(__mp *, esMODS_MOpen     )(__u8 mid, __u8 mode);
SYSCALL_MODS(__s32, esMODS_MClose    )(__mp *mp);
SYSCALL_MODS(__u32, esMODS_MRead     )(void *pdata, __u32 size, __u32 n, __mp *mp);
SYSCALL_MODS(__u32, esMODS_MWrite    )(const void *pdata, __u32 size, __u32 n, __mp *mp);
SYSCALL_MODS(__s32, esMODS_MIoctrl   )(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);

//**********************************************************************************************************

//**********************************************************************************************************
//* define level 1( system level)
//************************************************
// 常数定义

//************************************************
// 数据类型定义

//************************************************
// 变量定义

extern SWIHandler_MODS_t SWIHandler_MODS;
//************************************************
// 函数定义
__s32 MODS_Init	(void);
__s32 MODS_Exit  (void);


//**********************************************************************************************************

#endif  /* _MODS_H_ */


