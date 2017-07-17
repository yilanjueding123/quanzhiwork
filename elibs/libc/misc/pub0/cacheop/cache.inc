;///*
;//*********************************************************************************************************
;//*											        ePOS
;//*						           the Easy Portable/Player Operation System
;//*									           MEMS sub-system
;//*
;//*						        (c) Copyright 2006-2007, Steven.ZGJ China
;//*											All	Rights Reserved
;//*
;//* File   : cache.inc
;//* Version: V1.0
;//* By     : steven.zgj
;//*********************************************************************************************************
;//*/
        if :LNOT::DEF:_CACHE_INC_
	    GBLA          _CACHE_INC_


        IF EPDK_CPU = EPDK_CPU_ARM920T 
CSIZE                   EQU 14  ;// cache size as 2**CSIZE (16 K assumed)
CLINE                   EQU  5  ;// cache line size in bytes as 2**CLINE
NWAY                    EQU  6  ;// set associativity = 2**NWAY (64 way)
I7SET                   EQU  5  ;// CP15 c7 set incrementer as 2**ISET
I7WAY                   EQU 26  ;// CP15 c7 way incrementer as 2**SSET
I9WAY                   EQU 26  ;// CP15 c9 way incrementer as 2**SSET
        ENDIF
        
        IF EPDK_CPU = EPDK_CPU_ARM922T 
CSIZE                   EQU 14  ;// cache size as 2**CSIZE (16 K assumed)
CLINE                   EQU  5  ;// cache line size in bytes as 2**CLINE
NWAY                    EQU  6  ;// set associativity = 2**NWAY (64 way)
I7SET                   EQU  5  ;// CP15 c7 set incrementer as 2**ISET
I7WAY                   EQU 26  ;// CP15 c7 way incrementer as 2**SSET
I9WAY                   EQU 26  ;// CP15 c9 way incrementer as 2**SSET
        ENDIF
        
        IF EPDK_CPU = EPDK_CPU_ARM926EJ_S 
;//CSIZE                   EQU 15  ;// cache size as 2**CSIZE (16 K assumed)
ICSIZE                  EQU 15  ;// cache size as 2**CSIZE (32 K assumed)
DCSIZE                  EQU 14  ;// cache size as 2**CSIZE (16 K assumed)
CLINE                   EQU  5  ;// cache line size in bytes as 2**CLINE
NWAY                    EQU  2  ;// set associativity = 2**NWAY (4 way)
I7SET                   EQU  5  ;// CP15 c7 set incrementer as 2**ISET
I7WAY                   EQU 30  ;// CP15 c7 way incrementer as 2**IWAY
        ENDIF
       
        IF EPDK_CPU = EPDK_CPU_ARM940T 
CSIZE                   EQU 12  ;// cache size as 2**CSIZE (4K)
CLINE                   EQU  4  ;// cache line size in bytes as 2**CLINE
NWAY                    EQU  6  ;// set associativity = 2**NWAY (64 way)
I7SET                   EQU  4  ;// CP15 c7 set incrementer = 2**ISET
I7WAY                   EQU 26  ;// CP15 c7 way incrementer = 2**IWAY
I9WAY                   EQU 0   ;// CP15 c9 way incrementer = 2**IWAY
        ENDIF
        
        IF EPDK_CPU = EPDK_CPU_ARM946E_S 
CSIZE                   EQU 12  ;// cache size as 2**CSIZE (4 K assumed)
CLINE                   EQU  5  ;// cache line size in bytes as 2**CLINE
NWAY                    EQU  2  ;// set associativity = 2**NWAY (4 way)
I7SET                   EQU  4  ;// CP15 c7 set incrementer = 2**ISET
I7WAY                   EQU 30  ;// CP15 c7 way incrementer = 2**IWAY
I9WAY                   EQU  0  ;// CP15 c7 way incrementer = 2**IWAY
        ENDIF
        
        IF EPDK_CPU = EPDK_CPU_ARM1022E 
CSIZE                   EQU 14  ;// cache size as 2**CSIZE (16 K)
CLINE                   EQU  5  ;// cache line size in bytes as 2**CLINE
NWAY                    EQU  6  ;// set associativity = 2**NWAY (64 way)
I7SET                   EQU  5  ;// CP15 c7 set incrementer as 2**ISET
I7WAY                   EQU 26  ;// CP15 c7 way incrementer as 2**SSET
I9WAY                   EQU 26  ;// CP15 c7 way incrementer = 2**IWAY
        ENDIF
        
        IF EPDK_CPU = EPDK_CPU_ARM1026EJ_S 
CSIZE                   EQU 14  ;// cache size as 2**CSIZE (16 K assumed)
CLINE                   EQU  5  ;// cache line size in bytes as 2**CLINE
NWAY                    EQU  2  ;// set associativity = 2**NWAY (4 way)
I7SET                   EQU  5  ;// CP15 c7 set incrementer as 2**ISET
I7WAY                   EQU 30  ;// CP15 c7 way incrementer as 2**IWAY
        ENDIF
        
        IF EPDK_CPU = EPDK_CPU_SA_110 
CSIZE                   EQU 14  ;// cache size as 2**CSIZE (16 K)
CLINE                   EQU  5  ;// cache line size in bytes as 2**CLINE
NWAY                    EQU  5  ;// set associativity = 2**NWAY (4 way)
CleanAddressDcache      EQU 0x8000
        ENDIF
        
        IF EPDK_CPU = EPDK_CPU_XSCALE 
CSIZE                   EQU 15  ;// cache size as 2**CSIZE (32 K)
CLINE                   EQU  5  ;// cache line size in bytes as 2**CLINE
NWAY                    EQU  5  ;// set associativity = 2**NWAY (32 way)
MNWAY                   EQU  1  ;// set assoc mini D-cache = 2**NWAY (2 way)
MCSIZE                  EQU 11  ;// mini cache size  as 2**CSIZE (2 K)
CleanAddressDcache      EQU 0x8000  ;//(32K block 0x8000-0x10000)
CleanAddressMiniDcache  EQU 0x10000 ;//(2K block 0x10000-0x10800)
        ENDIF

;   ----------------
DCSWAY                    EQU (DCSIZE-NWAY)            ;//dcache size of way = 2**SWAY
DCNSET                    EQU (DCSIZE-NWAY-CLINE)      ;//d cachelines per way = 2**NSET
ICSWAY                    EQU (ICSIZE-NWAY)            ;//i cache size of way = 2**SWAY
ICNSET                    EQU (ICSIZE-NWAY-CLINE)      ;//i cachelines per way = 2**NSET
        MACRO  
        CPWAIT  
        MRC     p15, 0, r12, c2, c0, 0  ;// read any CP15
        MOV     r12, r12 
        SUB     pc, pc, #4              ;// branch to next instruction
        MEND
 	
 	    endif     ;//  IF :LNOT::DEF:_CACHE_INC_
	    end       ;//  end of s3c2440.inc 
	
	