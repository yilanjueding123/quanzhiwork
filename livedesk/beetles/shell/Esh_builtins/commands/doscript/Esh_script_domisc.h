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
* File    : Esh_script_domisc.h
* By      : Sunny
* Version : v2.0                                                                            2009.3.3
**********************************************************************************************************************
*/

#ifndef  _ESH_SCRIPT_DO_MISC_H_
#define  _ESH_SCRIPT_DO_MISC_H_

struct OpType
{
    char *OpType_name;
    int OpType_val;
};
/* Arithmetic  */

#define OP_TYPE_ADD                     0       // +
#define OP_TYPE_SUB                     1       // -
#define OP_TYPE_MUL                     2       // *
#define OP_TYPE_DIV                     3       // /
//#define OP_TYPE_MOD                     4       // %

#define OP_TYPE_INC                     5      // ++
#define OP_TYPE_DEC                     6      // --


/* Logical */

//#define OP_TYPE_LOGICAL_AND             7      // &&
//#define OP_TYPE_LOGICAL_OR              8      // ||
//#define OP_TYPE_LOGICAL_NOT             9      // !

/* Relational */

#define OP_TYPE_EQUAL                   10      // ==
#define OP_TYPE_NOT_EQUAL               11      // !=
#define OP_TYPE_LESS                    12      // <
#define OP_TYPE_GREATER                 13      // >
#define OP_TYPE_LESS_EQUAL              14      // <=
#define OP_TYPE_GREATER_EQUAL           15      // >=

#endif      /* _ESH_SCRIPT_DO_MISC_H_ */
