/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                              Read one command
*
* File    : Esh_read_command.c
* By      : sunny
* Version : V1.00                                                           2009.3.2
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

cmd_history Esh_CMD_Table[Esh_MAX_HISTORY_LEN];    /* commands record table */
cmd_history *Esh_Current_CMD;                      /* record location of current command */

/* 插入命令节点 */
static void __Esh_Insert_cmd(char *elem)
{
    Esh_strcpy(Esh_Current_CMD->cmd, elem);
    Esh_Current_CMD = Esh_Current_CMD->next;
}

/* 初始化命令记录 */
static void Esh_Inithistory(void)
{
    __u32 i;
    Esh_CMD_Table->prior = (cmd_history *)(Esh_CMD_Table + Esh_MAX_HISTORY_LEN - 1);
    (Esh_CMD_Table + Esh_MAX_HISTORY_LEN - 1)->next = (cmd_history *)(Esh_CMD_Table);
    for(i = 0; i < Esh_MAX_HISTORY_LEN; i++)
    {
        if ( i != (Esh_MAX_HISTORY_LEN - 1) )
        {
            (Esh_CMD_Table + i)->next = (cmd_history *)(Esh_CMD_Table + i + 1);
        }
        if ( i != 0 )
        {
            (Esh_CMD_Table + i)->prior = (cmd_history *)(Esh_CMD_Table + i - 1);
        }
    }
}
/*
*****************************************************************************
*                            __Esh_getcmd()
*
*  Description:  Get the command input;
*
*
*  Parameters:   cmd_buf--Input command string
*
*  Return value:None
*****************************************************************************
*/
static void __Esh_getcmd(char *cmd_buf, __u32 len)
{
    char curpos = 0;	/* current position - index __s32o cmd_buf */
    char c;
    char cps[ESH_MAX_CMD_LEN];
    __u32  valid_length = 0;
    __u32  cps_length = 0;
    __s32  cmd_echo = 1;
    __u32  redir_flg = 0;
    cmd_history *pt;

    /* from the last command begin */
    pt = Esh_Current_CMD;

    /* Clear out the buffer */
    Esh_memset(cmd_buf, 0, ESH_MAX_CMD_LEN);

    while( 1 )
    {
        c = __getc();
        switch (c)
        {
        case 0x08:
        case 0x06:
        case 0x07:
        //case 0x7E:
        case 0x7F:	/* backspace or delete ,char u*/
            /* we're not at the beginning of the line */
            if (curpos)
            {
                curpos--;
                __putc(0x08);	/* go backwards */
                __putc(' ');	/* overwrite the char */
                __putc(0x08);	/* go back again */
            }
            cmd_buf[curpos] = '\0';
            break;

        /*check direction key '↑' input*/
        case '~':
            redir_flg = 1;
            break;
        case 'U':
            /* clear last time command */
            if( redir_flg )
            {
                curpos = Esh_strlen(cmd_buf);
                while( curpos )
                {
                    __putc(0x08);	/* go backwards */
                    __putc(' ');	/* overwrite the char */
                    __putc(0x08);	/* go back again */
                    curpos--;
                }
                /* get record command */
                Esh_strcpy(cmd_buf, pt->cmd);
                Esh_printf("%s", cmd_buf);

                /*pt move forwards*/
                pt = pt->prior;

                curpos = Esh_strlen(cmd_buf);
                redir_flg = 0;
                break;
            }
            else
            {
                if (curpos < (ESH_MAX_CMD_LEN - 1))
                {
                    cmd_buf[curpos] = c;
                    /* echo it back out to the screen */
                    if (cmd_echo)
                        __putc(c);
                    curpos++;
                }
                break;
            }

        /*check direction key '↓' input*/
        case 'D':
            /* clear last time command */
            if (redir_flg )   /* direct char */
            {
                while( curpos )
                {
                    __putc(0x08);	/* go backwards */
                    __putc(' ');	/* overwrite the char */
                    __putc(0x08);	/* go back again */
                    curpos--;
                }
                Esh_strcpy(cmd_buf, pt->cmd);
                Esh_printf("%s", cmd_buf);

                /*pt move backwards*/
                pt = pt->next;

                curpos = Esh_strlen(cmd_buf);
                redir_flg = 0;
                break;
            }
            else       /* normal char */
            {
                if (curpos < (ESH_MAX_CMD_LEN - 1))
                {
                    cmd_buf[curpos] = c;
                    /* echo it back out to the screen */
                    if (cmd_echo)
                        __putc(c);
                    curpos++;
                }
                break;
            }

        case '\t':
            /* compensate name full under current directory name */
            /* clear last time cps string */
            while( curpos )
            {
                __putc(0x08);	/* go backwards */
                __putc(' ');	/* overwrite the char */
                __putc(0x08);	/* go back again */
                curpos--;
            }
            frt_compensate_name(cmd_buf, Esh_Global->PWdir, cps, &cps_length, &valid_length);
            Esh_printf("%s", (char *)(cmd_buf));
            curpos = Esh_strlen(cmd_buf);
            break;

        /*end of command*/
        case '\r':
        case '\n':
        case '\0':
            __putc('\r');
            __putc('\n');
            goto end_cmd;

        default:
            if (curpos < (ESH_MAX_CMD_LEN - 1))
            {
                cmd_buf[curpos] = c;
                /* echo it back out to the screen */
                if (cmd_echo)
                    __putc(c);
                curpos++;
            }
            break;
        }
    }
end_cmd:
    Esh_printf("\n\r");
    //Esh_printf("COMMAND: %s\n\r", cmd_buf);
}
/*
*****************************************************************************
*                            Esh_parseargs
*
*  Description: Parse user command line
*
*
*  Parameters: argstr:   输入待分析的命令串
*              *agrgc_p: 输入命令串的单词个数
*              **argv  ：输入命名串的单词记录
*
*  Return value:
*****************************************************************************
*/
/* parse token state */
enum ParseState
{
    PS_WHITESPACE,
    PS_TOKEN,
    PS_STRING,
    PS_ESCAPE
};
void Esh_parseargs(char *argstr, __s32 *argc_p, char **argv, char **resid)
{
    __s32 argc = 0;
    char c;
    enum ParseState stackedState = PS_WHITESPACE;
    enum ParseState lastState = PS_WHITESPACE;

    /* tokenize the argstr */
    while ((c = *argstr) != 0)
    {
        enum ParseState newState;

        if (c == ';' && lastState != PS_STRING && lastState != PS_ESCAPE)
            break;

        if (lastState == PS_ESCAPE)
        {
            newState = stackedState;
        }
        else if (lastState == PS_STRING)
        {
            if (c == '"')
            {
                newState = PS_WHITESPACE;
                *argstr = 0;
            }
            else
            {
                newState = PS_STRING;
            }
        }
        else if ((c == ' ') || (c == '\t'))
        {
            /* whitespace character */
            *argstr = 0;
            newState = PS_WHITESPACE;
        }
        else if (c == '"')
        {
            newState = PS_STRING;
            *argstr++ = 0;
            argv[argc++] = argstr;
            /*		} else if (c == '\\') {
            			stackedState = lastState;
            			newState = PS_ESCAPE;   */
        }
        else
        {
            /* token */
            if (lastState == PS_WHITESPACE)
            {
                argv[argc++] = argstr;
            }
            newState = PS_TOKEN;
        }
        lastState = newState;
        argstr++;
    }
    argv[argc] = NULL;
    if (argc_p != NULL)
        *argc_p = argc;

    if (*argstr == ';')
    {
        *argstr++ = '\0';
    }
    *resid = argstr;
}
/*
*****************************************************************************
*                            Esh_execute_one_command
*
*  Description:parse and execute a string
*
*  Parameters: buf  ---execute string pointer
*
*  Return value:None
*****************************************************************************
*/
static void Esh_execute_one_command(char *buf)
{
    int i;
    int argc;
    char *argv[ESH_MAX_CMD_ARGS];
    char *resid;

    /* init argv pointer table */
    for ( i = 0; i < ESH_MAX_CMD_ARGS; i++ )
    {
        argv[i] = NULL;
    }
    while (*buf)
    {

        /* parse input string */
        Esh_parseargs(buf, &argc, argv, &resid);

        if (argc > 0)
            Esh_execute_command((const char **)argv);
        buf = resid;
    }
}
/*
*****************************************************************************
*                            Esh_ReaderLoop
*
*  Description : Read one command, and execute command
*
*
*  Parameters : None
*
*  Return value : None
*
*****************************************************************************
*/
void Esh_ReaderLoop(void)
{
    static char cmd_buf[ESH_MAX_CMD_LEN];

    /* 命令记录链表初始化 */
    Esh_memset(Esh_CMD_Table, 0, sizeof(Esh_CMD_Table));
    Esh_Inithistory();

    Esh_Current_CMD = Esh_CMD_Table;                        /* 初始化命令位置 */

    /* read input commands loop */
    while( 1 )
    {
        /* check need exit or not first */
        if (Esh_Global->ExitFlag == ESH_TURE)
        {
            break;
        }

        Esh_printf( "%s>", Esh_Global->PWdir);
        __Esh_getcmd(cmd_buf, ESH_MAX_CMD_LEN);

        /* insert command to history list */
        __Esh_Insert_cmd(cmd_buf);

        if (cmd_buf[0])
            Esh_execute_one_command(cmd_buf);

        /* delay some time for a command */
        esKRNL_TimeDly(10);
    }
}
