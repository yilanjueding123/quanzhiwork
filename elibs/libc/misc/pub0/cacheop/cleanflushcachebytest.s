;//------------------------------------------------------------------------
;// clean or clean-and-flush cache using way/set indexing
;//----------------------------------------------------------------
;// C-prototypes
;// void  cleanDCache(void);                /* clean all Dcache */
;// void  cleanFlushDCache(void);           /* clean & flush all Dcache */
;// void  cleanFlushCache(void);            /* clean & flush Dcache by way */
;//------------------------------------------------------------------------

;//*********************************************************************************************************
;// test clean cache
;//*********************************************************************************************************
        AREA cleanflushcachebytest, CODE, READONLY ;// Start of Area block
        
        include elibs_i.inc
        
        if  EPDK_CPU = EPDK_CPU_AC320           :LOR: \
		    EPDK_CPU = EPDK_CPU_ARM926EJ_S      :LOR: \
            EPDK_CPU = EPDK_CPU_ARM1026EJ_S     
		    
            export  eLIBs_CleanDCache
            export  eLIBs_CleanFlushDCache
            export  eLIBs_CleanFlushCache
            export  eLIBs_FlushICache
            export  eLIBs_FlushDCache
            export  eLIBs_FlushCache
            
            
            include cache.inc

c7f         RN      0
            macro
            CACHEFLUSH  $op
            
            mov     c7f, #0
            if  "$op" = "Icache"
                mcr p15, 0, c7f, c7, c5, 0      ;//flush i-cache
            endif
            if  "$op" = "Dcache"
                mcr p15, 0, c7f, c7, c6, 0      ;//flush d-cache
            endif
            if  "$op" = "IDcache"
                mcr p15, 0, c7f, c7, c7, 0      ;//flush id-cache
            endif
            mov     pc, lr
            
            mend

eLIBs_CleanDCache
		    mrc		p15, 0, pc, c7, c10, 3		;//test-clean d cache
		    bne		eLIBs_CleanDCache
		    mov		pc, lr
eLIBs_CleanFlushDCache
		    mrc		p15, 0, pc, c7, c14, 3		;//test-clean and flush d cache
		    bne		eLIBs_CleanFlushDCache
		    mov		pc, lr
eLIBs_CleanFlushCache
		    mrc		p15, 0, pc, c7, c14, 3		;//test-clean cache
		    bne		eLIBs_CleanFlushCache
		    mcr		p15, 0, r0, c7, c5, 0
		    mov		pc, lr

eLIBs_FlushICache
            CACHEFLUSH  Icache     
eLIBs_FlushDCache
            CACHEFLUSH  Dcache     
eLIBs_FlushCache
            CACHEFLUSH  IDcache     
		
		endif
        
    end
        