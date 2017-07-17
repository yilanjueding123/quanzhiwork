/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*                                     The command execute function
*
* File    : dohelp.c
* By      : sunny
* Version : V2.00                                                                         2009.3.3
*********************************************************************************************************
*/

#include    "Esh_shell_private.h"

/*
*****************************************************************************
*                            dohelp
*
*  Description: show help information
*
*
*  Parameters: None
*
*  Return value:None
*****************************************************************************
*/
int dohelp(struct op *t , const char **args)
{
    Esh_printf("==================================================================================\n");
    Esh_printf("=                      Help Information:  v2.0                                   =\n");
    Esh_printf("==================================================================================\n");
    Esh_printf("=  cat       <filename>   [/x]     : show specific text file content(/x: hex)    =\n");
    Esh_printf("=  cd        <dst_path>            : change current diretory                     =\n");
    Esh_printf("=  copy      <srcfile>   <dstfile> : copy one file                               =\n");
    Esh_printf("=  cpustatstart                    : start cpu stat                              =\n");
    Esh_printf("=  cpustatstop                     : stop cpu stat                               =\n");
    Esh_printf("=  cpustatreport                   : report cpu stat                             =\n");
    Esh_printf("=  create    <filename>            : creat a file                                =\n");
    Esh_printf("=  del       <filename>            : delete a file                               =\n");
    Esh_printf("=  dir                             : show content of current or specific dir     =\n");
    Esh_printf("=  fs                              : fs debug control                            =\n");
    Esh_printf("=  help                            : help information                            =\n");
    Esh_printf("=  history                         : display the history command                 =\n");
    Esh_printf("=  insmod    \\drv\\<drvname>      : install module                              =\n");
    Esh_printf("=  keysim                          : keyboard simulation                         =\n");
    Esh_printf("=  mkdir     <dirname>             : creat a direntory                           =\n");
    Esh_printf("=  mount     <driver>    <res>     : mount one specific driver                   =\n");
    Esh_printf("=  poweroff                        : power off                                   =\n");
    Esh_printf("=  quit                            : jump out shell   Be careful use 'quit'      =\n");
    Esh_printf("=  reset                           : reset the system                            =\n");
    Esh_printf("=  rmdir     <dirname              : delete a direntory                          =\n");
    Esh_printf("=  run       <programme>  [/w]     : run axf file under current or specific dir  =\n");
    Esh_printf("=  set                             : set the debug envirnment                    =\n");
    Esh_printf("=  setpath                         : set app seach path                          =\n");
    Esh_printf("=  src       <script file>         : execute one script file                     =\n");
    Esh_printf("=  startx                          : start x window                              =\n");
    Esh_printf("=  sysinfo                         : list process/thread info and memory info    =\n");
    Esh_printf("=  dump                            : list register from a to b   eg:dump 0xf1c0b00c 0xf1c0b010 =\n");
    Esh_printf("==================================================================================\n");

    return DOSUCCEED;
}

