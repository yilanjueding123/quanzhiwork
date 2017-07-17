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
* File    : doscript_private.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/
#ifndef  _DO_SCRIPT_PRIVATE_
#define  _DO_SCRIPT_PRIVATE_

#include  "Esh_script_area.h"              /* memory allocate                        */
#include  "Esh_script_domisc.h"            /* build_in_cmd                           */
#include  "Esh_script_executer.h"          /* Execute parser out command             */
#include  "Esh_script_lexer.h"             /* lex skan                               */
#include  "Esh_script_onecommand.h"        /* Get one command Execute it dynamic     */
#include  "Esh_script_parser.h"            /* parser syntax tree                     */
#include  "Esh_script_public.h"            /* public var and struct for all          */
#include  "Esh_script_var.h"               /* var support                            */
#include  "Esh_script_eval.h"              /* expand command, parse comamand args    */
#include  "Esh_script_external_var.h"      /* Declare All The External var           */
#include  "Esh_script_platform.h"          /* script paltform deal: just for epos    */

#include  "Esh_shell_private.h"

#endif  /* _DO_SCRIPT_PRIVATE_ */
