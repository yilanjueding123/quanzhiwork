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
* File    : Esh_script_pasrer.c
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include  "doscript_private.h"

struct wdblock *wdlist;      //word list
struct op *outtree;          //result from parser

int yynerrs;   //Parser error flag
int startl;    //start of new command
int peeksym;   //prestrore a yylex skan word
int linenumber;// record number of line in a script


/* static function */
static void       yyerror(const char *s);
static struct op *c_list(void);
static char     **wordlist(void);
static struct op *list(struct op *t1, struct op *t2);
static struct op *block(int type, struct op *t1, struct op *t2, char **wp);
static struct op *andor(void);
static struct op *command(int cf);
static void       musthave(int c, int cf);
static struct op *simple(void);
static struct op *nested(int type, int mark);
//static struct op *dowholefile(int type /*, int mark*/);
static struct op *dogroup(int onlydone);
static struct op *caselist(void);
static struct op *casepart(void);
static char **pattern(void);
static struct op *thenpart(void);
static struct op *elsepart(void);
static struct op *namelist(struct op *t);
static struct op *newtp(void);
static char      *tree(unsigned size);
static struct wdblock *newword(int nw);
static char **copyw(void);
static void word(char *cp);

/*
 * Error deal with
 */

/*
 * Is the end of a file or not
 */
