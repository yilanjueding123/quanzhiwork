/*
*********************************************************************************************************
*											        ePDK
*						           the Easy Portable/Player Development Kits
*									           shell sub-system
*
*						        (c) Copyright 2006-2007, Steven.ZGJ China
*											All	Rights Reserved
*
* File   : Esh_shell_private.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _ESH_SHELL_PRIVATE_H_
#define _ESH_SHELL_PRIVATE_H_

/* all the header files */
#include  "epdk.h"
#include  "apps.h"
#include  "Esh_shell.h"
#include  "Esh_init.h"
#include  "Esh_exit.h"
#include  "Esh_common.h"
#include  "Esh_support.h"
#include  "Esh_execute_cmd.h"
#include  "Esh_read_command.h"
#include  "Esh_error.h"
#include  "Esh_builtins.h"

#include  "doscript.h"
#include  "Esh_script_parser.h"
#include  "bmp_parser\\Parse_Picture.h"

extern __hdle logo_layer;
extern Picture_t *picture;

#endif  /* _ESH_SHELL_PRIVATE_H_ */
