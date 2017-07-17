/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Config system
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : sys_config.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-28
* Descript: system config sevice functions.
* Update  : date                auther      ver     notes
*           2011-4-28 20:57:56  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef  __SYS_CONFIG_H__
#define  __SYS_CONFIG_H__

//config system call table
typedef struct
{
    __pSWI_t esCFG_GetKeyValue        ;
    __pSWI_t esCFG_GetSecKeyCount     ;
    __pSWI_t esCFG_GetSecCount        ;
    __pSWI_t esCFG_GetGPIOSecKeyCount ;
    __pSWI_t esCFG_GetGPIOSecData     ;

    __pSWI_t esCFG_Init_Ex               ;
    __pSWI_t esCFG_Exit_Ex               ;
    __pSWI_t esCFG_GetKeyValue_Ex        ;
    __pSWI_t esCFG_GetSecKeyCount_Ex     ;
    __pSWI_t esCFG_GetSecCount_Ex        ;
    __pSWI_t esCFG_GetGPIOSecKeyCount_Ex ;
    __pSWI_t esCFG_GetGPIOSecData_Ex     ;

} SWIHandler_CONFIG_t;

extern SWIHandler_CONFIG_t SWIHandler_CONFIG;

#ifndef SIM_PC_WIN
#define SYSCALL_CFG(x,y) __swi(SWINO(SWINO_CFG, SWIHandler_CONFIG_t, y))x y
#else
#define SYSCALL_CFG(x,y) x y
#endif

SYSCALL_CFG(__s32, esCFG_GetKeyValue       )(char *SecName, char *KeyName, int Value[], int Count);
SYSCALL_CFG(__s32, esCFG_GetSecKeyCount    )(char *SecName);
SYSCALL_CFG(__s32, esCFG_GetSecCount       )(void);
SYSCALL_CFG(__s32, esCFG_GetGPIOSecKeyCount)(char *GPIOSecName);
SYSCALL_CFG(__s32, esCFG_GetGPIOSecData    )(char *GPIOSecName, void *pGPIOCfg, int GPIONum);

SYSCALL_CFG(H_PARSER, esCFG_Init_Ex)(char *path);
SYSCALL_CFG(__s32, esCFG_Exit_Ex)(H_PARSER parser);
SYSCALL_CFG(__s32, esCFG_GetKeyValue_Ex       )(H_PARSER parser, char *KeyName, int Value[], int Count);
SYSCALL_CFG(__s32, esCFG_GetSecKeyCount_Ex    )(H_PARSER parser, char *SecName);
SYSCALL_CFG(__s32, esCFG_GetSecCount_Ex       )(H_PARSER parser);
SYSCALL_CFG(__s32, esCFG_GetGPIOSecKeyCount_Ex)(H_PARSER parser, char *GPIOSecName);
SYSCALL_CFG(__s32, esCFG_GetGPIOSecData_Ex    )(H_PARSER parser, char *GPIOSecName, void *pGPIOCfg, int GPIONum);


//************************************************
// º¯Êý¶¨Òå
//************************************************
extern __s32 esCFG_Init(__u8 *CfgVAddr, __u32 size);
extern __s32 esCFG_Exit(void);

#endif  // __SYS_CONFIG_H__
