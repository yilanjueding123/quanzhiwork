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
* File    : Esh_script_area.c
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include  "doscript_private.h"

struct region *areabot;  /* bottom of area */
struct region *areatop;  /* top of area */
struct region *areanxt;  /* starting point of scan */
char  *brktop;           /* top of bank */
char  *brkaddr;          /* address of allocation */
char  *intialaddr;       /* the intial address of malloc */
int areanum;             /* current allocation area */

/*
 * All memory between (char *)areabot and (char *)(areatop+1) is
 * exclusively administered by the area management routines.
 * It is assumed that sbrk() and brk() manipulate the high end.
 */

char *sbrk(int X)
{

    char *__q = (char *) - 1;
    if (brkaddr + (int)(X) < brktop)
    {
        __q = brkaddr;
        brkaddr += (int)(X);
    }
    return __q;
}

/*
 * Initail area
 */
void initarea(void)
{
    intialaddr = (char *)malloc(AREASIZE);  // Allocate memory for dispatcher
    brkaddr = intialaddr;
    brktop = (char *)(brkaddr) + AREASIZE - 1;

    /* intÀàÐÍ ¶ÔÆë */
    while ((long) sbrk(0) & ALIGN)
        sbrk(1);
    areabot = (struct region *)sbrk(REGSIZE);

    areabot->next = areabot;
    areabot->area = BUSY;
    areatop = areabot;
    areanxt = areabot;
    return;
}
void cleanarea(void)
{
    free(intialaddr);
    //    intialaddr = (char *)-1;
    //    brkaddr = (char *)-1;
    //    brktop = (char *)-1;
    //    areabot = (struct region *) -1;
    //    areabot->next = (struct region *) -1;;
    //    areatop = areabot;
    //	  areanxt = areabot;
}

char *getcell(unsigned nbytes)
{
    int nregio;
    struct region *p, *q;
    int i;

    if (nbytes == 0)
    {
        Esh_prs("getcell(0)");
        return NULL;
    }
    /* silly and defeats the algorithm */
    /*
     * round upwards and add administration area
     */
    nregio = (nbytes + (REGSIZE - 1)) / REGSIZE + 1;  //while number + 1 of region

    p = areanxt;
    for (;;)
    {
        if (p->area > areanum)
        {
            /*
             * merge free cells
             */
            while ((q = p->next)->area > areanum && q != areanxt)
                p->next = q->next;
            /*
             * exit loop if cell big enough
             */
            if (q >= p + nregio)
                goto found;
        }
        p = p->next;
        if (p == areanxt)
            break;
    }
    i = nregio >= GROWBY ? nregio : GROWBY;

    p = (struct region *) sbrk(i * REGSIZE);
    if (p == (struct region *) - 1)
        return NULL;;
    p--;
    if (p != areatop)
    {
        Esh_prs("not contig");
    }
    q = p + i;
    p->next = q;
    p->area = FREE;
    q->next = areabot;
    q->area = BUSY;
    areatop = q;
found:
    /*
     * we found a FREE area big enough, pointed to by 'p', and up to 'q'
     */
    areanxt = p + nregio;
    if (areanxt < q)
    {
        /*
         * split into requested area and rest
         */
        if (areanxt + 1 > q)
        {
            Esh_prs("OOM");
            //			abort();			/* insufficient space left for admin */
        }
        areanxt->next = q;
        areanxt->area = FREE;
        p->next = areanxt;
    }
    p->area = areanum;
    return (char *) (p + 1);
}

void freecell(char *cp)
{
    struct region *p;

    p = (struct region *) cp;
    if (p != NULL)
    {
        p--;
        if (p < areanxt)
            areanxt = p;
        p->area = FREE;
    }
}

void freearea(int a)
{
    struct region *p, *top;

    top = areatop;
    for (p = areabot; p != top; p = p->next)
        if (p->area >= a)
            p->area = FREE;
}

void setarea(char *cp, int a)
{
    struct region *p;

    p = (struct region *) cp;
    if (p != NULL)
        (p - 1)->area = a;
}

int getarea(char *cp)
{
    return ((struct region *) cp - 1)->area;
}

void garbage(void)
{
    struct region *p, *q, *top;

    top = areatop;
    for (p = areabot; p != top; p = p->next)
    {
        if (p->area > areanum)
        {
            while ((q = p->next)->area > areanum)
                p->next = q->next;
            areanxt = p;
        }
    }
}

void *get_space(int n)
{
    char *cp;

    cp = getcell(n);
    if (cp == NULL)
        Esh_Error("out of string space");
    return cp;
}

char *strsave(const char *s, int a)
{
    char *cp;

    cp = get_space(strlen(s) + 1);
    if (cp == NULL)
    {
        return (char *)"";
    }
    setarea(cp, a);
    strcpy(cp, s);
    return cp;
}
