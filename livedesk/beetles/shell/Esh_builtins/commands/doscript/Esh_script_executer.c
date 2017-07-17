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
* File    : Esh_script_executer.c
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include  "doscript_private.h"

/* static function */
static struct op *findcase(struct op *t, const char *w);

/*
 * execute outtree form parser
 *
 * return
 * 1: successful
 * 0: failed
 *
 */

/* static function */
static int forkexec(struct op *t, int *pin, int *pout, int no_fork, char **wp);


int execute(struct op *t, int *pin, int *pout, int no_fork)
{
    struct op *t1;
    volatile int rv, a;
    const char *cp;
    char **wp, **wp2;
    struct var *vp;
    //	struct op *outtree_save;

    /* no any command return succeeded */
    if (t == NULL)
    {
        return 0;
    }

    rv = 0;
    a = areanum++;
    wp2 = t->op_words;

    /* expand t->op_words */
    wp = (wp2 != NULL)
         ? eval(wp2, t->op_type == TCOM ? DOALL : DOALL & ~DOKEY)
         : NULL;
    switch (t->op_type)
    {

    case TPAREN:   //( ; ; ;)
        rv = execute(t->left, pin, pout, /* no_fork: */ 0);
        break;

    case TCOM:
        /* skip out commands after break in loop */
        if (IsBreakOut)
            goto broken;

        rv = forkexec(t, pin, pout, no_fork, wp);
        break;

    case TLIST:
        (void) execute(t->left, pin, pout, /* no_fork: */ 0);
        rv = execute(t->right, pin, pout, /* no_fork: */ 0);
        break;

    case TOR:
    case TAND:
        rv = execute(t->left, pin, pout, /* no_fork: */ 0);
        t1 = t->right;
        if (t1 != NULL && (rv == 0) == (t->op_type == TAND))  // 'AND' 'OR' command
            rv = execute(t1, pin, pout, /* no_fork: */ 0);
        break;

    case TFOR:
        /* intial wp */
        vp = lookup(t->str);
        for (t1 = t->left; *wp != NULL;)
        {
            /* check IsBreakOut, control loop skip out */
            if (IsBreakOut)
                goto broken;
            setval(vp, *wp++);
            rv = execute(t1, pin, pout, /* no_fork: */ 0);
        }
        break;

    case TWHILE:
    case TUNTIL:
        t1 = t->left;
        while ((execute(t1, pin, pout, /* no_fork: */ 0) == 0) == (t->op_type == TWHILE))
        {
            /* check IsBreakOut, control loop skip out */
            if (IsBreakOut)
                goto broken;
            rv = execute(t->right, pin, pout, /* no_fork: */ 0);
        }
        break;

    case TIF:
    case TELIF:
        if (t->right != NULL)
        {
            rv = !execute(t->left, pin, pout, /* no_fork: */ 0) ?
                 execute(t->right->left, pin, pout, /* no_fork: */ 0) :
                 execute(t->right->right, pin, pout, /* no_fork: */ 0);
        }
        break;

    case TCASE:
        cp = evalstr(t->str, DOSUB | DOTRIM);
        if (cp == NULL)
            cp = "";

        t1 = findcase(t->left, cp);
        if (t1 != NULL)
        {
            rv = execute(t1, pin, pout, /* no_fork: */ 0);
        }
        break;

    case TBRACE:   //{ ; ; ; }
        if (rv >= 0)
        {
            t1 = t->left;
            if (t1)
            {
                rv = execute(t1, pin, pout, /* no_fork: */ 0);
            }
        }
        break;

    };

broken:
    // Restoring op_words is most likely not needed now: see comment in forkexec()
    // (also take a look at exec builtin (doexec) - it touches t->op_words)

    t->op_words = wp2;
    freearea(areanum);
    areanum = a;

    return rv;
}


/* -------- gmatch.c -------- */
/*
 * int gmatch(string, pattern)
 * char *string, *pattern;
 *
 * Match a pattern as in sh(1).
 */
static int gmatch(const char *s, const char *p)
{
    int sc, pc;

    if (s == NULL || p == NULL)
        return 0;

    while ((pc = *p++ & CMASK) != '\0')
    {
        sc = *s++ & QMASK;
        switch (pc)
        {

        case '*':
            s--;
            do
            {
                if (*p == '\0' || gmatch(s, p))
                    return 1;
            }
            while (*s++ != '\0');
            return 0;

        default:
            if (sc != (pc & ~QUOTE))
                return 0;
        }
    }
    return *s == '\0';
}
static struct op **find1case(struct op *t, const char *w)
{
    struct op *t1;
    struct op **tp;
    char **wp;
    char *cp;

    if (t == NULL)
    {
        return NULL;
    }


    if (t->op_type == TLIST)
    {
        tp = find1case(t->left, w);
        if (tp != NULL)
        {
            return tp;
        }
        t1 = t->right;			/* TPAT */
    }
    else
        t1 = t;

    for (wp = t1->op_words; *wp;)
    {
        cp = evalstr(*wp++, DOSUB);
        if (cp && gmatch(w, cp))
        {

            return &t1->left;
        }
    }
    return NULL;
}

static struct op *findcase(struct op *t, const char *w)
{
    struct op **tp;

    tp = find1case(t, w);
    return tp != NULL ? *tp : NULL;
}
/*
 * execute a TCOM type command
 */
static int forkexec(struct op *t, int *pin, int *pout, int no_fork, char **wp)
{
    builtin_func_ptr bltin = NULL;
    const char *cp;
    int DoResult;

    DoResult = 0;   // defult failed, is not a command
    cp = (const char *)(*wp);
    if (t->op_type == TCOM)
    {
        bltin = Esh_inbuilt(cp);
    }
    /* Run builtin */
    if (bltin)
    {
        DoResult = bltin(t, (const char **)wp);

        /* not a build in command */
    }
    else
    {
        zzerr();
        printf("\"%s\"  not a executed command\n", cp);
    }

    /* return */
    return DoResult;
}
