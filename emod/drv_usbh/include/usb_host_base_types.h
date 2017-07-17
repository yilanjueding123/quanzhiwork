/*
*******************************************************************************
*                                              usb host module
*
*                             Copyright(C), 2006-2008, SoftWinners Co., Ltd.
*											       All Rights Reserved
*
* File Name :
*
* Author : GLHuang(HoLiGun)
*
* Version : 1.0
*
* Date : 2008.05.xx
*
* Description :
*
* History :
********************************************************************************************************************
*/
#ifndef	_USB_HOST_BASE_TYPES_H_
#define	_USB_HOST_BASE_TYPES_H_

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char __s8;
typedef signed short __s16;
typedef signed int __s32;

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;

#define	USB_INLINE	__inline
#define inline __inline

#define readb(reg)						(*(volatile unsigned char *)(reg))
#define readw(reg)						(*(volatile unsigned short *)(reg))
#define readl(reg)						(*(volatile unsigned long *)(reg))

#define writeb(value,reg)				(*(volatile unsigned char *)(reg) = (value))
#define writew(value,reg)				(*(volatile unsigned short *)(reg) = (value))
#define writel(value,reg)				(*(volatile unsigned long *)(reg) = (value))
//===
#define set_bit_b( mask, reg) 			(writeb((readb(reg) | (mask)) , (reg)))
#define set_bit_w( mask, reg) 	 		(writew((readw(reg) | (mask)) , (reg)))
#define clear_bit_b( mask, reg) 	 	(writeb((readb(reg) & (~ (mask))) , (reg)))
#define clear_bit_w( mask, reg) 	 	(writew((readw(reg) & (~ (mask))) , (reg)))

#undef  set_bit
#define set_bit( value, bit )      		( (value) |=  ( 1U << (bit) ) )

#undef  clear_bit
#define clear_bit( value, bit )    		( (value) &= ~( 1U << (bit) ) )

#undef  reverse_bit
#define reverse_bit( value, bit )  		( (value) ^=  ( 1U << (bit) ) )

#undef  test_bit
#define test_bit( value, bit )     		( (value)  &  ( 1U << (bit) ) )


#undef  min
#define min( x, y )          			( (x) < (y) ? (x) : (y) )

#undef  max
#define max( x, y )          			( (x) > (y) ? (x) : (y) )

/* 取绝对值 */
#undef  absolute
#define absolute(p)        				((p) > 0 ? (p) : -(p))

#define	EPERM		 	1		/* Operation not permitted 							*/
#define	ENOENT		 	2		/* No such file or directory 						*/
#define	ESRCH		 	3		/* No such process 									*/
#define	EINTR		 	4		/* Interrupted system call 							*/
#define	EIO		 	 	5		/* I/O error 										*/
#define	ENXIO		 	6		/* No such device or address 						*/
#define	E2BIG		 	7		/* Argument list too long 							*/
#define	ENOEXEC		 	8		/* Exec format error 								*/
#define	EBADF		 	9		/* Bad file number 									*/
#define	ECHILD			10		/* No child processes 								*/
#define	EAGAIN			11		/* Try again 										*/
#define	ENOMEM			12		/* Out of memory 									*/
#define	EACCES			13		/* Permission denied 								*/
#define	EFAULT			14		/* Bad address 										*/
#define	ENOTBLK			15		/* Block device required 							*/
#define	EBUSY			16		/* Device or resource busy 							*/
#define	EEXIST			17		/* File exists 										*/
#define	EXDEV			18		/* Cross-device link 								*/
#define	ENODEV			19		/* No such device 									*/
#define	ENOTDIR			20		/* Not a directory 									*/
#define	EISDIR			21		/* Is a directory 									*/
#define	EINVAL			22		/* Invalid argument 								*/
#define	ENFILE			23		/* File table overflow 								*/
#define	EMFILE			24		/* Too many open files 								*/
#define	ENOTTY			25		/* Not a typewriter 								*/
#define	ETXTBSY			26		/* Text file busy 									*/
#define	EFBIG			27		/* File too large 									*/
#define	ENOSPC			28		/* No space left on device 							*/
#define	ESPIPE			29		/* Illegal seek 									*/
#define	EROFS			30		/* Read-only file system 							*/
#define	EMLINK			31		/* Too many links 									*/
#define	EPIPE			32		/* Broken pipe 										*/
#define	EDOM			33		/* Math argument out of domain of func 				*/
#define	ERANGE			34		/* Math result not representable 					*/

#define	ENOMSG			42		/* No message of desired type 						*/
#define	EL2HLT			51		/* Level 2 halted 									*/
#define	ESHUTDOWN		58		/* No send after transport endpoint shutdown 		*/
#define	EPROTO			71		/* Protocol error 									*/
#define	EOVERFLOW		75		/* Value too large for defined data type 			*/
#define	EMSGSIZE		90		/* Message too long 								*/
#define	EOPNOTSUPP		95		/* Operation not supported on transport endpoint	*/
#define	ECONNRESET		104		/* Connection reset by peer 						*/
#define	ENOTCONN		107		/* Transport endpoint is not connected 				*/
#define	ETIMEDOUT		110		/* Connection timed out 							*/
#define	EHOSTUNREACH	113		/* No route to host 								*/
#define	EINPROGRESS		115		/* Operation now in progress 						*/
#define	EREMOTEIO		181		/* Remote I/O error 								*/
#define ENOTSUPP		524		/* Operation is not supported 						*/
#define ECANCEL			600		/* Operation cancel by software						*/

//-------------------------------------------------------------------
//  USB 硬件部分
//-------------------------------------------------------------------
#define EFIFO       	700  	/* operation fifo failed 							*/
#define	EEPSTALL		701		/* usb ep stall 									*/
#define	EEPERROR		702		/* usb ep error 									*/
#define	EEPNAKOUT		702		/* usb ep nak out 									*/

#endif





