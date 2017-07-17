#ifndef 	__CSI_INIT_H__
#define		__CSI_INIT_H__

#include "epdk.h"

void CSI_init_req(void);
void CSI_exit_req(void);
__u8  spi_write_chanelfreq(__u8 chanel);
__u8 Search_Freq_Pro(void);

#endif




