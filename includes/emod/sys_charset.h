/*
************************************************************************************************************************
*                                                    Charset
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : sys_charset.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.08.22
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.08.22       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  _SYS_CHARSET_H_
#define  _SYS_CHARSET_H_



typedef  enum
{
    EPDK_CHARSET_ENM_GB2312  = 0,       // 简体中文
    EPDK_CHARSET_ENM_UTF8       ,       // utf8
    EPDK_CHARSET_ENM_UTF16BE    ,       // utf16be
    EPDK_CHARSET_ENM_UTF16LE    ,       // utf16le
    EPDK_CHARSET_ENM_BIG5       ,       // 繁体中文
    EPDK_CHARSET_ENM_GBK        ,       // 中文
    EPDK_CHARSET_ENM_SJIS       ,       // 日文
    EPDK_CHARSET_ENM_EUC_JP     ,       // 日文, 暂不支持
    EPDK_CHARSET_ENM_EUC_KR     ,       // 韩文
    EPDK_CHARSET_ENM_KIO8_R     ,       // 俄文
    EPDK_CHARSET_ENM_ISO_8859_1 ,       // cp1252.西欧语言
    EPDK_CHARSET_ENM_ISO_8859_2 ,       // 中欧语言
    EPDK_CHARSET_ENM_ISO_8859_3 ,       // 南欧语言
    EPDK_CHARSET_ENM_ISO_8859_4 ,       // 北欧语言
    EPDK_CHARSET_ENM_ISO_8859_5 ,       // 西里尔字母
    EPDK_CHARSET_ENM_ISO_8859_6 ,       // 阿拉伯语
    EPDK_CHARSET_ENM_ISO_8859_7 ,       // 希腊语
    EPDK_CHARSET_ENM_ISO_8859_8 ,       // 希伯来语, 暂不支持
    EPDK_CHARSET_ENM_ISO_8859_9 ,       // cp1254，土耳其语
    EPDK_CHARSET_ENM_ISO_8859_10,       // 北欧斯堪的纳维亚语系
    EPDK_CHARSET_ENM_ISO_8859_11,       // 泰语, 暂不支持
    EPDK_CHARSET_ENM_ISO_8859_12,       // 梵文, 暂不支持
    EPDK_CHARSET_ENM_ISO_8859_13,       // 波罗的海语系
    EPDK_CHARSET_ENM_ISO_8859_14,       // 凯尔特人语系
    EPDK_CHARSET_ENM_ISO_8859_15,       // 扩展了法语和芬兰语的西欧语系
    EPDK_CHARSET_ENM_ISO_8859_16,       // 扩展的东南欧语系, 暂不支持
    EPDK_CHARSET_ENM_CP874      ,       // 泰文
    EPDK_CHARSET_ENM_CP1250     ,       // 中欧
    EPDK_CHARSET_ENM_CP1251     ,       // 西里尔文
    EPDK_CHARSET_ENM_CP1253     ,       // 希腊文, 暂不支持
    EPDK_CHARSET_ENM_CP1255     ,       // 希伯来文
    EPDK_CHARSET_ENM_CP1256     ,       // 阿拉伯文, 暂不支持
    EPDK_CHARSET_ENM_CP1257     ,       // 波罗的海文, 暂不支持
    EPDK_CHARSET_ENM_CP1258     ,       // 越南, 暂不支持
    EPDK_CHARSET_ENM_CP1252     ,       // 西欧语言。cp1252是iso-8859-1的超集
    EPDK_CHARSET_ENM_CP1254     ,       // 土耳其语。cp1254是iso-8859-9的超集
    EPDK_CHARSET_ENM_JIS        ,       // 日文JIS字符编码,暂不支持
    EPDK_CHARSET_ENM_NUM        ,

    EPDK_CHARSET_ENM_UNKNOWN = -1       // always the last one
} __epdk_charset_enm_e;


#define EPDK_CHARSET_NAME_GB2312        "CHARSET_GB2312"
#define EPDK_CHARSET_NAME_UTF8          "CHARSET_UTF8"
#define EPDK_CHARSET_NAME_UTF16BE       "CHARSET_UTF16BE"
#define EPDK_CHARSET_NAME_UTF16LE       "CHARSET_UTF16LE"
#define EPDK_CHARSET_NAME_BIG5          "CHARSET_BIG5"
#define EPDK_CHARSET_NAME_GBK           "CHARSET_GBK"
#define EPDK_CHARSET_NAME_SJIS          "CHARSET_SJIS"
#define EPDK_CHARSET_NAME_EUC_JP        "CHARSET_EUC_JP"
#define EPDK_CHARSET_NAME_EUC_KR        "CHARSET_EUC_KR"
#define EPDK_CHARSET_NAME_KIO8_R        "CHARSET_KIO8_R"
#define EPDK_CHARSET_NAME_ISO_8859_1    "CHARSET_ISO_8859_1"
#define EPDK_CHARSET_NAME_ISO_8859_2    "CHARSET_ISO_8859_2"
#define EPDK_CHARSET_NAME_ISO_8859_3    "CHARSET_ISO_8859_3"
#define EPDK_CHARSET_NAME_ISO_8859_4    "CHARSET_ISO_8859_4"
#define EPDK_CHARSET_NAME_ISO_8859_5    "CHARSET_ISO_8859_5"
#define EPDK_CHARSET_NAME_ISO_8859_6    "CHARSET_ISO_8859_6"
#define EPDK_CHARSET_NAME_ISO_8859_7    "CHARSET_ISO_8859_7"
#define EPDK_CHARSET_NAME_ISO_8859_8    "CHARSET_ISO_8859_8"
#define EPDK_CHARSET_NAME_ISO_8859_9    "CHARSET_ISO_8859_9"
#define EPDK_CHARSET_NAME_ISO_8859_10   "CHARSET_ISO_8859_10"
#define EPDK_CHARSET_NAME_ISO_8859_11   "CHARSET_ISO_8859_11"
#define EPDK_CHARSET_NAME_ISO_8859_12   "CHARSET_ISO_8859_12"
#define EPDK_CHARSET_NAME_ISO_8859_13   "CHARSET_ISO_8859_13"
#define EPDK_CHARSET_NAME_ISO_8859_14   "CHARSET_ISO_8859_14"
#define EPDK_CHARSET_NAME_ISO_8859_15   "CHARSET_ISO_8859_15"
#define EPDK_CHARSET_NAME_ISO_8859_16   "CHARSET_ISO_8859_16"
#define EPDK_CHARSET_NAME_CP874         "CHARSET_CP874"
#define EPDK_CHARSET_NAME_CP1250        "CHARSET_CP1250"
#define EPDK_CHARSET_NAME_CP1251        "CHARSET_CP1251"
#define EPDK_CHARSET_NAME_CP1253        "CHARSET_CP1253"
#define EPDK_CHARSET_NAME_CP1255        "CHARSET_CP1255"
#define EPDK_CHARSET_NAME_CP1256        "CHARSET_CP1256"
#define EPDK_CHARSET_NAME_CP1257        "CHARSET_CP1257"
#define EPDK_CHARSET_NAME_CP1258        "CHARSET_CP1258"


#define EPDK_CHARSET_NAME_LEN_MAX       31


//************************************************
// 函数定义
/* system call table */
typedef struct
{
    __pSWI_t esCHS_Uni2Char       ;
    __pSWI_t esCHS_Char2Uni       ;
    __pSWI_t esCHS_GetChUpperTbl  ;
    __pSWI_t esCHS_GetChLowerTbl  ;

} SWIHandler_CHS_t;

