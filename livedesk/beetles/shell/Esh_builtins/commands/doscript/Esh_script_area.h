/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              apps Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : Shell
* File    : Esh_script_area.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#ifndef  _ESH_SCRIPT_AREA_H_
#define  _ESH_SCRIPT_AREA_H_

/* -------- area stuff -------- */
//#define AREASIZE  90000     //area size
#define AREASIZE  102400      /* area size :100K */
#define REGSIZE   sizeof(struct region)
#define GROWBY    (256)


#define FREE      (32767)   /* 7FFF */
#define BUSY      (0)
#define ALIGN     (sizeof(int)-1)


struct region
{
    struct region *next;
    int area;
};

/* external function */
#define	DELETE(obj) freecell((char *)obj)

extern  void initarea(void);
extern  void cleanarea(void);
extern  char *getcell(unsigned nbytes);
extern  void freecell(char *cp);
extern  void freearea(int a);
extern  void setarea(char *cp, int a);
extern  int  getarea(char *cp);
extern  void garbage(void);
extern  void *get_space(int n);
extern  char *strsave(const char *s, int a);

#endif  /* _ESH_SCRIPT_AREA_H_ */
