/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                             config file parser
*
* File    : config.c
* By      : sunny
* Version : V2.00                                                                         2010.7.5
*********************************************************************************************************
*/

#include    ".\scan_ini\scan_ini.h"
#include    "config.h"

HCONFIG OpenConfig(char *szFilePath, void *Param)
{
    return (HCONFIG)open_ini(szFilePath);
}

u32 CloseConfig(HCONFIG hcfg)
{
    return close_ini((HINI)hcfg);
}

u32 GetKeyValue(HCONFIG hcfg, s8 *szSection, s8 *szKey, u32 *pValue)
{
    HINI hIni;
    HLIN hLine;
    int  LineAttr;

    char tmpSecName[MAX_SECTION];
    char tmpKeyName[MAX_KEY];
    char tmpKeyValue[MAX_VALUE];

    hIni = (HINI)hcfg;

    /* rewind to file begin */
    line_rewind(hIni);

    while (1)
    {
        hLine = get_next_line(hIni);
        if (hLine == NULL)
        {
            break;
        }
        LineAttr = line2attr(hLine);
        if (LineAttr == LINE_ATTR_SET)
        {
            line2set(hLine, tmpSecName);
            if (!eLIBs_stricmp(tmpSecName, (const char *)szSection))
            {
                while (1)
                {
                    hLine = get_next_line(hIni);
                    if (hLine == NULL)
                    {
                        break;
                    }
                    LineAttr = line2attr(hLine);
                    if (LineAttr == LINE_ATTR_KEY)
                    {
                        line2key(hLine, tmpKeyName, tmpKeyValue);
                        if (!eLIBs_stricmp(tmpKeyName, (const char *)szKey))
                        {
                            (*pValue) = (__u32)eLIBs_strtol(tmpKeyValue, NULL, 10);
                            return 0;
                        }
                    }
                }
            }
        }
    }
    /* speical key name not find */
    return 1;
}

u32 GetKeyString(HCONFIG hcfg, s8 *szSection, s8 *szKey, s8 *String)
{
    HINI hIni;
    HLIN hLine;
    int  LineAttr;

    char tmpSecName[MAX_SECTION];
    char tmpKeyName[MAX_KEY];
    char tmpKeyValue[MAX_VALUE];

    hIni = (HINI)hcfg;

    /* rewind to file begin */
    line_rewind(hIni);

    while (1)
    {
        hLine = get_next_line(hIni);
        if (hLine == NULL)
        {
            break;
        }
        LineAttr = line2attr(hLine);
        if (LineAttr == LINE_ATTR_SET)
        {
            line2set(hLine, tmpSecName);
            if (!eLIBs_stricmp(tmpSecName, (const char *)szSection))
            {
                while (1)
                {
                    hLine = get_next_line(hIni);
                    if (hLine == NULL)
                    {
                        break;
                    }
                    LineAttr = line2attr(hLine);
                    if (LineAttr == LINE_ATTR_KEY)
                    {
                        line2key(hLine, tmpKeyName, tmpKeyValue);
                        if (!eLIBs_stricmp(tmpKeyName, (const char *)szKey))
                        {
                            eLIBs_strcpy((char *)String, tmpKeyValue);
                            return 0;
                        }
                    }
                }
            }
        }
    }
    /* speical key name not find */
    return 1;
}