#ifndef SIM_PC_WIN
#define SYSCALL_CHS(x,y) __swi(SWINO(SWINO_CHS, SWIHandler_CHS_t, y))x y
#else
#define SYSCALL_CHS(x,y) x y
#endif

/***************************************************************************************************
*Name        : esCHSuni2char
*Prototype   : __s32 esCHSuni2char( __s32 charset_enm, __u16 uni, __u8 *stream, __u32 len )
*Arguments   : charset_enm   input. the type of charset
*              uni           input. unicode value
*              stream        output. charset stream pointer
*              len           input. the length of charset stream
*Return      : The length of the specfic encoding byte stream converted from the corresponding unicode.
*              "<=0" means that converting failed.
*Description : Convert a unicode to the specfic charset.
*Other       :
***************************************************************************************************/
SYSCALL_CHS(__s32,  esCHS_Uni2Char      )(__s32 charset_enm, __u16 uni, __u8 *stream, __u32 len);


/***************************************************************************************************
*Name        : esCHSchar2uni
*Prototype   : __s32 esCHSchar2uni( __s32 charset_enm, const __u8 *stream, __u32 len, __u16 *uni )
*Arguments   : charset_enm   input. the type of charset
*              stream        input. charset stream pointer
*              len           input. the length of charset stream
*              uni           output. the buffer to store the unicode value.
*Return      : The length of the specfic encoding byte stream corresponding to the unicode. "<=0" means
*              that converting failed.
*Description : Convert the specfic charset to a unicode.
*Other       :
***************************************************************************************************/
SYSCALL_CHS(__s32,  esCHS_Char2Uni      )(__s32 charset_enm, const __u8 *stream, __u32 len, __u16 *uni);


