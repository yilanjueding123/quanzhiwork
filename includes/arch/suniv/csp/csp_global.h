/*
*********************************************************************************************************
*											        eBase
*						                the Abstract of Hardware
*									           the OAL of DMA
*
*						        (c) Copyright 2006-2010, AW China
*											All	Rights Reserved
*
* File    	: 	csp_global.h
* Date		:	2010-11-22
* By      	: 	holigun
* Version 	: 	V1.00
*********************************************************************************************************
*/
#ifndef	__CSP_GLOBAL_H__
#define	__CSP_GLOBAL_H__

#define	CSP_GLOBAL_MODULE_NR	80		//实际< 70

#define	CSP_GLOBAL_MOD_TYPE_NULL		0x00
#define	CSP_GLOBAL_MOD_TYPE_IO			0x01
#define	CSP_GLOBAL_MOD_TYPE_MEMORY		0x02

typedef struct module_info_item
{
    u32 phy_base_addr;	//module`s io memory base address
    u32 phy_io_size;	//io memory space
    u32 mod_type;		//such as CSP_GLOBAL_MOD_TYPE_NULL
} module_info_item_t;

typedef	struct ic_module_info
{
    u32 module_nr;		//module number
    struct module_info_item items[CSP_GLOBAL_MODULE_NR];
} ic_module_info_t;


/*
**********************************************************************************************************************
*                                               CSP_GLOBAL_get_module_info
*
* Description:
*		获得IC中各个模块的IO空间开始地址，
* Arguments  :
*
* Returns    :
*		本IC中各个module的phy addr，以及io空间的长度等信息
* Notes      :
*
**********************************************************************************************************************
*/
struct ic_module_info *CSP_GLOBAL_get_module_info( void );

#endif	//__CSP_GLOBAL_H__

