// sim_shell.cpp : Defines the entry point for the DLL application.
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
    __exec_mgsec_t g_modinfo;
    extern const __exec_mgsec_t exfinfo;
    void *GetExecMgsInfo()
    {
        memcpy(&g_modinfo, &exfinfo, sizeof(__exec_mgsec_t));
        return &g_modinfo;
    }
    int      doupdate(struct op *t, const char **args)
    {
        return 1;
    }
}
