/*
*********************************************************************************************************
*											        eBase
*						                the Abstract of Hardware
*									           the OAL of CSP
*
*						        (c) Copyright 2006-2010, AW China
*											All	Rights Reserved
*
* File    	: 	csp_int_ops.c
* Date		:	2010-11-24
* By      	: 	holigun
* Version 	: 	V1.00
* Note        :
*       aw1619的中断cps接口
*********************************************************************************************************
*/

#ifndef	_CSP_INTC_OPS_H_
#define	_CSP_INTC_OPS_H_

/*
**********************************************************************************************************************
*                                               CSP_INTC_init
*
* Description:
*				init
* Arguments  :
*
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_INTC_init( void );

/*
**********************************************************************************************************************
*                                               CSP_INTC_exit
*
* Description:
*				exit
* Arguments  :
*
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_INTC_exit(void);

/*
**********************************************************************************************************************
*                                               CSP_INTC_get_int_src_max
*
* Description:
*				get max int src
* Arguments  :
*
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_INTC_get_int_src_max(void);

/*
**********************************************************************************************************************
*                                               CSP_INTC_set_VTAddr
*
* Description:
*				get max int src
* Arguments  :
*		vtaddr	:	table addr
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
void  CSP_INTC_set_VTAddr(__u32 vtaddr);

/*
**********************************************************************************************************************
*                                               CSP_INTC_get_INT_vector
*
* Description:
*				get current int number
* Arguments  :
*
* Returns    :
*		irq number
* Notes      :
*
**********************************************************************************************************************
*/
__u32  CSP_INTC_get_INT_vector(void);

/*
**********************************************************************************************************************
*                                               CSP_INTC_pending_get
*
* Description:
*				pending get
* Arguments  :
*		irqno		:	irq number
*		is_pending	:	==0 no pending ,!= 0 pending
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_INTC_pending_get(__u32 irqno , __u32 *is_pending);


/*
**********************************************************************************************************************
*                                               CSP_INTC_NMI_bitmap_get
*
* Description:
*				get NMI int channel`s bitmap
* Arguments  :
*		buff_len	:	p_buff len,(Byte)
*		p_buff		:	buff,推荐
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_INTC_NMI_bitmap_get(__u32 buff_len , __u32 *p_buff);


/*
**********************************************************************************************************************
*                                               CSP_INTC_NMI_trigger_set
*
* Description:
*				pending get
* Arguments  :
*		irqno		:	irq number
*		trigger_mod	:	such as INTC_NMI_TRIGGER_MOD_LOW_LEVEL
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_INTC_NMI_trigger_set(__u32 irqno , __u32 trigger_mod);


/*
**********************************************************************************************************************
*                                               CSP_INTC_INT_enable
*
* Description:
*				enable int
* Arguments  :
*		irqno		:	irq number
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_INTC_INT_enable(__u32 irqno);

/*
**********************************************************************************************************************
*                                               CSP_INTC_INT_disable
*
* Description:
*				disable int
* Arguments  :
*		irqno		:	irq number
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_INTC_INT_disable(__u32 irqno);

/*
**********************************************************************************************************************
*                                               CSP_INTC_MASK_get
*
* Description:
*				Mask get
* Arguments  :
*		irqno_start		:	从第几个irq 开始,base == 0
*		counter			:	共多少个
*		mask_bits		:	== 1 masked ,== 0 not masked
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_INTC_MASK_get(__u32 buff_len , __u32 *mask_bits);

/*
**********************************************************************************************************************
*                                               CSP_INTC_MASK_set
*
* Description:
*				Mask set
* Arguments  :
*		irqno_start		:	从第几个irq 开始,base == 0
*		counter			:	共多少个
*		mask_bits		:	bits
* Returns    :
*		EBSP_TRUE/EBSP_FALSE
* Notes      :
*
**********************************************************************************************************************
*/
__s32 CSP_INTC_MASK_set(__u32 buff_len , __u32 *mask_bits);


/*
**********************************************************************************************************************
*                                               CSP_INTC_Get_virt_base
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
__u32 CSP_INTC_Get_virt_base(void);

#endif	//_CSP_INTC_OPS_H_
