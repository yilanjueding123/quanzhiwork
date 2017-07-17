/*
*********************************************************************************************************
*											        eBase
*						                the Abstract of Hardware
*									           the OAL of DMA
*
*						        (c) Copyright 2006-2010, AW China
*											All	Rights Reserved
*
* File    	: 	dma.h
* Date		:	2010-01-21
* By      	: 	holigun
* Version 	: 	V1.00
* Description :             just for aw1619
* History     :
*
*********************************************************************************************************
*/
#ifndef	__CSP_DMAC_OPS_H__
#define	__CSP_DMAC_OPS_H__


/*
****************************************************************************************************
*
*             DMAC_Init
*
*  Description:
*       dma controller init
*
*  Parameters:
*
*  Return value:
*		EBSP_TRUE/FAIL
****************************************************************************************************
*/
__s32 CSP_DMAC_init(void);

/*
****************************************************************************************************
*
*             DMAC_Exit
*
*  Description:
*       dma controller exit
*
*  Parameters:
*
*  Return value:
*		EPDK_OK/BIOS_ERROR
****************************************************************************************************
*/
__s32 CSP_DMAC_exit(void);

/*
****************************************************************************************************
*
*             CSP_DMAC_GetIrqNum
*
*  Description:
*       get dma irq number
*
*  Parameters:
*
*  Return value:
*		dma int number
****************************************************************************************************
*/
__s32 CSP_DMAC_GetIrqNum(void);

/*
****************************************************************************************************
*
*             DMAC_request
*
*  Description:
*       request dma
*
*  Parameters:
*		type	0: normal timer
*				1: special timer
*  Return value:
*		dma handler
*		if 0, fail
****************************************************************************************************
*/
__hdle CSP_DMAC_request(__u32 dmatype);

/*
****************************************************************************************************
*
*             DMAC_release
*
*  Description:
*       release dma
*
*  Parameters:
*       hDma	dma handler
*
*  Return value:
*		EPDK_OK/BIOS_ERROR
****************************************************************************************************
*/
__s32 CSP_DMAC_release(__hdle hDma);

/*
****************************************************************************************************
*
*             DMAC_setting
*
*  Description:
*       start interrupt
*
*  Parameters:
*       hTmr	timer handler
*		pArg    *(pArg + 0)         ctrl
*               *(pArg + 1)         page size
*               *(pArg + 2)         page step
*               *(pArg + 3)         comity & block count
*
*  Return value:
*		EPDK_OK/BIOS_ERROR
*
**********************************************************************************************************************
*/
__s32 CSP_DMAC_setting(__hdle hDMA, struct CSP_dma_config *p_config);


/*
**********************************************************************************************************************
*
*             DMAC_start
*
*  Description:
*       start interrupt
*
*  Parameters:
*       hTmr	timer handler
*
*  Return value:
*		EPDK_OK/BIOS_ERROR
*
****************************************************************************************************
*/
__s32 CSP_DMAC_start(__hdle hDMA, __u32 saddr, __u32 daddr, __u32 bytes);

/*
**********************************************************************************************************************
*
*             DMAC_stop
*
*  Description:
*       stop dma
*
*  Parameters:
*       hDma	dma handler
*
*  Return value:
*		EPDK_OK/BIOS_ERROR
*
**********************************************************************************************************************
*/
__s32 CSP_DMAC_stop(__hdle hDma);

/*
**********************************************************************************************************************
*
*             CSP_DMAC_restart
*
*  Description:
*       start dma
*
*  Parameters:
*       hDma	dma handler
*
*  Return value:
*		EPDK_OK/BIOS_ERROR
*
**********************************************************************************************************************
*/
__s32 CSP_DMAC_restart(__hdle hDma);

/*
**********************************************************************************************************************
*                                       CSP_DMAC_query_stat
*
* Description: This function is used to query dma transfer status, busy or not
*
* Arguments  : hDma         dma handle
*
* Returns    :
*
*
*
**********************************************************************************************************************
*/
__u32  	CSP_DMAC_query_stat(__hdle hDma);

