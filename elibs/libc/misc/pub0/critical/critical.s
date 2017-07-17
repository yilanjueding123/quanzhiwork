;//*********************************************************************************************************
;//*											        eLIBC
;//*							            	the embedded lib for c
;//*
;//*						        (c) Copyright 2006-2007, Steven.ZGJ China
;//*											All	Rights Reserved
;//*
;//* File   : critical.s
;//* Version: V1.0
;//* By     : steven.zgj
;//*********************************************************************************************************
    include elibs_i.inc
NO_INT      equ     0xc0 

		export  CPU_SR_Save
		export  CPU_SR_Restore
		
        area 	CODE, CODE, READONLY
        
CPU_SR_Save
        mrs     r0, cpsr                 ;// Set IRQ and FIQ bits in CPSR to disable all interrupts
        orr     r1, r0, #NO_INT
        msr     cpsr_c,r1
        mov     pc, lr                   ;// Disabled, return the original CPSR contents in R0
CPU_SR_Restore
        msr     cpsr_c, r0
        mov     pc, lr
        
        end