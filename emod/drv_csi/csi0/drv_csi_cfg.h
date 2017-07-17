/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_duckweed sub-system
*                          (module name, e.g.mpeg4 decoder plug-in) module
*
*          (c) Copyright 2010-2012, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : drv_csi_cfg.h
* Version: V1.0
* By     : Eric_wang
* Date   : 2010-1-3
* Description:
********************************************************************************
*/
#ifndef _DRV_CSI_CFG_H_
#define _DRV_CSI_CFG_H_

#include "epdk.h"

#define NEW_SDK 1//1=new sdk, other old sdk=>GPIO_define differ


////==============================================================================
////                              READ/WRITE DATA DEFINE
////==============================================================================

#ifdef W
#undef W
#endif

#ifdef R
#undef R
#endif

#ifdef S
#undef S
#endif

#ifdef C
#undef C
#endif

#define W(offset,value)          (*((volatile __u32 *)(offset))=(value))    //
#define R(offset)                (*((volatile __u32 *)(offset)))            //
#define S(offset,bit)            (*((volatile __u32 *)(offset)) |=(bit))    //
#define C(offset,bit)            (*((volatile __u32 *)(offset)) &=(~(bit)))   //


////==============================================================================
////                              MEMORY MANAGEMENT REDEFINE
////==============================================================================
//#ifdef MALLOC
//#undef MALLOC
//#endif
#ifdef csi_malloc
#undef csi_malloc
#endif
//#ifdef FREE
//#undef FREE
//#endif
#ifdef csi_free
#undef csi_free
#endif
//#ifdef PHY_MALLOC
//#undef PHY_MALLOC
//#endif
//#ifdef PHY_FREE
//#undef PHY_FREE
//#endif
//#ifdef MEM_SET
//#undef MEM_SET
//#endif
#ifdef csi_memset
#undef csi_memset
#endif
//#ifdef MEM_CPY
//#undef MEM_CPY
//#endif
#ifdef csi_memcpy
#undef csi_memcpy
#endif

#define csi_malloc(size)                    eLIBs_malloc(size)
#define csi_free(pbuf)                      eLIBs_free(pbuf)
//#define PHY_MALLOC(size, align)         cedarlib_phymalloc(size, align)
//#define PHY_FREE(pbuf)                  cedarlib_phyfree(pbuf)
#define csi_memset(buf, val, size)          eLIBs_memset(buf, val, size)
#define csi_memcpy(dst, src, size)          eLIBs_memcpy(dst, src, size)
//#define MALLOC                          csi_malloc
//#define FREE                            csi_free
//#define MEM_SET                         csi_memset
//#define MEM_CPY                         csi_memcpy

////==============================================================================
////                              FILE SYSTEM ADAPT MODULE
////==============================================================================
//#ifdef FOPEN
//#undef FOPEN
//#endif
#ifdef csi_fopen
#undef csi_fopen
#endif
//#ifdef FCLOSE
//#undef FCLOSE
//#endif
#ifdef csi_fclose
#undef csi_fclose
#endif
//#ifdef FREAD
//#undef FREAD
//#endif
#ifdef csi_fread
#undef csi_fread
#endif
//#ifdef FSEEK
//#undef FSEEK
//#endif
#ifdef csi_fseek
#undef csi_fseek
#endif
//#ifdef FTELL
//#undef FTELL
//#endif
#ifdef csi_ftell
#undef csi_ftell
#endif

#ifdef csi_fioctrl
#undef csi_fioctrl
#endif



#define csi_fopen(name, mode)               eLIBs_fopen((name),(mode))
#define csi_fclose(fptr)                    eLIBs_fclose((fptr))
#define csi_fread(buf,unit,size,fptr)       eLIBs_fread((buf),(unit),(size),(fptr))
#define csi_fseek(fptr,offset,origin)       eLIBs_fseek((fptr),(offset),(origin))
#define csi_ftell(fptr)                     eLIBs_ftell((fptr))
#define csi_fioctrl(fptr,cmd,aux,pbuf)      eLIBs_fioctrl((fptr), (cmd), (aux), (pbuf))


//#define FOPEN                           csi_fopen
//#define FCLOSE                          csi_fclose
//#define FREAD                           csi_fread
//#define FSEEK                           csi_fseek
//#define FTELL                           csi_ftell


//==============added by zly@2011-9-16 13:30:47============================================

////==============================================================================
////                              esMEM DEFINE
////==============================================================================

#ifdef csi_pa2va
#undef csi_pa2va
#endif

#ifdef csi_va2pa
#undef csi_va2pa
#endif

#define csi_pa2va(pa, aux)             esMEMS_GetIoVAByPA((pa), (aux))

#define csi_va2pa(va)                  esMEMS_VA2PA((va))

////==============================================================================
////                              KRNL DEFINE
////==============================================================================

#ifdef csi_SemPend
#undef csi_SemPend
#endif

#ifdef csi_SemPost
#undef csi_SemPost
#endif

