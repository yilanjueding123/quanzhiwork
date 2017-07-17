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
* File    : Esh_script_var.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#ifndef  _ESH_SCRIPT_VAR_H_
#define  _ESH_SCRIPT_VAR_H_

struct var
{
    char *value;
    char *name;
    struct var *next;
    char status;
};

/* var components */
#define	NONAME	((char *)NULL)
#define	NOVALUE	((char *)NULL)

#define	COPYV	1				/* flag to setval, suggesting copy */
#define	RONLY	01				/* variable is read-only */
#define	EXPORT	02				/* variable is to be exported */
#define	GETCELL	04				/* name & value space was got with getcell */

/* extrenal function */
extern  int eqname(const char *n1, const char *n2);
extern  const char *findeq(const char *cp);
extern  struct var *lookup(const char *n);
extern  void nameval(struct var *vp, const char *val, const char *name);
extern  void setval(struct var *vp, const char *val);
extern  void export(struct var *vp);
extern  void ronly(struct var *vp);
extern  int isassign(const char *s);
extern  int assign(const char *s, int cf);
extern  int checkname(const char *cp);
extern  void putvlist(void);
extern  char *findname(const char *vname);


#endif      /* _ESH_SCRIPT_VAR_H_ */
