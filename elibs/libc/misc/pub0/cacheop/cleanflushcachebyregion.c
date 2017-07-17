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

#include "elibs.h"
#include "cache.h"

#if EPDK_CPU == EPDK_CPU_AC320          || \
    EPDK_CPU == EPDK_CPU_ARM920T        || \
    EPDK_CPU == EPDK_CPU_ARM922T        || \
    EPDK_CPU == EPDK_CPU_ARM946E_S      || \
    EPDK_CPU == EPDK_CPU_ARM926EJ_S     || \
    EPDK_CPU == EPDK_CPU_ARM1022E       || \
    EPDK_CPU == EPDK_CPU_ARM1026EJ_S    || \
    EPDK_CPU == EPDK_CPU_XSCALE

extern void _eLIBs_FlushICacheRegion     (void *adr, __u32 bytes);
extern void _eLIBs_FlushDCacheRegion     (void *adr, __u32 bytes);
extern void _eLIBs_FlushCacheRegion      (void *adr, __u32 bytes);
extern void _eLIBs_CleanDCacheRegion     (void *adr, __u32 bytes);
extern void _eLIBs_CleanFlushDCacheRegion(void *adr, __u32 bytes);
extern void _eLIBs_CleanFlushCacheRegion (void *adr, __u32 bytes);


void eLIBs_CleanDCacheRegion(void *adr, __u32 bytes)
{
    _eLIBs_CleanDCacheRegion(adr, bytes + (1 << CLINE) * 2 - 2);
}
void eLIBs_CleanFlushDCacheRegion(void *adr, __u32 bytes)
{
    _eLIBs_CleanFlushDCacheRegion(adr, bytes + (1 << CLINE) * 2 - 2);
}
void eLIBs_CleanFlushCacheRegion(void *adr, __u32 bytes)
{
    _eLIBs_CleanFlushCacheRegion(adr, bytes + (1 << CLINE) * 2 - 2);
}
void eLIBs_FlushICacheRegion(void *adr, __u32 bytes)
{
    _eLIBs_FlushICacheRegion(adr, bytes + (1 << CLINE) * 2 - 2);
}

void eLIBs_FlushDCacheRegion(void *adr, __u32 bytes)
{
    _eLIBs_CleanFlushDCacheRegion(adr, 1 << CLINE);
    _eLIBs_CleanFlushDCacheRegion((void *)((__u32)adr + bytes), 1 << CLINE);
    if(bytes > (1 << CLINE))
    {
        _eLIBs_FlushDCacheRegion((void *)((__u32)adr + (1 << CLINE)), bytes);
    }
}
void eLIBs_FlushCacheRegion(void *adr, __u32 bytes)
{
    _eLIBs_CleanFlushCacheRegion(adr, 1 << CLINE);
    _eLIBs_CleanFlushCacheRegion((void *)((__u32)adr + bytes), 1);
    if(bytes > (1 << CLINE))
    {
        _eLIBs_FlushCacheRegion((void *)((__u32)adr + (1 << CLINE)), bytes);
    }
}

#endif

#if    EPDK_CPU == EPDK_CPU_SA_110

extern void _eLIBs_FlushDCacheRegion     (void);
extern void _eLIBs_CleanDCacheRegion     (void);
extern void _eLIBs_CleanFlushDCacheRegion(void);

void eLIBs_CleanDCacheRegion(void *adr, __u32 bytes)
{
    _eLIBs_CleanDCacheRegion(adr, bytes + (1 << CLINE) * 2 - 2);
}
void eLIBs_CleanFlushDCacheRegion(void *adr, __u32 bytes)
{
    _eLIBs_CleanFlushDCacheRegion(adr, bytes + (1 << CLINE) * 2 - 2);
}
void eLIBs_FlushDCacheRegion(void *adr, __u32 bytes)
{
    _eLIBs_CleanFlushDCacheRegion(adr, 1);
    _eLIBs_CleanFlushDCacheRegion((void *)((__u32)adr + bytes), 1);
    if(bytes > (1 << CLINE))
    {
        _eLIBs_FlushDCacheRegion((void *)((__u32)adr + (1 << CLINE)), bytes);
    }
}

#endif
