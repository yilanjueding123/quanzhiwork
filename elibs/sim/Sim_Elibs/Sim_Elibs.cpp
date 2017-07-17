// Sim_Elibs.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return TRUE;
}

extern "C"
{
#include "typedef.h"
#include "ePDK.h"

    int GetAudioFormat()
    {
        return 0;
    }

    __s32 eLIBs_GetNumSubItems (ES_DIR *pDir)
    {
        return 0;
    }
}