int EndOfFile(void)
{
    /* end of file */
    if (g_SourceCode == g_SourceCodeTop)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
static void yyerror(const char *s)
{
    yynerrs = 1;
    multiline = 0;
    while (EndOfFile() && yylex(0) != '\n')  //Jump to Or line end or script file end
        continue;
    printf("line %d :", linenumber);
    printf("%s\n", s);
}
void zzerr(void)
{
    yyerror("syntax error");
}
/*
 * syntax parse: get syntax tree
 */
int yyparse(void)
{
    /* Indicate new line begin */
    startl = 1;
    peeksym = 0;
    yynerrs = 0;
    outtree = c_list();
    musthave('\n', 0);  //one command end
    return yynerrs;     //0/1
}
/*
 * get command list
 */
static struct op *c_list(void)
{
    struct op *t, *p;
    int c;

    t = andor();        // is  cmd1  ||(&&)  cmd2 ?

    if (t != NULL)
    {
        peeksym = yylex(0);

        while ( ((c = yylex(0)) == ';') || (multiline && (c == '\n')) )
        {
            p = andor();
            if (p == NULL)
            {
                return t;
            }
            peeksym = yylex(0);

            t = list(t, p);
        }

        peeksym = c;
    }
    return t;
}
/*
 * used in FOR
 */
static char **wordlist(void)
{
    int c;

    c = yylex(0);
    if (c != IN)
    {
        peeksym = c;
        return NULL;
    }
    startl = 0;
    while ((c = yylex(0)) == WORD)
        word(yylvalcp);
    word(NOWORD);
    peeksym = c;
    return copyw();
}

static struct op *list(struct op *t1, struct op *t2)
{

    if (t1 == NULL)
        return t2;
    if (t2 == NULL)
        return t1;

    return block(TLIST, t1, t2, NOWORDS);
}
/*
 * Link two command to form (c-list) format
 */
static struct op *block(int type, struct op *t1, struct op *t2, char **wp)
{
    struct op *t;

    t = newtp();
    t->op_type = type;
    t->left = t1;
    t->right = t2;
    t->op_words = wp;

    return t;
}
/*
 * Check command is: 'cmd1 || cmd2' or 'cmd1 or cmd2' ?
 */
static struct op *andor(void)
{
    struct op *t, *p;
    int c;

    t = command(0);

    if (t != NULL)
    {

        /* support more then one '||' '&&' in a command
         * eg: cmd1 ||(&&) cmd2 ||(&&) cmd3
         */
        while ((c = yylex(0)) == LOGAND || c == LOGOR)
        {
            p = command(CONTIN);    //get command behind '||' '&&'
            if (p == NULL)          // Error:behind '||' have no cmd
            {
                zzerr();
            }

            t = block(c == LOGAND ? TAND : TOR, t, p, NOWORDS);
        }

        peeksym = c;
    }
    return t;
}
/*
 * parse command
 */
static struct op *command(int cf)
{
    struct op *t;
    int c;

    if (multiline)
        cf |= CONTIN;

    c = yylex(cf);

    switch (c)
    {

    /* TCOM */
    default:
        peeksym = c;
        t = simple();
        if ( t == NULL )
        {
            return NULL;
        }
        break;

    case '(':
        t = nested(TPAREN, ')');
        break;

    case '{':
        t = nested(TBRACE, '}');
        break;

    case FOR:
        t = newtp();
        t->op_type = TFOR;
        musthave(WORD, 0);
        startl = 1;
        t->str = yylvalcp;     //Loop condition for 'for'
        multiline++;
        t->op_words = wordlist();
        c = yylex(0);
        if (c != '\n' && c != ';')
            peeksym = c;
        t->left = dogroup(0);
        multiline--;
        break;

    case WHILE:
    case UNTIL:
        multiline++;
        t = newtp();
        t->op_type = (c == WHILE ? TWHILE : TUNTIL);
        t->left = c_list();    //loop control condition
        t->right = dogroup(1); //loop commands
        multiline--;
        break;

    case CASE:
        t = newtp();
        t->op_type = TCASE;
        musthave(WORD, 0);
        t->str = yylvalcp;
        startl++;
        multiline++;
        musthave(IN, CONTIN);
        startl++;

        t->left = caselist();

        musthave(ESAC, 0);    //'esac' indicate case end
        multiline--;
        break;

    case IF:
        multiline++;
        t = newtp();
        t->op_type = TIF;
        t->left = c_list();    //decide condition for 'if'
        t->right = thenpart(); //execute command after 'if'
        musthave(FI, 0);       //indicate 'if' end
        multiline--;
        break;

    /* suppert script file in script file */
    case DOT:
        t = newtp();
        t->op_type = TDOT;

        musthave(WORD, 0);              //gets name of file

        word(yylvalcp);                //add word to wdlist
        word(NOWORD);                  //terminate  wdlist
        t->op_words = copyw();         //dup wdlist
        break;

    }
    t = namelist(t);

    return t;
}

static void musthave(int c, int cf)
{
    peeksym = yylex(cf);

    if (peeksym != c)
    {
        zzerr();
    }
    peeksym = 0;
}

static struct op *simple(void)
{
    struct op *t;

    t = NULL;
    for (;;)
    {
        switch (peeksym = yylex(0))
        {
        case WORD:          //get a build_in_cmd
            if (t == NULL)
            {
                t = newtp();
                t->op_type = TCOM;
            }
            peeksym = 0;
            word(yylvalcp);  //Add word to wlist
            break;

        default:
            return t;
        }
    }
}

/*
 * get nested command: (cmd1;cmd2;cmd3;) {cmd1;cmd2;cmd3;}
 */
static struct op *nested(int type, int mark)
{
    struct op *t;

    multiline++;
    t = c_list();
    musthave(mark, 0);
    multiline--;
    return block(type, t, NOBLOCK, NOWORDS);
}
///*
// * Execute a script file
// */
//static struct op *dowholefile(int type /*, int mark*/)
//{
//	struct op *t;
//
//	multiline++;
//	t = c_list();
//	multiline--;
//	t = block(type, t, NOBLOCK, NOWORDS);
//	return t;
//}
/*
 * Parser a group cmd
 */
static struct op *dogroup(int onlydone)
{
    int c;
    struct op *mylist;
    c = yylex(CONTIN);
    if (c == DONE && onlydone)   //empty loop for just "while" but except "for"
        return NULL;
    if (c != DO)                 //DO indicate loop begin
        zzerr();
    mylist = c_list();
    musthave(DONE, 0);           //DONE indicate loop end
    return mylist;
}

static struct op *caselist(void)
{
    struct op *t;

    t = NULL;

    /* Get every case pattern until the end indentifer 'esac'*/
    while ((peeksym = yylex(CONTIN)) != ESAC)
    {
        t = list(t, casepart());
    }
    return t;
}

static struct op *casepart(void)
{
    struct op *t;

    t = newtp();
    t->op_type = TPAT;       // pattern in case
    t->op_words = pattern(); // get case parameter
    musthave(')', 0);        //')' indicate case command begin
    t->left = c_list();
    peeksym = yylex(CONTIN);
    if (peeksym != ESAC)
        musthave(BREAK, CONTIN);

    return t;
}

static char **pattern(void)
{
    int c, cf;

    cf = CONTIN;

    /* more then one match mode
     * eg: Y|yes|YES)
     */
    do
    {
        musthave(WORD, cf);
        word(yylvalcp);
        cf = 0;
        c = yylex(0);
    }
    while (c == '|');
    peeksym = c;
    word(NOWORD);

    return copyw();
}

static struct op *thenpart(void)
{
    int c;
    struct op *t;

    c = yylex(0);

    /* must 'then' behind 'if' or 'elif' */
    if (c != THEN)
    {
        peeksym = c;
        return NULL;
    }
    t = newtp();

    t->left = c_list();     //execute command for if

    if (t->left == NULL)
        zzerr();
    t->right = elsepart(); //else or elif part
    return t;
}

static struct op *elsepart(void)
{
    int c;
    struct op *t;

    switch (c = yylex(0))
    {
    case ELSE:             //else
        t = c_list();      //commands for else
        if (t == NULL)
            zzerr();
        return t;

    case ELIF:           //elif
        t = newtp();
        t->op_type = TELIF;
        t->left = c_list();   //decide condition for elif
        t->right = thenpart();//command for elif
        return t;

    default:           //just if command, no else and elif
        peeksym = c;
        return NULL;
    }
}

static struct op *newtp(void)
{
    struct op *t;

    t = (struct op *) tree(sizeof(*t));
    memset(t, 0, sizeof(*t));
    return t;
}

static struct op *namelist(struct op *t)
{

    if (t->op_type != TCOM)
    {
        if (t->op_type != TPAREN)
        {
            t = block(TPAREN, t, NOBLOCK, NOWORDS);
        }
        return t;
    }

    word(NOWORD);
    t->op_words = copyw();   // Add node parameter

    return t;
}
/*
 * Get a new tree
 */
static char *tree(unsigned size)
{
    char *t;

    t = getcell(size);
    if (t == NULL)
    {
        Esh_prs("command line too complicated\n");
        /* NOTREACHED */
    }
    return t;
}

/* -------- word.c -------- */

static struct wdblock *newword(int nw)
{
    struct wdblock *wb;

    wb = get_space(sizeof(*wb) + nw * sizeof(char *));
    wb->w_bsize = nw;
    wb->w_nword = 0;
    return wb;
}

struct wdblock *addword(char *wd, struct wdblock *wb)
{
    struct wdblock *wb2;
    int nw;

    /* Get a new word */
    if (wb == NULL)
    {
        /* NSTART(16) :default number of words to allow for initially */
        wb = newword(NSTART);
    }
    nw = wb->w_nword;
    if (nw >= wb->w_bsize)     //the number of words great then w_bsize
    {
        wb2 = newword(nw * 2); //w_bsize : nw * 2
        memcpy((char *) wb2->w_words, (char *) wb->w_words,
               nw * sizeof(char *));
        wb2->w_nword = nw;     //w_nword : nw
        DELETE(wb);            //free wb
        wb = wb2;
    }
    wb->w_words[wb->w_nword++] = wd;;
    return wb;
}

char **getwords(struct wdblock *wb)
{
    char **wd;
    int nb;

    if (wb == NULL)
    {
        return NULL;
    }
    if (wb->w_nword == 0)
    {
        DELETE(wb);
        return NULL;
    }
    nb = sizeof(*wd) * wb->w_nword;
    wd = get_space(nb);
    memcpy(wd, wb->w_words, nb);

    DELETE(wb);			// free wb
    return wd;
}

static char **copyw(void)
{
    char **wd;

    wd = getwords(wdlist);
    wdlist = NULL;
    return wd;
}

static void word(char *cp)
{
    wdlist = addword(cp, wdlist);
}
