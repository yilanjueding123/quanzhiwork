typedef struct CHARSET_API_ENTRY
{
    __s32(*ptr_CHS_Uni2Char)(__s32 charset_type, __u16 uni, __u8 *str, __u32 len);
    __s32(*ptr_CHS_Char2Uni)(__s32 charset_type, const __u8 *str, __u32 len, __u16 *uni);
    __u32(*ptr_CHS_GetChUpperTbl)(__s32 charset_type, void *buf, __u32 size);
    __u32(*ptr_CHS_GetChLowerTbl)(__s32 charset_type, void *buf, __u32 size);
} charset_api_entry_t;



#ifndef USED_BY_CHARSET	//not used by charset module itsef
#define CHARSET_ENTRY_TBL   (0xe4400000)   //this address is depended on the link address of mod_charset
#define CHARSET_FUNC_ENTRY  ((charset_api_entry_t *)CHARSET_ENTRY_TBL)
#define CHS_Uni2Char     	(CHARSET_FUNC_ENTRY->ptr_CHS_Uni2Char	)
#define CHS_Char2Uni     	(CHARSET_FUNC_ENTRY->ptr_CHS_Char2Uni	)
#define CHS_GetChUpperTbl	(CHARSET_FUNC_ENTRY->ptr_CHS_GetChUpperTbl	)
#define CHS_GetChLowerTbl	(CHARSET_FUNC_ENTRY->ptr_CHS_GetChLowerTbl	)
#endif
