#ifndef		_USB_UTILS_MEM_PRINT_H_
#define		_USB_UTILS_MEM_PRINT_H_



void usb_debug_mem_trace_init(void);
void usb_debug_mem_trace_reset(void);
void usb_debug_mem_trace_input_str(char *buff );
void usb_debug_mem_trace_input_str_len(  char *buff, int buff_len);
void usb_debug_mem_trace_input_format_str_hex(int value);
void usb_debug_mem_trace_input(char *buff, u32 input_count);
void usb_debug_mem_trace_output(void );


#endif
