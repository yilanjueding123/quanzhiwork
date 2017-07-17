/*
*********************************************************************************************************
*											        ePDK
*						            the Easy Portable/Player Develop Kits
*									          shell application
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File    : Esh_shell.h
* By      : sunny
* Version : V2.0                                                            2009.3.2
*********************************************************************************************************
*/

#ifndef _ESH_SHELL_H_
#define _ESH_SHELL_H_

#define     ESH_MAX_FILENAME        260     /* max file name length */

#define     ESH_MAX_CMD_LEN		    80      /* max command length */

#define     ESH_MAX_CMD_ARGS        10      /* max arguments number for one command */

#define     ESH_MAX_PRO_ARGS_LEN    64      /* max argument string length: passed to main function */
#define     ESH_MAX_PRO_ARGS_NUM    10      /* max argument number: passed to main function */

#define     ESH_AXF_TYPE_FILE       1       /* .axf type file */
#define     ESH_ESH_TYPE_FILE       2       /* .esh type file  */


#define     ESH_FAILED          1       /* Eshell failed  */
#define     ESH_SUCCEED         0       /* Eshell succeed */

#define     DOFAILED            1       /* do command failed  */
#define     DOSUCCEED           0       /* do commmad succeed */

#define		ESH_TURE			1
#define		ESH_FALSE			0

typedef  struct Esh_Global_Varible_t
{
    int  ExitFlag;                           /* exit flag 				*/
    int  ErrorNum;                           /* record error number 		*/
    char PWdir[ESH_MAX_FILENAME];            /* current work directory 	*/
    char AppPath1[ESH_MAX_FILENAME];         /* app seack path1 			*/
    char StartUpScript[ESH_MAX_FILENAME];    /* startup script file 		*/

} Esh_Global_Varible;

#endif  /* _ESH_SHELL_H_ */
