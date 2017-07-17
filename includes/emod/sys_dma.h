/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               DMA driver Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sys_dma.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-24 15:44
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-24 15:44    kevin.z     2.0     buidl the file.
*********************************************************************************************************
*/
#ifndef __SYS_DMA_H__
#define __SYS_DMA_H__

#include "typedef.h"
#include "cfgs.h"


#ifdef EPDK_USED_BY_KRNL
#define __swi(x)
#endif


typedef struct
{
    __pSWI_t esDMA_Request          ;
    __pSWI_t esDMA_Release          ;
    __pSWI_t esDMA_Setting          ;
    __pSWI_t esDMA_Start            ;
    __pSWI_t esDMA_Restart          ;
    __pSWI_t esDMA_Stop             ;
    __pSWI_t esDMA_QueryStat        ;
    __pSWI_t esDMA_QuerySrc         ;
    __pSWI_t esDMA_QueryDst         ;
    __pSWI_t esDMA_QueryRestCount   ;
    __pSWI_t esDMA_ChangeMode       ;
    __pSWI_t esDMA_EnableINT        ;
    __pSWI_t esDMA_DisableINT       ;
    __pSWI_t esDMA_RegDmaHdler      ;
    __pSWI_t esDMA_UnregDmaHdler    ;
    __pSWI_t esDMA_Information      ;

} SWIHandler_DMA_t;


#ifndef SIM_PC_WIN
#define SYSCALL_DMA(x,y) __swi(SWINO(SWINO_DMA, SWIHandler_DMA_t, y))x y
#else
#define SYSCALL_DMA(x,y) x y
#endif


SYSCALL_DMA(__hdle, esDMA_Request       )(char *user, __u32 type);
SYSCALL_DMA(__s32,  esDMA_Release       )(__hdle dma);
SYSCALL_DMA(__s32,  esDMA_Setting       )(__hdle dma, void *pArg);
SYSCALL_DMA(__s32,  esDMA_Start         )(__hdle dma, __u32 saddr, __u32 daddr, __u32 len);
SYSCALL_DMA(__s32,  esDMA_Stop          )(__hdle dma);
SYSCALL_DMA(__s32,  esDMA_Restart       )(__hdle dma);
SYSCALL_DMA(__u32,  esDMA_QueryStat     )(__hdle dma);
SYSCALL_DMA(__u32,  esDMA_QuerySrc      )(__hdle dma);
SYSCALL_DMA(__u32,  esDMA_QueryDst      )(__hdle dma);
SYSCALL_DMA(__s32,  esDMA_EnableINT     )(__hdle dma, __s32 type);
SYSCALL_DMA(__s32,  esDMA_DisableINT    )(__hdle dma, __s32 type);
SYSCALL_DMA(__u32,  esDMA_QueryRestCount)(__hdle dma);
SYSCALL_DMA(__s32,  esDMA_ChangeMode    )(__hdle dma, __s32 mode);
SYSCALL_DMA(__s32,  esDMA_RegDmaHdler   )(__hdle dma, __s32 type, __pCBK_t hdler, void *arg);
SYSCALL_DMA(__s32,  esDMA_UnregDmaHdler )(__hdle dma, __s32 type, __pCBK_t hdler);
SYSCALL_DMA(void,   esDMA_Information   )(void);



//==============================================================
// define interface for kernel
//==============================================================
extern SWIHandler_DMA_t SWIHandler_DMA;
__s32 DMA_Init(void);
__s32 DMA_Exit(void);


#endif  /* __SYS_DMA_H__ */

