/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*
*
* File    : Esh_builtin.h
* By      : sunny
* Version : V1.00                                                                       2009.3.2
*********************************************************************************************************
*/
#ifndef     _ESH_BUILTIN_H_
#define     _ESH_BUILTIN_H_

#include  "Esh_script_parser.h"

/* Build in commands prototype */
typedef int (*builtin_func_ptr)(struct op *, const char **);

struct builtincmd
{
    const char *name;
    builtin_func_ptr builtinfunc;
};

/* seach builtin command function prototype */
extern  builtin_func_ptr Esh_inbuilt(const char *s);

/* builtin commands */
extern      int     docat(struct op *t, const char **args);
extern      int     docd(struct op *t, const char **args);
extern      int     docopy(struct op *t, const char **args);
extern      int     dodel(struct op *t, const char **args);
extern      int     dodir(struct op *t , const char **args);
extern      int     docreate(struct op *t, const char **args);
extern      int     dofs(struct op *t , const char **args);
extern      int     dohelp(struct op *t , const char **args);
extern      int     dohistory(struct op *t , const char **args);
extern      int     dokeysim(struct op *t , const char **args);
extern      int     doinsmod(struct op *t , const char **args);
extern      int     dopoweroff(struct op *t , const char **args);
extern      int     domount(struct op *t , const char **args);
extern      int     domkdir(struct op *t, const char **args);
extern      int     doreset(struct op *t , const char **args);
extern      int     dormdir(struct op *t, const char **args);
extern      int     dostartx(struct op *t , const char **args);
extern      int     dosysinfo(struct op *t , const char **args);
extern      int     dorun(struct op *t, const char **args);

extern      int     dosetpath(struct op *t, const char **args);

extern      int     doclear(struct op *t , const char **args);

/* script execute */
extern      int     doscript(struct op *t, const char **args);

/* script support */
extern     int      dotest(struct op *t, const char **args);
extern     int      doecho(struct op *t, const char **args);
extern     int      doexpr(struct op *t , const char **args);
extern     int      dovar(struct op *t , const char **args);
extern     int      doset(struct op *t , const char **args);
extern     int      doputv(struct op *t , const char **args);
extern     int      dobreak(struct op *t , const char **args);
//extern     int doexport(struct op *t , const char **args);
extern     int      doreadonly(struct op *t , const char **args);

extern     int      dofastclear(struct op *t , const char **args);
extern     int      dorandom(struct op *t , const char **args);
extern     int      dogetpartsize(struct op *t , const char **args);

extern     int      dosendkey(struct op *t , const char **args);
extern     int      dowait(struct op *t , const char **args);
extern     int		doquit(struct op *t , const char **args);
extern     int      doassert(struct op *t, const char **args);
extern     int      doxcopy(struct op *t, const char **args);

extern     int      doformatcheck(struct op *t , const char **args);
extern     int      doformat(struct op *t , const char **args);
extern     int      doshowdsize(struct op *t, const char **args);
extern     int      doshowfsize(struct op *t, const char **args);
extern     int      doconfigfs(struct op *t , const char **args);

extern     int      startcpustat(struct op *t , const char **args);
extern     int      stopcpustat(struct op *t , const char **args);
extern     int      reportcpustat(struct op *t , const char **args);
extern     int      dode(struct op *t , const char **args);
extern     int      domeminfo(struct op *t , const char **args);

extern     int      doupdate(struct op *t, const char **args);

extern     int      doregwr(struct op *t, const char **args);
extern     int      doregrd(struct op *t, const char **args);
extern     int doregdump(struct op *t, const char **args);
#endif   /* _ESH_BUILTIN_H_ */
