;//------------------------------------------------------------------------
;// clean or clean-and-flush cache using way/set indexing
;//----------------------------------------------------------------
;// C-prototypes
;// void  cleanDCache(void);                /* clean all Dcache */
;// void  cleanFlushDCache(void);           /* clean & flush all Dcache */
;// void  cleanFlushCache(void);            /* clean & flush Dcache by way */
;//------------------------------------------------------------------------

;//*********************************************************************************************************
;// Clean cache by region
;//*********************************************************************************************************
        include elibs_i.inc


        AREA cleanflushcachebyregion, CODE, READONLY ;// Start of Area block
        
        if  EPDK_CPU = EPDK_CPU_AC320         :LOR: \
            EPDK_CPU = EPDK_CPU_ARM920T       :LOR: \
            EPDK_CPU = EPDK_CPU_ARM922T       :LOR: \
            EPDK_CPU = EPDK_CPU_ARM946E_S     :LOR: \
            EPDK_CPU = EPDK_CPU_ARM926EJ_S    :LOR: \
            EPDK_CPU = EPDK_CPU_AC320         :LOR: \
            EPDK_CPU = EPDK_CPU_ARM1022E      :LOR: \
            EPDK_CPU = EPDK_CPU_ARM1026EJ_S   :LOR: \
            EPDK_CPU = EPDK_CPU_XSCALE        :LOR: \
            EPDK_CPU = EPDK_CPU_SA_110
            
            include cache.inc

adr         RN  0   ;//active address
size        RN  1   ;//size of region in bytes     
nl          RN  1   ;要清理或者清除的cache行数
        
            macro
            CACHEBYREGION   $op    
        
            bic     adr, adr, #(1 << CLINE) - 1         ;//clip 2 cline adr
            mov     nl, size, lsr #CLINE                ;//bytes to cline
10
            if "$op" = "IcacheFlush"
                mcr p15, 0, adr, c7, c5, 1              ;//清除i-cline@adr
            endif
            if "$op" = "DcacheFlush"
                mcr p15, 0, adr, c7, c6, 1              ;//清除d-cline@adr
            endif
            if "$op" = "IDcacheFlush"
                mcr p15, 0, adr, c7, c5, 1              ;//清除i-cline@adr
                mcr p15, 0, adr, c7, c6, 1              ;//清除d-cline@adr
            endif
            if "$op" = "DcacheClean"
                mcr p15, 0, adr, c7, c10, 1             ;//清理d-cline@adr
            endif
            if "$op" = "DcacheCleanFlush"
                if  EPDK_CPU = EPDK_CPU_XSCALE :LOR:   \
                    EPDK_CPU = EPDK_CPU_SA_110
                    mcr p15, 0, adr, c7, c10, 1         ;//清理d-cline@adr
                    mcr p15, 0, adr, c7, c6, 1          ;//清除d-cline@adr
                else
                    mcr p15, 0, adr, c7, c14, 1         ;//清除清理d-cline@adr
                endif
            endif
            if "$op" = "IDcacheCleanFlush"
                if  EPDK_CPU = EPDK_CPU_ARM920T     :LOR: \
                    EPDK_CPU = EPDK_CPU_ARM922T     :LOR: \
                    EPDK_CPU = EPDK_CPU_ARM946E_S   :LOR: \
                    EPDK_CPU = EPDK_CPU_ARM926EJ_S  :LOR: \
                    EPDK_CPU = EPDK_CPU_AC320       :LOR: \
                    EPDK_CPU = EPDK_CPU_ARM1022E    :LOR: \
                    EPDK_CPU = EPDK_CPU_ARM1026EJ_S
                    mcr p15, 0, adr, c7, c14, 1         ;//清除清理d-cline@adr
                    mcr p15, 0, adr, c7, c5, 1          ;//清除i-cline@adr
                endif
                if  EPDK_CPU = EPDK_CPU_XSCALE
                    mcr p15, 0, adr, c7, c10, 1         ;//清理d-cline@adr
                    mcr p15, 0, adr, c7, c6, 1          ;//清除d-cline@adr
                    mcr p15, 0, adr, c7, c5, 1          ;//清除i-cline@adr
                endif
            endif
        
            add     adr, adr, #(1 << CLINE)               ;//+1 next line adr
            subs    nl, nl, #1                          ;//-1 cline counter
            bne     %BT10
11            
            if  EPDK_CPU = EPDK_CPU_XSCALE
                cpwait
            endif
            mov     pc, lr
            mend
        
            if EPDK_CPU = EPDK_CPU_SA_110

                export  _eLIBs_CleanDCacheRegion
                export  _eLIBs_FlushDCacheRegion            
                export  _eLIBs_CleanFlushDCacheRegion
_eLIBs_CleanDCacheRegion
                CACHEBYREGION   DcacheClean        
_eLIBs_FlushDCacheRegion
                CACHEBYREGION   DcacheFlush
_eLIBs_CleanFlushDCacheRegion
                CACHEBYREGION   DcacheCleanFlush

            else
                export  _eLIBs_FlushICacheRegion            
                export  _eLIBs_FlushDCacheRegion            
                export  _eLIBs_FlushCacheRegion            
                export  _eLIBs_CleanDCacheRegion
                export  _eLIBs_CleanFlushDCacheRegion
                export  _eLIBs_CleanFlushCacheRegion

_eLIBs_FlushICacheRegion            
                CACHEBYREGION   IcacheFlush
_eLIBs_FlushDCacheRegion            
                CACHEBYREGION   DcacheFlush
_eLIBs_FlushCacheRegion            
                CACHEBYREGION   IDcacheFlush
_eLIBs_CleanDCacheRegion
                CACHEBYREGION   DcacheClean        
_eLIBs_CleanFlushDCacheRegion
                CACHEBYREGION   DcacheCleanFlush        
_eLIBs_CleanFlushCacheRegion
                CACHEBYREGION   IDcacheCleanFlush        
                   
            endif
        endif
        
    end
        