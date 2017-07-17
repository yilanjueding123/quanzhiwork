#ifndef __SIM_COMM_H__
#define __SIM_COMM_H__
#define AW_SIM_API __declspec(dllimport)
#ifdef __cplusplus
#define  WM_UPDATE_SCREEN WM_USER+10

typedef struct DevRegStruct
{
    char _szClassName[24];
    char _szDevNam[24];
    void *_pDevOp;
    UINT _nState; // 0 out, 1 int
} _devRegInfo;

extern "C"
{
#endif
extern AW_SIM_API void PCTrace(const char *szMsg, ...);
extern AW_SIM_API void TraceMessageInLevel(int nLevel, const char *szMsg, ...);
#ifdef __cplusplus
}
#endif


#endif