#ifdef csi_SemCreate
#undef csi_SemCreate
#endif

#ifdef csi_SemDel
#undef csi_SemDel
#endif

#ifdef csi_CallBack
#undef csi_CallBack
#endif

#ifdef csi_delay
#undef csi_delay
#endif

#define csi_SemPend(hdl, para, pErr)        esKRNL_SemPend((hdl), (para), (pErr))
#define csi_SemPost(hdl)                    esKRNL_SemPost(hdl)
#define csi_SemCreate(para)                 esKRNL_SemCreate(para)
#define csi_SemDel(hdl, para, pErr)         esKRNL_SemDel((hdl), (para), (pErr))
#define csi_CallBack(src, dst)              esKRNL_CallBack((src), (dst))
#define csi_delay(para)                     esKRNL_TimeDly(para)
////==============================================================================
////                              CLK MNG DEFINE
////==============================================================================

#ifdef csi_OpenMclk
#undef csi_OpenMclk
#endif

#ifdef csi_CloseMclk
#undef csi_CloseMclk
#endif

#ifdef csi_SetMclkSrc
#undef csi_SetMclkSrc
#endif

#ifdef csi_GetSrcFreq
#undef csi_GetSrcFreq
#endif

#ifdef csi_SetMclkDiv
#undef csi_SetMclkDiv
#endif

#ifdef csi_MclkOnOff
#undef csi_MclkOnOff
#endif

#define csi_OpenMclk(name)                  esCLK_OpenMclk(name)
#define csi_CloseMclk(hdl)                  esCLK_CloseMclk(hdl)
#define csi_SetMclkSrc(hdl, name)           esCLK_SetMclkSrc((hdl), (name))
#define csi_GetSrcFreq(name)                esCLK_GetSrcFreq(name)
#define csi_SetMclkDiv(hdl, div)            esCLK_SetMclkDiv((hdl), (div))
#define csi_MclkOnOff(hdl, state)           esCLK_MclkOnOff((hdl), (state))



////==============================================================================
////                              GPIO&PIN MNG DEFINE
////==============================================================================

#ifdef csi_GetPinInfo
#undef csi_GetPinInfo
#endif

#ifdef csi_pinRequest
#undef csi_pinRequest
#endif

#ifdef csi_PinGrpRel
#undef csi_PinGrpRel
#endif

#ifdef csi_GetPinStat
#undef csi_GetPinStat
#endif

#ifdef csi_SetPinStat
#undef csi_SetPinStat
#endif

#if NEW_SDK == 1
#define csi_GetPinInfo(name, pin_strc, size)               esCFG_GetGPIOSecData((name),(pin_strc),(size))
#define csi_PinGrpReq(pin_strc, size)                      esPINS_PinGrpReq((pin_strc),(size))
#define csi_PinGrpRel(hpin, bRestore)                      esPINS_PinGrpRel((hpin), (bRestore))
#define csi_GetPinStat(hpin, pPinStat, name, bHW)          esPINS_GetPinStat(hpin, pPinStat, name, bHW)
#define csi_SetPinStat(hpin, pPinStat, name, stat)         esPINS_SetPinStat(hpin, pPinStat, name, stat)
#define csi_SetPinIO(hPin, bOut, name)                     esPINS_SetPinIO(hPin, bOut, name)
#define csi_SetPinDrive(hPin, drive, name)                 esPINS_SetPinDrive(hPin, drive, name)
#define csi_ReadPinData(hPin, name)                        esPINS_ReadPinData(hPin, name)
#define csi_WritePinData(hPin, value, name)                esPINS_WritePinData(hPin, value, name)

#else
#define csi_GetPinInfo(name, pin_strc, size)               esKSRV_Script_FetchMainKey_GPIO_Data((name),(pin_strc),(size))
#define csi_PinGrpReq(pin_strc, size)                      esPINS_DevPinsReq((pin_strc),(size))
#define csi_PinGrpRel(hpin, bRestore)                      esPINS_DevPinsRel((hpin), (bRestore))
#define csi_GetPinStat(hpin, pPinStat, name, bHW)          esPINS_DevGetONEPins_Status(hpin, pPinStat, name, bHW)
#define csi_SetPinStat(hpin, pPinStat, name, stat)         esPINS_DevSetONEPin_Status(hpin, pPinStat, name, stat)
#define csi_SetPinIO(hPin, bOut, name)                     esPINS_DevSetONEPIN_IO_STATUS(hPin, bOut, name)
#define csi_SetPinDrive(hPin, drive, name)                 esPINS_DevSetONEPIN_DRIVER_LEVEL_STATUS(hPin, drive, name)
#define csi_ReadPinData(hPin, name)                        esPINS_DevREAD_ONEPIN_DATA(hPin, name)
#define csi_WritePinData(hPin, value, name)                esPINS_DevWRITE_ONEPIN_DATA(hPin, value, name)
#endif



#endif  /* _DRV_CSI_CFG_H_ */

