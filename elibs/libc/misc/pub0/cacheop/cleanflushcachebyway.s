;//------------------------------------------------------------------------
;// clean or clean-and-flush cache using way/set indexing
;//----------------------------------------------------------------
;// C-prototypes
;// void  cleanDCache(void);                /* clean all Dcache */
;// void  cleanFlushDCache(void);           /* clean & flush all Dcache */
;// void  cleanFlushCache(void);            /* clean & flush Dcache by way */
;//------------------------------------------------------------------------


;//*********************************************************************************************************
;// Clean cache by way
;//*********************************************************************************************************
        AREA cleanflushcachebyway, CODE, READONLY ;// Start of Area block
        
        include elibs_i.inc
    
        if  EPDK_CPU = EPDK_CPU_ARM920T       :LOR: \
            EPDK_CPU = EPDK_CPU_ARM922T       :LOR: \
            EPDK_CPU = EPDK_CPU_ARM940T       :LOR: \
            EPDK_CPU = EPDK_CPU_ARM946E_S     :LOR: \
            EPDK_CPU = EPDK_CPU_ARM1022E
            
            export  eLIBs_CleanDCache
            export  eLIBs_CleanFlushDCache
            export  eLIBs_CleanFlushCache

            include cache.inc

c7f         RN  0   ;// cp15:c7  register format

            macro
            CACHECLEANBYWAY $op
       
            mov     c7f, #0                     ;// create c7 format
5
            if      "$op" = "Dclean"
                mcr     p15, 0, c7f, c7, c10, 2   	;// clean D-cline
            ENDIF
            if      "$op" = "Dcleanflush"
                mcr     p15, 0, c7f, c7, c14, 2   	;// cleanflush D-cline
            ENDIF

            add     c7f, c7f, #1<<I7SET         ;// +1 set index
            
            ;//TST     c7f, #1<<(NSET+I7SET)       ;// test index overflow
            TST     c7f, #1<<(DCNSET+I7SET)       ;// test index overflow
            BEQ     %BT5 

            ;//BIC     c7f, c7f, #1<<(NSET+I7SET)  ;// clear index overflow
            BIC     c7f, c7f, #1<<(DCNSET+I7SET)  ;// clear index overflow
            ADDS    c7f, c7f, #1<<I7WAY         ;// +1 victim pointer
            BCC     %BT5                        ;// test way overflow
            
            mend

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
                if  EPDK_CPU = EPDK_CPU_ARM940T         :LOR:  \
                    EPDK_CPU = EPDK_CPU_ARM946E_S
                    mcr p15, 0, c7f, c7, c5, 0
                    mcr p15, 0, c7f, c7, c6, 0
                else
                    mcr p15, 0, c7f, c7, c7, 0      ;//flush id-cache
                endif
            endif
            mov     pc, lr
            mend

eLIBs_CleanDCache
            CACHECLEANBYWAY Dclean
            mov     pc, lr
eLIBs_CleanFlushDCache
            CACHECLEANBYWAY Dcleanflush
            mov     pc, lr
eLIBs_CleanFlushCache
            CACHECLEANBYWAY Dcleanflush
            mcr     p15,0,r0,c7,c5,0    		;// flush I-cache
            mov     pc, lr

            if EPDK_CPU = EPDK_CPU_ARM720T
                export  eLIBs_FlushCache
eLIBs_FlushCache                
                CACHEFLUSH IDcache
            else
                export  eLIBs_FlushICache
                export  eLIBs_FlushDCache
                export  eLIBs_FlushCache

eLIBs_FlushICache
                CACHEFLUSH  Icache     
eLIBs_FlushDCache
                CACHEFLUSH  Dcache     
eLIBs_FlushCache
                CACHEFLUSH  IDcache     
            endif
        endif



        
    end
        