/***************************************************************************************************
*Name        : esCHSget_charset2upper_table
*Prototype   : __u32 esCHSget_charset2upper_table( __s32 charset_enm, void *buf, __u32 len );
*Arguments   : charset_enm   input. the type of charset
*              buf           output. the buffer to store the specfic charset2upper table.
*              size          input. the size of the buffer.
*Return      : the valid size of the specfic charset2upper table. 0 means that error occured.
*Description : get the content of the specfic charset2upper table.
*Other       :
***************************************************************************************************/
SYSCALL_CHS(__u32,   esCHS_GetChUpperTbl )(__s32 charset_enm, void *buf, __u32 size);


/***************************************************************************************************
*Name        : esCHSget_charset2lower_table
*Prototype   : __u32 esCHSget_charset2lower_table( __s32 charset_enm, void *buf, __u32 len );
*Arguments   : charset_enm   input. the type of charset
*              buf           output. the buffer to store the specfic charset2lower table.
*              size          input. the size of the buffer.
*Return      : the valid size of the specfic charset2lower table. 0 means that error occured.
*Description : get the content of the specfic charset2lower table.
*Other       :
***************************************************************************************************/
SYSCALL_CHS(__u32,   esCHS_GetChLowerTbl )(__s32 charset_enm, void *buf, __u32 size);


/* iso-8859-1 */
/*西欧语言，包括阿尔巴尼亚语、巴斯克语、布列塔尼语、加泰罗尼亚语、丹麦语、荷兰语、法罗语、弗里西语
* 、加利西亚语、德语、格陵兰语、冰岛语、爱尔兰盖尔语、意大利语、拉丁语、卢森堡语、挪威语、葡萄牙语、
* 里托罗曼斯语、苏格兰盖尔语、西班牙语及瑞典语 */


/* iso-8859-2 */
/*中欧语言，此字符集主要支援以下文字：克罗地亚语、捷克语、匈牙利语、波兰语、斯洛伐克语、斯洛文尼亚语
* 、上索布语、下索布语。而阿尔巴尼亚语、英语、德语、拉丁语也可用此字符集显示。*/


/* iso-8859-3 */
/*南欧语言，它原先设计来表示土耳其语及马耳他语文字。但土耳其语已改用ISO/IEC 8859-9显示。现时只有世界
* 语及马耳他语仍使用此字符集。此字符集同时能支援以下文字：英语、德语、意大利语、拉丁语及葡萄牙语。*/


/* iso-8859-4 */
/*北欧语言，它设计来表示爱沙尼亚语、格陵兰语、拉脱维亚语、立陶宛语及萨米诸语言（Sámi）部分文字。此字
* 符集同时能支援以下文字：丹麦语、英语、芬兰语、德语、拉丁语、挪威语、斯洛文尼亚语及瑞典语。*/


/* iso-8859-5 */
/*此字集收录西里尔字母，供俄语、白俄罗斯语、保加利亚语、马其顿语、塞尔维亚语、乌克兰语使用*/


/* iso-8859-6 */
/*供现代阿拉伯语使用。它等同于阿拉伯标准ASMO-708。但因很多于阿拉伯语使用的字母都未能支援，故近来已逐
* 步被Unicode取代*/


/* iso-8859-7 */
/*供现代希腊语使用。它只有现代希腊语的单音字母（Monotonic Greek），没有收录古希腊语使用的多音字母
* （Polytonic Greek）。希腊的国家标准ELOT 928等同于此字集*/


