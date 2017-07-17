
#include "usb_host_config.h"

#include "usb_os_platform.h"

//#define	USB_DEBUG_MEM_STRING

//内存打印，用来跟踪执行路径
#ifdef	USB_DEBUG_MEM_STRING
#include <stdarg.h>

#define	USB_DEBUG_MEM_SIZE	 512//(1024 * 2)
static u32 usb_debug_mem_pos = 0;
char usb_debug_mem_str_buff[USB_DEBUG_MEM_SIZE];

#define  MASK_LOW4      0xf
#define  NEGATIVE       1
#define  POSITIVE       0
#define  HEX_x          'x'
#define  HEX_X          'X'

void usb_debug_mem_trace_input(  char *buff , u32 input_count);
/*
******************************************************************************
*
* Description:
*
* Parameters:
*
* Return value:
*
*
******************************************************************************
*/
static void _usb_debug_mem_trace_int_to_string_dec( int input , char *str)
{
    char stack[12];
    char sign_flag = POSITIVE ;      // 'sign_flag indicates wheater 'input' is positive or negative, default
    int i ;                           // value is 'POSITIVE'.
    int j ;


    if( input == 0 )
    {
        str[0] = '0';
        str[1] = '\0';                   // 'str' must end with '\0'
        return ;
    }

    if( input < 0 )                      // If 'input' is negative, 'input' is assigned to its absolute value.
    {
        sign_flag = NEGATIVE ;
        input = -input ;
    }

    for( i = 0; input > 0; ++i )
    {
        stack[i] = input % 10 + '0';    // characters in reverse order are put in 'stack' .
        input /= 10;
    }                                   // at the end of 'for' loop, 'i' is the number of characters.


    j = 0;
    if( sign_flag == NEGATIVE )
        str[j++] = '-';		            // If 'input' is negative, minus sign '-' is placed in the head.
    for( --i  ; i >= 0; --i, ++j )
        str[j] = stack[i];
    str[j] = '\0';				        // 'str' must end with '\0'

}

void _usb_debug_mem_trace_int_to_string_hex( int input, char *str, int hex_flag )
{
    int i;
    int j;
    char stack[8];

    int hgl[4] = {1,};

    i = hgl[0] ;
    for(i = 0; i < 8; ++i )
    {
        if( hex_flag == HEX_X )
        {
            stack[i] = "0123456789ABCDEF"[input & MASK_LOW4];
        }
        else
        {
            stack[i] = "0123456789abcdef"[input & MASK_LOW4];
        }

        input >>= 4;
    }

    str[0] = '0';
    str[1] = hex_flag;                         // Hex format shoud start with "0x" or "0X".
    for(i = 7, j = 2; i >= 0; --i, ++j)
    {
        str[j] = stack[i];
    }

    str[j] = '\0';                             // 'str' must end with '\0'
}

void _usb_debug_mem_trace_UART_puts_no_newline( const char *str )
{
    usb_debug_mem_trace_input((char *)str, strlen(str));
}


/*
******************************************************************************
*
* Description:
*
* Parameters:
*
* Return value:
*
*
******************************************************************************
*/
void usb_debug_mem_trace_init(void)
{
    void *phy_addr = 0;

    USB_OS_MEMSET(usb_debug_mem_str_buff, 0, USB_DEBUG_MEM_SIZE);
    usb_debug_mem_pos = 0;
    phy_addr = USB_OS_VA2PA(usb_debug_mem_str_buff);

    USBH_MEM_DEBUG("USBH_MEM_TRACE : buff phy addr = %x,vir addr = %x ,len = %xn",
                   phy_addr,
                   usb_debug_mem_str_buff,
                   USB_DEBUG_MEM_SIZE);
}

/*
******************************************************************************
*
* Description:
*
* Parameters:
*
* Return value:
*
*
******************************************************************************
*/
void usb_debug_mem_trace_reset(void)
{
    USB_OS_MEMSET(usb_debug_mem_str_buff, 0, USB_DEBUG_MEM_SIZE);
    usb_debug_mem_pos = 0;
}

void usb_debug_mem_trace_input(  char *buff , u32 input_count)
{
    if((usb_debug_mem_pos + input_count ) >= USB_DEBUG_MEM_SIZE)
    {
        USBH_MEM_DEBUG("PANIC : usb_debug_mem_trace() :overflow\n");
        return ;
    }

    USB_OS_MEMCPY(&(usb_debug_mem_str_buff[usb_debug_mem_pos]),
                  buff,
                  input_count);

    usb_debug_mem_pos += input_count;
}

/*
******************************************************************************
*
* Description:
*     打印字符串
* Parameters:
*
* Return value:
*
*
******************************************************************************
*/
void usb_debug_mem_trace_input_str(  char *buff )
{
    int str_len = strlen(buff);

    if((usb_debug_mem_pos + str_len ) > USB_DEBUG_MEM_SIZE)
    {
        USBH_MEM_DEBUG("PANIC : usb_debug_mem_trace() :overflow\n");
        return ;
    }

    usb_debug_mem_trace_input(buff, str_len);
}

/*
******************************************************************************
*
* Description:
*     打印字符串,外界输入长度
* Parameters:
*
* Return value:
*
*
******************************************************************************
*/
void usb_debug_mem_trace_input_str_len(  char *buff, int buff_len)
{
    int str_len = buff_len;
    if((usb_debug_mem_pos + str_len ) > USB_DEBUG_MEM_SIZE)
    {
        USBH_MEM_DEBUG("PANIC : usb_debug_mem_trace() :overflow\n");
        return ;
    }

    usb_debug_mem_trace_input(buff, str_len);
}

/*
******************************************************************************
*
* Description:
*     打印16进制字符串
* Parameters:
*
* Return value:
*
*
******************************************************************************
*/
void usb_debug_mem_trace_input_format_str_hex(int value)
{
    char temp[16];

    _usb_debug_mem_trace_int_to_string_hex(value, temp, HEX_x);
    usb_debug_mem_trace_input(temp, 10);
}


void usb_debug_mem_trace_output(void )
{
    int i = 0;
    if(usb_debug_mem_pos == 0)
    {
        return;
    }

    USBH_MEM_DEBUG("======\n usb_debug_mem_trace :\n");
    USBH_MEM_DEBUG("%s", usb_debug_mem_str_buff);
    USBH_MEM_DEBUG("\n======\n");
    usb_debug_mem_trace_init();
}



#else

void usb_debug_mem_trace_init(void)
{
}

void usb_debug_mem_trace_reset(void)
{
}

void usb_debug_mem_trace_input_str(char *buff )
{
}

void usb_debug_mem_trace_input_str_len(  char *buff, int buff_len)
{
}

void usb_debug_mem_trace_input_format_str_hex(int value)
{
}

void usb_debug_mem_trace_input(char *buff, u32 input_count)
{
}

void usb_debug_mem_trace_output(void )
{
}

#endif


