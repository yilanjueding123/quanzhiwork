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
* File    : Esh_script_lexer.c
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#include  "doscript_private.h"

char *yylvalcp;
int nlseen;       //Number of line seen in one lex skan time
int LocalPeekc;   //local prestore Char
int Localprev;    //previous character read by readc()

/* static function */
static int  collect(int c, int c1);
static int  dual(int c);
static int  GetSourceCh(void);
static int  rlookup(char *n);

/**
 * lex scan
 **/
int yylex(int cf)
{
    int c, c1;
    int atstart;

    /* Prestored TOKEN*/
    c = peeksym;

    if (c > 0)
    {
        peeksym = 0;
        if (c == '\n')
            startl = 1;
        return c;
    }

    /* initial */
    nlseen = 0;
    atstart = startl;
    startl = 0;
    linep = line;

    /* MALAMO set end */
    line[LINELIM - 1] = '\0';

loop:
    while ((c = my_getc(0)) == ' ' || c == '\t')  	//Skip whitespace
    {
        continue;
    }
    switch (c)
    {
    case '#':	 //Comment, skip to next newline or End-of-string
        while ((c = my_getc(0)) != '\0' && c != '\n')
        {
            continue;
        }
        unget(c);
        goto loop;

    case 0:      //the end of source
        return c;

    case '$':
        *linep++ = c;
        c = my_getc(0);
        break;

    case '[':
        c = collect(c, ']');
        if (c != '\0')     //The use [] have error
            return c;
        goto pack;

    case '\'':
    case '"':
        c = collect(c, c);
        if (c != '\0')     //The use '' "" have error
            return c;
        goto pack;

    case '|':
    case '&':
    case ';':
        startl = 1;
        /* If more chars process them, else return NULL char */
        c1 = dual(c);
        if (c1 != '\0')
            return c1;   // is '||' or '&&' or ';;'
        return c;

    /* remove \r before \n in text mode */
    case '\r':
        goto loop;

    case '\n':
        nlseen++;
        linenumber++;
        startl = 1;

        /* multiline or cf control skan multi line cmd */
        if (multiline || cf & CONTIN)    //CONTIN: 01
        {
            if (cf & CONTIN)
                goto loop;
        }
        return c;

    case '(':
    case ')':
        startl = 1;
        return c;

    default:

        break;
    }

    unget(c);

    /* Get a word not include special symbol */
pack:
    while ((c = my_getc(0)) != '\0' && !Esh_any(c, "$ '\"\t;&()\r\n"))      // Special symbol
    {
        if (linep >= elinep)
        {
            Esh_Error("word too long");
        }
        else
            *linep++ = c;
    };

    unget(c);

    if (Esh_any(c, "\"'"))      //get " '
        goto loop;

    /* String End */
    *linep++ = '\0';

    /* get key word */
    if (atstart)
    {
        c = rlookup(line);
        if (c != 0)        // c != 0 Get a key word
        {
            startl = 1;
            return c;
        }
    }
    /* Get TCOM Parameter */
    yylvalcp = strsave(line, areanum);

    /* get normal word  */
    return WORD;
}
/*
 * Collect a string quotated by marks '' or ""
 */
static int collect(int c, int c1)
{
    char s[2];

    *linep++ = c;
    while ((c = my_getc(c1)) != c1)
    {
        if ( (c == 0) || (c == '\n') )
        {
            unget(c);
            s[0] = c1;
            s[1] = 0;
            Esh_Error("no closing at one line: ");
            Esh_prs(s);
            return 1;
        }

        *linep++ = c;
    }

    *linep++ = c;
    return 0;
}
/* "multiline commands" helper func
 *  see if next 2 chars form a multiline ('||' or '&&')
 *
 */
static int dual(int c)
{
    char s[3];
    char *cp = s;

    *cp++ = c;              /* c is the given "peek" char */
    *cp++ = my_getc(0);     /* get next char of input */
    *cp = '\0';             /* add EOS marker */

    c = rlookup(s);	        /* see if 2 chars form a shell multiline */
    if (c == 0)
        unget(*--cp);       /* String is not a shell multiline, put peek char back */

    return c;               /* String is multiline, return numeric multiline (restab) code */
}
/*
 *See if given string is a shell multiline (FOR, IF, etc)
 *return 0 get WORD
 *return other key words
 */
static int rlookup(char *n)
{
    struct res
    {
        char r_name[6];
        int r_val;
    };
    static const struct res restab[] =
    {
        { "for"  , FOR    },
        { "case" , CASE   },
        { "esac" , ESAC   },
        { "while", WHILE  },
        { "do"   , DO     },
        { "done" , DONE   },
        { "if"   , IF     },
        { "in"   , IN     },
        { "then" , THEN   },
        { "else" , ELSE   },
        { "elif" , ELIF   },
        { "until", UNTIL  },
        { "fi"   , FI     },
        { ";;"   , BREAK  },
        { "||"   , LOGOR  },
        { "&&"   , LOGAND },
        { "{"    , '{'    },
        { "}"    , '}'    },
        { "goto" , JUMP   },
        { "."    , DOT    },
    };

    const struct res *rp;

    for (rp = restab; rp->r_name[0]; rp++)
    {
        if (strcmp(rp->r_name, n) == 0)
        {
            return rp->r_val;	  //Return numeric code for multiline
        }
    }
    return 0;					  //Not a multiline
}

int my_getc(int ec)
{
    int c;

    if (linep > elinep)
    {
        while ((c = readc()) != '\n' && c) //skip the too long line
            continue;
        Esh_Error("input line too long");
        gflg = 1;                         //indicate line buffer overflow
        return c;
    }
    c = readc();
    if ((ec != '\''))                     // '' can't parse string in between ''
    {
        if (c == '\\')                    // '/' transfer meaning in script
        {
            c = readc();
            if (c == '\n' && ec != '\"')  // one line continue
                return my_getc(ec);
            c |= QUOTE;
        }
    }
    return c;
}

/**
 * read back a char
 **/
void unget(int c)
{
    LocalPeekc = c;
}

int readc(void)
{
    int c;
    c = LocalPeekc;
    if (c != '\0')
    {
        LocalPeekc = 0;
        return c;
    }
    c = GetSourceCh();
    if (c != '\0')         //is the end of source
    {
        return c;
    }
    /* NOTREACHED */
    return 0;
}
/*
 * get a source char
 */
int GetSourceCh(void)
{
    int c;
    c = *g_SourceCode;
    g_SourceCode++;     //move backwards a char
    return c;
}
