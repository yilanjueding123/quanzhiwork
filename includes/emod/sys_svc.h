/*
**********************************************************************************************************************
*											        ePDK
*			    					the Easy Portable/Player Development Kits
*									     system server sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : sys_svc.h
* Version: V1.0
* By     : steven.zgj
**********************************************************************************************************************
*/
#ifndef _SYS_SVC_H_
#define _SYS_SVC_H_

#include "typedef.h"
#include "cfgs.h"

#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif

//--------------------------------------------------------------------
//      resource management sevices
//--------------------------------------------------------------------
// work mode for resource request.
#define RESOURCE_REQ_MODE_WAIT     0
#define RESOURCE_REQ_MODE_NWAIT    1

// resources define
#define RESOURCE_VE_HW  (1<<0)


//--------------------------------------------------------------------
//      register sevices
//--------------------------------------------------------------------
typedef struct __REG_KEY
{
    char *name;
    char *value;
} __reg_key_t;


/*
**********************************************************************************************************************
* 函数定义
**********************************************************************************************************************
*/
/* system call table */
typedef struct
{
    __pSWI_t esSVC_RegCreatePath		;
    __pSWI_t esSVC_RegDeletePath		;

    __pSWI_t esSVC_RegOpenNode         ;
    __pSWI_t esSVC_RegCloseNode        ;
    __pSWI_t esSVC_RegDeleteNode       ;

    __pSWI_t esSVC_RegCreateSet        ;
    __pSWI_t esSVC_RegDeleteSet        ;
    __pSWI_t esSVC_RegGetSetCount      ;
    __pSWI_t esSVC_RegGetFirstSet      ;
    __pSWI_t esSVC_RegGetNextSet       ;

    __pSWI_t esSVC_RegCreateKey        ;
    __pSWI_t esSVC_RegDeleteKey        ;
    __pSWI_t esSVC_RegGetKeyCount      ;
    __pSWI_t esSVC_RegGetFirstKey      ;
    __pSWI_t esSVC_RegGetNextKey       ;
    __pSWI_t esSVC_RegSetKeyValue      ;
    __pSWI_t esSVC_RegGetKeyValue      ;

    __pSWI_t esSVC_RegIni2Reg          ;
    __pSWI_t esSVC_RegReg2Ini          ;
    __pSWI_t esSVC_RegSetRootPath      ;
    __pSWI_t esSVC_RegGetRootPath      ;

    __pSWI_t esSVC_RegGetError         ;
    __pSWI_t esSVC_RegClrError         ;

    // resource maangement sevices
    __pSWI_t esSVC_ResourceReq         ;
    __pSWI_t esSVC_ResourceRel         ;
} SWIHandler_SVC_t;
#ifndef SIM_PC_WIN
#define SYSCALL_SVC(x,y) __swi(SWINO(SWINO_SVC, SWIHandler_SVC_t, y))x y
#else
#define SYSCALL_SVC(x,y) x y
#endif

/* 目录操作 */
SYSCALL_SVC(__s32 , esSVC_RegCreatePath )(const char *path );
SYSCALL_SVC(__s32 , esSVC_RegDeletePath )(const char *path );

SYSCALL_SVC(__hdle, esSVC_RegOpenNode   )(const char *node, __s32 mode );
SYSCALL_SVC(__s32 , esSVC_RegCloseNode  )(__hdle handle);
SYSCALL_SVC(__s32 , esSVC_RegDeleteNode )(const  char *node);

SYSCALL_SVC(__s32 , esSVC_RegCreateSet  )(__hdle handle, const char *set_name );
SYSCALL_SVC(__s32 , esSVC_RegDeleteSet  )(__hdle handle, const char *set_name );
SYSCALL_SVC(__s32 , esSVC_RegGetSetCount)(__hdle handle, __u32 *count_p );
SYSCALL_SVC(__s32 , esSVC_RegGetFirstSet)(__hdle handle );
SYSCALL_SVC(__s32 , esSVC_RegGetNextSet )(__hdle handle, char *set_name );

SYSCALL_SVC(__s32 , esSVC_RegCreateKey  )(__hdle handle, const char *set_name, const char *key_name, const char *key_value);
SYSCALL_SVC(__s32 , esSVC_RegDeleteKey  )(__hdle handle, const char *set_name, const char *key_name);
SYSCALL_SVC(__s32 , esSVC_RegGetKeyCount)(__hdle handle, const char *set_name, __u32 *count_p );
SYSCALL_SVC(__s32 , esSVC_RegGetFirstKey)(__hdle handle, const char *set_name );
SYSCALL_SVC(__s32 , esSVC_RegGetNextKey )(__hdle handle, const char *set_name, char *key_name );
SYSCALL_SVC(__s32 , esSVC_RegSetKeyValue)(__hdle handle, const char *set_name, const char *key_name, const char *key_value );
SYSCALL_SVC(__s32 , esSVC_RegGetKeyValue)(__hdle handle, const char *set_name, const char *key_name, char *key_value );

SYSCALL_SVC(__s32 , esSVC_RegIni2Reg    )(const char *ini_file );
SYSCALL_SVC(__s32 , esSVC_RegReg2Ini    )(const char *ini_file );
SYSCALL_SVC(void  , esSVC_RegSetRootPath)(const char *path );
SYSCALL_SVC(void  , esSVC_RegGetRootPath)(char *path );

SYSCALL_SVC(__s32 , esSVC_RegGetError   )(__hdle handle );
SYSCALL_SVC(__s32 , esSVC_RegClrError   )(__hdle handle );

// resource management sevices
SYSCALL_SVC(__hdle, esSVC_ResourceReq   )(__u32 res, __u32 mode, __u32 timeout);
SYSCALL_SVC(__s32 , esSVC_ResourceRel   )(__hdle hRes);

//**********************************************************************************************************

//**********************************************************************************************************
//* define level 1( system level)
//************************************************
// 常数定义

//************************************************
// 数据类型定义

//************************************************
// 变量定义
extern SWIHandler_SVC_t SWIHandler_SVC;
//************************************************
// 函数定义
__s32 SVC_Init (void);
__s32 SVC_Exit (void);

#endif //#ifndef _SYS_DEV_H_



