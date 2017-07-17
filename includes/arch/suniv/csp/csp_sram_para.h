/*
*********************************************************************************************************
*											        eBase
*						                the Abstract of Hardware
*									           the OAL of CSP
*
*						        (c) Copyright 2006-2010, AW China
*											All	Rights Reserved
*
* File    	: 	csp_sram_para.h
* Date    	:	2011-01-24
* By      	: 	holigun
* Version 	: 	V1.00
* Description :            just for aw1619
* History     :
*               <author>            <date>              <note>
*               victor              2011-01-24          aw1619
*********************************************************************************************************
*/
#ifndef	_CSP_SRAM_PARA_H_
#define	_CSP_SRAM_PARA_H_


//------ZONE--------------------------------------
typedef	enum
{
    CSP_SRAM_ZONE_NULL		=	0x00,

    CSP_SRAM_ZONE_BOOT              ,   /* 可使用的模块为: BOOT或NORMAL. 注意:OS务必将这个区域设置为NORMAL  */

    CSP_SRAM_ZONE_B1		        ,   /* 可使用的模块为: CPU_DMA或USB */
    CSP_SRAM_ZONE_C1		        ,   /* 可使用的模块为: CPU_DMA或VE  */
    CSP_SRAM_ZONE_C2		        ,   /* 可使用的模块为: DE_FE */
    CSP_SRAM_ZONE_C3		        ,   /* 可使用的模块为: DE_BE */

    CSP_SRAM_ZONE_ICACHE	        ,   /* not used */
    CSP_SRAM_ZONE_DCACHE			,   /* not used */

    CSP_SRAM_ZONE_MAX_NR
} csp_sram_zone_id_t;

//------Module--------------------------------------
typedef enum
{
    CSP_SRAM_MODULE_NULL	=	0x00,

    CSP_SRAM_MODULE_BOOT			, /* boot模块  : zone c1,c2,c3作启动用   */
    CSP_SRAM_MODULE_NORMAL          , /* normal模块: zone c1(CPU_DMA/VE),zone c2(DE_FE),zone c3(DE_BE) */

    CSP_SRAM_MODULE_CPU_DMA	        ,
    CSP_SRAM_MODULE_USB	            ,
    CSP_SRAM_MODULE_VE		        ,
    CSP_SRAM_MODULE_DE_FE	        ,
    CSP_SRAM_MODULE_DE_BE	        ,

    CSP_SRAM_MODULE_MAX_NR
} csp_sram_module_t;


typedef struct  sram_zone_info
{
    csp_sram_zone_id_t zone_id;
    u32 reserved;	//u32 zone_size;
} sram_zone_info_t;


#endif	//_CSP_SRAM_OPS_H_

