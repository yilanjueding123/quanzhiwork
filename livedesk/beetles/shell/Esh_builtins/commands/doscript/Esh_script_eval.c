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
* File    : Esh_script_eval.c
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include  "doscript_private.h"

const char *ifs = " \t";			/* field separators */

/* static function */
static int expand(const char *cp, struct wdblock **wbp, int f);
static char *dollar(const char *varname);
static char *singlequotes(const char *src);
static char *dublequotes(const char *src);
static char *unquote(char *as);
static char *squarebrackets(const char *src);

/*
 * ${}
 * `command`
 * blank interpretation
 * quoting
 */
char **eval(char **ap, int f)
{
    struct wdblock *wb;
    char **wp;

    wp = NULL;
    wb = NULL;

    for ( ; *ap; ap++)
    {

        /* expand */
        expand(*ap, &wb, f & ~DOKEY);      // not do key '='
    }
    wb = addword((char *) 0, wb);          // the end of words

    wp = getwords(wb);

    return wp;
}

static int expand(const char *cp, struct wdblock **wbp, int f)
{
    char *xp;
    char *varname;
    char *name;

    /* intail xp */
    xp = null;
    if (cp == NULL)
        return 0;

    if ( Esh_any(*cp, "$'\"[]") )
    {
        switch (*cp)
        {

        /* expand '$' */
        case '$' :
            cp++;   //skip $

            /* get a space for var name */
            varname = get_space( strlen(cp) + 3);
            setarea(varname, areanum);

            name = varname;
            /* check var name is legal ? */
            if ( *cp != '_' && (unsigned)(((*cp) | 0x20) - 'a') > 25 )
            {
                Esh_Error("Var name : ");
                Esh_prs(cp);
                Esh_prs("is illegimate\n");
                xp = null;
            }
            else
            {
                while ( *cp == '_' || Esh_isalnum(*cp) )
                {
                    *varname = *cp;
                    varname++;
                    cp++;
                }
            }
            *varname = '=';  // var name postfix
            varname++;
            *varname = '\0'; // the end of var name

            xp = dollar(name);
            xp = strsave(xp, areanum);
            break;

        /* expand '\''' */
        case '\'' :

            xp = singlequotes(cp);
            break;

        /* expand '""' */
        case '\"' :
            xp = dublequotes(cp);
            break;

        /* expand '[]' */
        case '[' :
            xp = squarebrackets(cp);
            break;
        }
    }
    else
    {
        xp = (char *)cp;
    }
    xp = strsave(xp, areanum);
    if (f & DOTRIM)
        unquote(xp);
    *wbp = addword(xp, *wbp);
    return 1;
}
/*
 * var quote
 * get var value
 */
static char *dollar(const char *varname)
{
    struct var *varpt;
    varpt = lookup(varname); //lookup var
    return varpt->value;
}
/*
 * Expand single quote
 */
static char *singlequotes(const char *src)
{
    int len;
    char *cp;
    char *tp;
    char *xp;
    cp = (char *)src;
    len = strlen(src);
    xp = cp + len;    //the end of string
    /* skan last ' */
    while ( *xp != '\'' )
    {
        xp--;
    }
    if ( xp <= src )
    {
        Esh_Error("no closing  ' in string: ");
        Esh_prs(src);
        Esh_prs("\n");
        return null;
    }
    tp = cp;
    src++;      //skip first '
    while ( src < xp )
    {
        *cp++ = *src++;
    }
    *cp = '\0';
    return tp;
}
/*
 * Expand duble quote
 */
static char *dublequotes(const char *src)
{
    char *cp;
    char *xp;
    char *value;
    char *varname;
    char *name;

    /* get a space for cp */
    xp = get_space( MAXARGLEN + 1);
    setarea(xp, areanum);
    memset(xp, '\0', (MAXARGLEN + 1));

    cp = xp;
    while ( *src != '\0' )
    {

SkanLoop:
        if ( *src != '"')
        {
            /* expand '$' in "" */
            if( *src == '$' )
            {
                /* get var name */
                src++;              //skip '$'

                /* get a space for var name */
                varname = get_space( strlen(src) + 3);
                setarea(varname, areanum);
                name = varname;

                /* check var name is legal ? */
                if ( *src != '_' && (unsigned)(((*src) | 0x20) - 'a') > 25 )
                {
                    Esh_Error("Var name : ");
                    Esh_prs(src);
                    Esh_prs("is illegimate\n");
                    xp = null;
                }
                else
                {
                    while ( *src == '_' || Esh_isalnum(*src) )
                    {
                        *varname = *src;
                        varname++;
                        src++;
                    }
                }
                *varname = '=';   //the postfix of var name
                varname++;
                *varname = '\0';  //the end of var name

                /* lookup var value */
                value = dollar(name);
                value = strsave(value, areanum);

                /* add value to cp */
                while ( *value )
                {
                    *cp = *value;
                    cp++;
                    value++;
                }
                /* expand others continue */
                goto SkanLoop;
            }
            *cp = *src;
            cp++;
        }
        src++;
    }
    cp++;
    *cp = '\0';
    return xp;
}
/*
 * Expand square brackets
 */
static char *squarebrackets(const char *src)
{
    int len;
    char *cp;
    char *tp;
    char *xp;
    cp = (char *)src;
    len = strlen(src);
    xp = cp + len;    //the end of string
    /* skan last ' */
    while ( *xp != ']' )
    {
        xp--;
    }
    if ( xp <= src )
    {
        Esh_Error("no closing  ' in string: ");
        Esh_prs(src);
        Esh_prs("\n");
        return null;
    }
    tp = cp;
    src++;      //skip first '
    while ( src < xp )
    {
        *cp++ = *src++;
    }
    *cp = '\0';
    return tp;
}

char *evalstr(char *cp, int f)
{
    struct wdblock *wb;

    wb = NULL;
    if (expand(cp, &wb, f))
    {
        if (wb == NULL || wb->w_nword == 0
                || (cp = wb->w_words[0]) == NULL
           )
        {
            cp = (char *)"";
        }
        DELETE(wb);
    }
    else
        cp = NULL;
    return cp;
}
static char *unquote(char *as)
{
    char *s;

    s = as;
    if ( *s != '\0')
        while (*s)
            *s++ &= ~QUOTE;
    return as;
}

