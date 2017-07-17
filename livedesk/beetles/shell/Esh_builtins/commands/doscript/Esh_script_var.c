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
* File    : Esh_script_var.c
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include  "doscript_private.h"

struct var *vlist;		/* dictionary : var list */

int eqname(const char *n1, const char *n2)
{
    for (; *n1 != '=' && *n1 != '\0'; n1++)
        if (*n2++ != *n1)
            return 0;
    return *n2 == '\0' || *n2 == '=';
}
/*
 * find '=' in a string :var=value
 */
const char *findeq(const char *cp)
{
    while (*cp != '\0' && *cp != '=')
        cp++;
    return cp;
}
/*
 * Find the given name in the dictionary
 * and return its value.  If the name was
 * not previously there, enter it now and
 * return a null value.
 */
struct var *lookup(const char *n)
{
    static struct var dummy;

    struct var *vp;
    const char *cp;
    char *xp;


    /* var 'n' in vlist ? */
    for (vp = vlist; vp; vp = vp->next)
    {
        if (eqname(vp->name, n))
        {
            return vp;
        }
    }

    /* A new var, Add it */
    cp = findeq(n);
    vp = get_space(sizeof(*vp));
    if (vp == 0 || (vp->name = get_space((int) (cp - n) + 2)) == NULL)
    {
        dummy.name = dummy.value = (char *)"";
        return &dummy;
    }

    xp = vp->name;
    while ((*xp = *n++) != '\0' && *xp != '=')
        xp++;
    *xp++ = '=';
    *xp = '\0';;
    setarea((char *) vp, 0);
    setarea((char *) vp->name, 0);
    vp->value = null;
    vp->next = vlist;
    vp->status = GETCELL;
    vlist = vp;
    return vp;
}

/*
 * if name is not NULL, it must be
 * a prefix of the space `val',
 * and end with `='.
 * this is all so that exporting
 * values is reasonably painless.
 */
void nameval(struct var *vp, const char *val, const char *name)
{
    const char *cp;
    char *xp;
    int fl;

    if (vp->status & RONLY)
    {
        xp = vp->name;
        while (*xp && *xp != '=')
        {
            Esh_prc(*xp++);
        }
        Esh_Error(" is read-only");
        return;
    }
    fl = 0;
    if (name == NULL)
    {
        xp = get_space(strlen(vp->name) + strlen(val) + 2);
        if (xp == NULL)
            return;

        /* make string: name=value */
        setarea(xp, 0);
        name = xp;
        cp = vp->name;
        while ((*xp = *cp++) != '\0' && *xp != '=')
            xp++;
        *xp++ = '=';
        strcpy(xp, val);
        val = xp;
        fl = GETCELL;
    }
    if (vp->status & GETCELL)
        freecell(vp->name);		   /* form new string `name=value' */

    vp->name = (char *)name;
    vp->value = (char *)val;
    vp->status |= fl;
}

/*
 * give variable at `vp' the value `val'.
 */
void setval(struct var *vp, const char *val)
{
    nameval(vp, val, NULL);
}

void export(struct var *vp)
{
    vp->status |= EXPORT;
}

void ronly(struct var *vp)
{
    if (Esh_isalpha(vp->name[0]) || vp->name[0] == '_')	/* not an internal symbol */
        vp->status |= RONLY;
}
/*
 * evaluate expression ?
 */
int isassign(const char *s)
{
    unsigned char c;

    c = *s;
    /* no Esh_isalpha() - we shouldn't use locale */
    /* c | 0x20 - lowercase (Latin) letters */
    if (c != '_' && (unsigned)((c | 0x20) - 'a') > 25)
        /* not letter */
        return 0;

    while (1)
    {
        c = *++s;
        if (c == '=')
            return 1;
        if (c == '\0')
            return 0;
        if (c != '_'
                && (unsigned)(c - '0') > 9         /* not number */
                && (unsigned)((c | 0x20) - 'a') > 25 /* not letter */
           )
        {
            return 0;
        }
    }
}

int assign(const char *s, int cf)
{
    const char *cp;
    struct var *vp;

    if (!Esh_isalpha(*s) && *s != '_')
        return 0;
    for (cp = s; *cp != '='; cp++)
        if (*cp == '\0' || (!Esh_isalnum(*cp) && *cp != '_'))
            return 0;
    vp = lookup(s);
    nameval(vp, ++cp, cf == COPYV ? NULL : s);
    if (cf != COPYV)
        vp->status &= ~GETCELL;
    return 1;
}

int checkname(const char *cp)
{

    if (!Esh_isalpha(*cp++) && *(cp - 1) != '_')
        return 0;
    while (*cp)
        if (!Esh_isalnum(*cp++) && *(cp - 1) != '_')
            return 0;
    return 1;
}
void putvlist(void)
{
    struct var *vp;
    char *vname;
    printf("\nThe var list : \n");
    for (vp = vlist; vp; vp = vp->next)
    {
        if ((Esh_isalpha(*vp->name) || *vp->name == '_'))
        {
            if (vp->status & EXPORT)
                printf("export  ");
            if (vp->status & RONLY)
                printf("readonly  ");

            /* get var name expect '=' */
            vname = findname(vp->name);
            printf("\nname : %s  value : %s \n", vname, vp->value);
        }
    }
}
/*
 * Find var name except '='
 */
char *findname(const char *vname)
{
    char *varname;
    char *name;

    /* get a space for var name */
    varname = get_space( strlen(vname) );
    setarea(varname, areanum);
    name = varname;

    /* get var namr expect '=' */
    while ( ( *vname != '=' && (Esh_isalnum(*vname) || *vname == '_') ) )
    {
        *varname = *vname;
        varname++;
        vname++;
    }
    *varname = '\0';  // the end of varname;
    return name;
}