/* iso-8859-8 */
/*供希伯来语使用。它只有希伯来语辅音字母，并没有元音符号。*/


/* iso-8859-9 */
/*表示土耳其语及库尔德语文字。这个字符集由ISO/IEC 8859-1演化而来。除土耳其语及库尔德语外，阿尔巴尼亚
* 语、巴斯克语、布列塔尼语、加泰罗尼亚语、丹麦语、荷兰语、英语、弗里斯语、加利西亚语、德语、格陵兰语
* 、爱尔兰盖尔语（新拼字法）、意大利语、拉丁语、卢森堡语、挪威语、葡萄牙语、里托罗曼斯语、苏格兰盖尔
* 语、瑞典语也可用此字符集显示。*/


/* iso-8859-10 */
/*为北日耳曼语支（斯堪的纳维亚）各语言而设的一个8位字符集。它设计来表示丹麦语、法罗语、格陵兰语、冰岛
*语、挪威语及瑞典语文字。此字符集同时能支援以下文字：英语、爱沙尼亚语、芬兰语、德语、爱尔兰盖尔语
* （新拼字法）、拉丁语、立陶宛语、斯洛文尼亚语及萨米诸语言部分文字*/


/* iso-8859-11 */
/*泰语。此字符集来自泰国工业标准 TIS-620。*/


/* iso-8859-13 */
/*为波罗的语族各语言而设的一个8位字符集。用来表示爱沙尼亚语、芬兰语、拉脱维亚语及立陶宛语文字。此字
* 符集同时能支援英语、波兰语及部分丹麦语、德语、斯洛文尼亚语及瑞典语文字。*/


/* iso-8859-14 */
/*用以表示凯尔特语族的文字，包括布列塔尼语、加利西亚语、爱尔兰盖尔语（新拼字法及旧拼字法）、曼岛语、
* 威尔士语等。除此之外，阿尔巴尼亚语、巴斯克语、加泰罗尼亚语、丹麦语、英语、德语、格陵兰语、意大利语
* 、拉丁语、卢森堡语、挪威语、葡萄牙语、里托罗曼斯语、西班牙语、瑞典语也可用此字符集显示。*/


/* iso-8859-15 */
/*加入在ISO/IEC 8859-1字符集缺少的若干法语字母，及芬兰语字母。同时，它亦把 ¤（通用货币符号）换成 �
* （欧元符号）。此字符集同时能支援以下文字：阿尔巴尼亚语、巴斯克语、布列塔尼语、加泰罗尼亚语、丹麦语
* 、荷兰语、英语、爱沙尼亚语、法罗语、芬兰语、法语、弗里斯语、加利西亚语、德语、格陵兰语、冰岛语、爱
* 尔兰盖尔语（新拼字法）、意大利语、拉丁语、卢森堡语、挪威语、葡萄牙语、里托罗曼斯语、苏格兰盖尔语、
* 西班牙语、瑞典语。*/


/* iso-8859-16 */
/*这个字符集设计来涵盖阿尔巴尼亚语、克罗地亚语、匈牙利语、意大利语、波兰语、罗马尼亚语及斯洛文尼亚语
* 等东南欧国家语言。除以上七种文字外，此字符集同时能支援以下文字：英语、芬兰语、法语、德语、爱尔兰盖
* 尔语（新拼字法）、拉丁语。*/




//************************************************
// 变量定义
extern SWIHandler_CHS_t SWIHandler_CHS;
//************************************************
// 函数定义

/***************************************************************************************************
*Name        : charset_init
*Prototype   : __s32 charset_init( __u32 mode, void *p_arg )
*Arguments   : mode       input. mode, whose value is always 0 at present
*              p_arg      input. a pointer to the parameter, whose value is always NULL at present
*Return      : 0    succeed in initialising.
*              !=0  fail in initialising.
*Description : initialise charset module
*Other       :
***************************************************************************************************/
extern __s32 esCHS_Init( __u32 mode, void *p_arg );
/***************************************************************************************************
*Name        : charset_exit
*Prototype   : void charset_exit( void )
*Arguments   : void
*Return      : void
*Description : exit charset module
*Other       :
***************************************************************************************************/
extern void esCHS_Exit( void );



#endif     //  ifndef _SYS_CHARSET_H_