/*
**********************************************************************************************************************
*                                       CSP_DMAC_query_src
*
* Description: This function is used to query dma transfer source address
*
* Arguments  : hDma         dma handle
*
* Returns    :
*
*
*
**********************************************************************************************************************
*/
__u32  	CSP_DMAC_query_src (__hdle hDma);


/*
**********************************************************************************************************************
*                                       CSP_DMAC_query_dst
*
* Description: This function is used to query dma transfer destination address
*
* Arguments  : hDma         dma handle
*
* Returns    :
*
*
*
**********************************************************************************************************************
*/
__u32  	CSP_DMAC_query_dst(__hdle hDma);

/*
**********************************************************************************************************************
*
*             CSP_DMAC_query_rest_bytecnt
*
*  Description:
*       start interrupt
*
*  Parameters:
*       hTmr	timer handler
*
*  Return value:
*		EPDK_OK/BIOS_ERROR
*
****************************************************************************************************
*/
__u32 CSP_DMAC_query_rest_bytecnt(__hdle hDMA);


/*
**********************************************************************************************************************
*
*             DMAC_query_bytecnt
*
*  Description:
*       start interrupt
*
*  Parameters:
*       hTmr	timer handler
*
*  Return value:
*		EPDK_OK/BIOS_ERROR
*
****************************************************************************************************
*/
__u32 CSP_DMAC_query_bytecnt(__hdle hDMA);


/*
**********************************************************************************************************************
*
*             CSP_DMAC_query_rest_bytecnt
*
*  Description:
*       start interrupt
*
*  Parameters:
*       hTmr	timer handler
*
*  Return value:
*		EPDK_OK/BIOS_ERROR
*
****************************************************************************************************
*/
__u32 CSP_DMAC_query_rest_bytecnt(__hdle hDMA);


/*
**********************************************************************************************************************
*
*             DMAC_change_mode
*
*  Description:
*       start interrupt
*
*  Parameters:
*       hTmr	timer handler
*
*  Return value:
*		EPDK_OK/BIOS_ERROR
*
****************************************************************************************************
*/
__s32 CSP_DMAC_set_mode(__hdle hDMA, __u32 mode);

/*
**********************************************************************************************************************
*                                       CSP_DMAC_query_int
*
* Description: This function is used to query interrupt pending
*
* Arguments  : hDma         dma handle
*              xDma
*                   bit1         dma end interrupt flag
*                   bit0         dma half end interrupt flag
*
* Returns    : EPDK_OK/BIOS_ERROR
**********************************************************************************************************************
*/
__s32  CSP_DMAC_query_int(__hdle dma, __s32 type);

/*
**********************************************************************************************************************
*                                       CSP_DMAC_clear_pending
*
* Description: This function is used to clear interrupt pending
*
* Arguments  : hDma         dma handle
*              xDma
*                   bit1         dma end interrupt flag
*                   bit0         dma half end interrupt flag
*
* Returns    : EPDK_OK/BIOS_ERROR
**********************************************************************************************************************
*/
__s32 CSP_DMAC_clear_pending(__hdle dma, __s32 type);


/*
**********************************************************************************************************************
*                                       CSP_DMAC_enable_int
*
* Description: This function is used to enable dma int
*
* Arguments  : hDma         dma handle
*              type
*                   bit1         dma end interrupt flag
*                   bit0         dma half end interrupt flag
*
* Returns    : EPDK_OK/BIOS_ERROR
**********************************************************************************************************************
*/
__s32 CSP_DMAC_enable_int(__hdle hDma, __u32 type);

/*
**********************************************************************************************************************
*                                       CSP_DMAC_disable_int
*
* Description: This function is used to disable dma int
*
* Arguments  : hDma         dma handle
*              xDma
*                   bit1         dma end interrupt flag
*                   bit0         dma half end interrupt flag
*
* Returns    : EPDK_OK/BIOS_ERROR
**********************************************************************************************************************
*/
__s32 CSP_DMAC_disable_int(__hdle hDma, __u32 xDma);



/*
**********************************************************************************************************************
*                                               CSP_DMAC_Get_virt_base
*
* Description:
*				get int virt base , for extend
* Arguments  :
*
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__u32 CSP_DMAC_Get_virt_base(void);



#endif	//__CSP_DMAC_OPS_H__

